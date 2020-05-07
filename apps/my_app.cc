// Copyright (c) 2020 Andy Lennox. All rights reserved.

#include "my_app.h"
#include "mylibrary/engine.h"

#include <cinder/app/App.h>
#include <Box2D/Box2D.h>
#include <cinder/gl/gl.h>
#include <CinderImGui.h>

namespace myapp {

using cinder::app::KeyEvent;
using namespace ci;
using namespace ci::app;

const char kDbPath[] = "fallingBalls.db";
const char kNormalFont[] = "Arial";
const size_t kLimit = 3;

fallingBalls::Engine* engine;


MyApp::MyApp()
  : leaderboard_{cinder::app::getAssetPath(kDbPath).string()} { }

void MyApp::setup() {
  std::cout << "Please enter your name: " << std::endl;
  std::string player_name;
  std::cin >> player_name;

  ImGui::initialize();
  fallingBalls::Player player = fallingBalls::Player(player_name, 0);
  engine = new fallingBalls::Engine(player);
  viewing_scores_ = false;
}

void MyApp::update() {
  if (engine->state_ == fallingBalls::GameState::kGameOver) {
    if (top_players_.empty()) {
      fallingBalls::Player player = engine->getPlayer();
      leaderboard_.AddScoreToLeaderBoard({player.name, player.score});
      top_players_ = leaderboard_.RetrieveHighScores(kLimit);
      current_player_scores_
          = leaderboard_.RetrieveHighScores({player.name, player.score}, kLimit);

      assert(!top_players_.empty());
      assert(!current_player_scores_.empty());
    }
    return;
  }

  if (engine->state_ == fallingBalls::GameState::kPaused) {
    return;
  }

  if (engine->state_ == fallingBalls::GameState::kPlaying) {
    engine->Step();
  }

}

void MyApp::draw() {
  Color black = Color::black();
  if (engine->state_ == fallingBalls::GameState::kGameOver) {
    if (!printed_game_over_) cinder::gl::clear(black);
    DrawGameOver();
    DrawMenu();
    return;
  }

  if(viewing_scores_) {
    if (!printed_high_scores_) cinder::gl::clear(black);
    DrawHighScores();
    DrawMenu();
    return;
  }

  gl::clear();
  DrawMenu();
  gl::color(1,1,1);
  engine->drawBodies();
}

void MyApp::DrawMenu() {
  bool window_active = true;
  ImGui::Begin("Main Menu", &window_active);
  ImGui::TextColored(ImVec4(1, 1, 0, 1),
      "Welcome to Falling Balls!");
  if (!(engine->state_ == fallingBalls::GameState::kPlaying)) {
    if (ImGui::Button("Start New Game")) {
      resetDrawings();
      engine->reset();
      engine->startGame();
      ImGui::SetWindowCollapsed(true);
    }
  }

  if (!(engine->state_ == fallingBalls::GameState::kPlaying)) {
    if (ImGui::Button("Start New 2P Game")) {
      resetDrawings();
      engine->reset();
      engine->start2PGame();
      ImGui::SetWindowCollapsed(true);
    }
  }

  if (engine->state_ == fallingBalls::GameState::kPlaying) {
    if (ImGui::Button("Pause")) {
      engine->pauseGame();
      ImGui::SetWindowCollapsed(true);
    }
  }

  if (engine->state_ == fallingBalls::GameState::kPaused &&
      engine->isStarted()) {
    if (ImGui::Button("Resume")) {
      engine->resumeGame();
      ImGui::SetWindowCollapsed(true);
    }
  }

  if (!engine->isStarted() && !viewing_scores_) {
    if (ImGui::Button("View Highscores")) {
      viewing_scores_ = true;
      ImGui::SetWindowCollapsed(true);
    }
  }
  ImGui::End();
}

void MyApp::keyDown(KeyEvent event) {
  switch (event.getCode()) {
    case KeyEvent::KEY_d:
    case KeyEvent::KEY_RIGHT:
      engine->movePlayer(kPLAYER_SPEED);
      break;

    case KeyEvent::KEY_a:
    case KeyEvent::KEY_LEFT:
      engine->movePlayer(-kPLAYER_SPEED);
      break;
  }
}

void MyApp::mouseDown(MouseEvent event) {
  if (engine->isTwoPlayer() && engine->canSpawnBall()) {
    int x = event.getX();
    int y = event.getY();
    if (y > kBALL_SPAWN_MIN_HEIGHT) {
      y = kBALL_SPAWN_MIN_HEIGHT;
    }
    float radius = engine->kBALL_MIN_RADIUS + static_cast <float> (rand())
        / ( static_cast <float> (RAND_MAX/(engine->kBALL_MAX_RADIUS
        - engine->kBALL_MIN_RADIUS)));
    engine->addCircle(x, y, radius, 1, .3);
  }
}

template <typename C>
void MyApp::PrintText(const std::string& text, const C& color,
    const cinder::ivec2& size,
    const cinder::vec2& loc) {
  cinder::gl::color(color);

  auto box = TextBox()
      .alignment(TextBox::CENTER)
      .font(cinder::Font(kNormalFont, 30))
      .size(size)
      .color(color)
      .backgroundColor(ColorA(0, 0, 0, 0))
      .text(text);

  const auto box_size = box.getSize();
  const cinder::vec2 locp = {loc.x - box_size.x / 2,
                             loc.y - box_size.y / 2};
  const auto surface = box.render();
  const auto texture = cinder::gl::Texture::create(surface);
  cinder::gl::draw(texture, locp);
}

void MyApp::DrawGameOver() {
  // Lazily print.
  if (printed_game_over_) return;
  if (top_players_.empty()) return;
  if (current_player_scores_.empty()) return;

  const cinder::vec2 center = getWindowCenter();
  const cinder::ivec2 size = {500, 50};
  const Color color = Color::white();

  size_t row = 0;
  PrintText("Game Over :(", color, size, center);
  std::string score_text = "You Lasted "
      + std::to_string(engine->getPlayer().score) + " seconds!";
  PrintText(score_text, color, size, {center.x, 750});

  // print high scores
  for (const fallingBalls::Player& player : top_players_) {
    std::stringstream ss;
    ss << player.name << " - " << player.score << " sec";
    PrintText(ss.str(), color, size, {center.x, center.y + (++row) * 50});
  }

  // print current player's high scores
  PrintText("Your High Scores", color, size, {center.x, 100});
  for (const fallingBalls::Player& player : current_player_scores_) {
    std::stringstream ss;
    ss << player.score  << " sec";
    PrintText(ss.str(), color, size, {center.x, (++row) * 50});
  }

  printed_game_over_ = true;
}

void MyApp::DrawHighScores() {
  // Lazily print.
  if (printed_high_scores_) return;

  const cinder::vec2 center = getWindowCenter();
  const cinder::ivec2 size = {500, 50};
  const Color color = Color::white();

  top_players_ = leaderboard_.RetrieveHighScores(kLimit);
  if (top_players_.empty()) {
    PrintText("No High Scores Yet!", color, size, center);
    return;
  }

  size_t row = 0;
  PrintText("High Scores", color, size, center);
  for (const fallingBalls::Player& player : top_players_) {
    std::stringstream ss;
    ss << player.name << " - " << player.score << " sec";
    PrintText(ss.str(), color, size, {center.x, center.y + (++row) * 50});
  }

  printed_high_scores_ = true;
}

void MyApp::resetDrawings() {
  viewing_scores_ = false;
  printed_game_over_ = false;
  printed_high_scores_ = false;
}

}  // namespace myapp
