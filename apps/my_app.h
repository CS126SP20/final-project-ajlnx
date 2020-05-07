// Copyright (c) 2020 CS126SP20. All rights reserved.

#ifndef FINALPROJECT_APPS_MYAPP_H_
#define FINALPROJECT_APPS_MYAPP_H_

#include <Box2D/Box2D.h>
#include <cinder/app/App.h>
#include <imgui.h>
#include <mylibrary/leaderboard.h>

namespace myapp {

class MyApp : public cinder::app::App {

  const float kPLAYER_SPEED = 7.5;

  // how low can player 2 click
  const int kBALL_SPAWN_MIN_HEIGHT = 200;

 public:
  MyApp();
  void setup() override;
  void update() override;
  void draw() override;
  void keyDown(cinder::app::KeyEvent) override;
  void mouseDown(cinder::app::MouseEvent event) override;

  bool printed_game_over_;
  bool printed_high_scores_;
  template <typename C>
  void PrintText(const std::string& text, const C& color,
                 const glm::ivec2& size, const glm::vec2& loc);

 private:
  fallingBalls::LeaderBoard leaderboard_;
  std::vector<fallingBalls::Player> top_players_;
  std::vector<fallingBalls::Player> current_player_scores_;
  bool viewing_scores_;

  // draw leaderboard
  void DrawGameOver();
  void DrawHighScores();

  // draw main menu
  void DrawMenu();

  // reset booleans when appropriate due to menu selections
  void resetDrawings();
};

}  // namespace myapp

#endif  // FINALPROJECT_APPS_MYAPP_H_
