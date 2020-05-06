//
// Created by Andy Lennox on 5/1/20.
//

#ifndef FINALPROJECT_ENGINE_H
#define FINALPROJECT_ENGINE_H

#include <Box2D/Box2D.h>

#include <chrono>

#include "direction.h"
#include "player.h"

namespace fallingBalls {

class Engine {
 public:
  Engine(float x_gravity, float y_gravity, Player player);
  ~Engine();
  void Step();
  void reset();
  void drawBodies();
  void addCircle(int x, int y, float radius, float density, float friction);
  Player getPlayer() const;
  void movePlayer(int direction);

  const float PPM = 32.0f;
  const float kTIME_STEP = 1.0 / 30.0;
  const float kVELOCITY_ITERATIONS = 8.0;
  const float kPOSITION_ITERATIONS = 3.0;
  b2World* world;
  bool gameEnded_;
  bool gameStarted_;
  bool gamePaused_;
  std::chrono::time_point<std::chrono::system_clock> time_started;
  std::chrono::time_point<std::chrono::system_clock> time_paused;
  std::chrono::time_point<std::chrono::system_clock> last_ball_spawn;
  float ball_spawn_rate;
  int num_balls_spawned;
  const int kRATE_INCREASE_INTERVAL = 6;
  const int kBALL_MAX_RADIUS = 20;

  void pauseGame();

  void resumeGame();

 private:
  Player player_;
  int seconds_paused;
};

} //namespace

#endif  // FINALPROJECT_ENGINE_H
