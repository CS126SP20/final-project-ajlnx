// Copyright (c) 2020 [Your Name]. All rights reserved.

#include "my_app.h"

#include <cinder/app/App.h>
#include "mylibrary/engine.h"
#include "Box2D/Box2D.h"
#include "cinder/gl/gl.h"

namespace myapp {

using cinder::app::KeyEvent;
using namespace ci;
using namespace ci::app;


fallingBalls::Engine* engine;
const char kDbPath[] = "fallingBalls.db";
const size_t kLimit = 3;
const char kNormalFont[] = "Arial";
const char kBoldFont[] = "Arial Bold";
const char kDifferentFont[] = "Papyrus";

MyApp::MyApp()
  : leaderboard_{cinder::app::getAssetPath(kDbPath).string()} { }

void MyApp::setup() {
  fallingBalls::Player player = fallingBalls::Player("Andy", 0);
  engine = new fallingBalls::Engine( 0.0f, 5.0f, player);

}

void MyApp::update() {
  if (engine->gameEnded_) {
    if (top_players_.empty()) {
      fallingBalls::Player player = engine->getPlayer();
      leaderboard_.AddScoreToLeaderBoard({player.name, player.score});
      top_players_ = leaderboard_.RetrieveHighScores(kLimit);
      current_player_scores_
          = leaderboard_.RetrieveHighScores({player.name, player.score}, kLimit);


      // It is crucial the this vector be populated, given that `kLimit` > 0.
      assert(!top_players_.empty());
      assert(!current_player_scores_.empty());
    }
    return;
  }
  engine->Step();
}

void MyApp::draw() {
  if (engine->gameEnded_) {
    if (!printed_game_over_) cinder::gl::clear(Color(0, 0, 0));
    DrawGameOver();
    return;
  }
  gl::clear();
  gl::color(1,1,1);
  engine->drawBodies();


  //gl::color(1,0,0);
  //engine->getPlayer().drawPlayer();

}

void MyApp::keyDown(KeyEvent event) {
  switch (event.getCode()) {
    case KeyEvent::KEY_d:
    case KeyEvent::KEY_RIGHT:
      engine->movePlayer(7.5);
      break;

    case KeyEvent::KEY_a:
    case KeyEvent::KEY_LEFT:
      engine->movePlayer(-7.5);
      break;
  }
}

void MyApp::mouseDown(MouseEvent event) {
  int x = event.getX();
  int y = event.getY();
  engine->addCircle(x, y, 20, 1, .3);
}

void MyApp::mouseDrag(MouseEvent event) {
  int x = event.getX();
  int y = event.getY();
  engine->addCircle(x, y, 20, 1, .3);
}

template <typename C>
void MyApp::PrintText(const std::string& text, const C& color, const cinder::ivec2& size,
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
  const cinder::vec2 locp = {loc.x - box_size.x / 2, loc.y - box_size.y / 2};
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
  std::string score_text = "You Lasted " + std::to_string(engine->getPlayer().score) + " seconds!";
  PrintText(score_text, color, size, {center.x, 750});
  for (const fallingBalls::Player& player : top_players_) {
    std::stringstream ss;
    ss << player.name << " - " << player.score << " sec";
    PrintText(ss.str(), color, size, {center.x, center.y + (++row) * 50});
  }

  PrintText("Your High Scores", color, size, {center.x, 100});
  for (const fallingBalls::Player& player : current_player_scores_) {
    std::stringstream ss;
    ss << player.score  << " sec";
    PrintText(ss.str(), color, size, {center.x, (++row) * 50});
  }

  printed_game_over_ = true;
}

}  // namespace myapp
