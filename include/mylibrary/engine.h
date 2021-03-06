//
// Created by Andy Lennox on 5/1/20.
//

#ifndef FINALPROJECT_ENGINE_H
#define FINALPROJECT_ENGINE_H


#include "player.h"

#include <Box2D/Box2D.h>

#include <chrono>



namespace fallingBalls {

enum class GameState {
  kPlaying,
  kPaused,
  kGameOver,
};

class Engine {

  // window dimensions
  const int kWIDTH = 800;
  const int kHEIGHT = 800;

  // pixels per meter
  const float PPM = 32.0f;

  // sandbox gravity
  const float kX_GRAV = 0.0f;
  const float kY_GRAV = 5.0f;

  // box2d engine step rates
  const float kTIME_STEP = 1.0 / 30.0;
  const float kVELOCITY_ITERATIONS = 8.0;
  const float kPOSITION_ITERATIONS = 3.0;

  // sandbox player and ground sizes
  const float kGROUND_HEIGHT = 10;
  const float kPLAYER_SIZE = 30;

  // ball spawn rate increase (ex. every kRATE_INCREASE_INTERVAL balls,
  // balls spawn kSPAWN_RATE_DECREASE seconds faster)
  const int kRATE_INCREASE_INTERVAL = 6;
  const float kSPAWN_RATE_DECREASE = .2;

  // ball bounciness
  const int kRESTITUTION = 1;


 public:
  explicit Engine(Player player);

  bool isStarted() const;
  bool isTwoPlayer() const;
  bool canSpawnBall() const;
  bool isTeleportEnabled() const;
  Player getPlayer() const;

  // update box2d world
  void Step();

  // delete old world and and create new world
  void reset();

  //draw circles and player
  void drawBodies();

  void addCircle(int x, int y, float radius, float density, float friction);
  void movePlayer(double direction);
  void pauseGame();
  void resumeGame();
  void startGame();
  void start2PGame();
  void enableTeleport();
  void disableTeleport();

  b2World* world;
  GameState state_;

  // ball radius constraints and min height player 2 can spawn at
  const int kBALL_MAX_RADIUS = 20;
  const int kBALL_MIN_RADIUS = 10;
  const int kMIN_BALL_HEIGHT = 100;

 private:
  // initialize world and fields, create player and ground box
  void initialize();

  // check for collision and end game if necessary
  // deletes out of bounds balls
  void updateBodies();

  Player player_;
  int seconds_paused{};
  std::chrono::time_point<std::chrono::system_clock> time_started;
  std::chrono::time_point<std::chrono::system_clock> time_paused;
  std::chrono::time_point<std::chrono::system_clock> last_ball_spawn;
  float ball_spawn_rate{};
  int num_balls_spawned{};
  bool hasStarted{};
  bool is_two_player_{};
  bool is_teleport_mode_{};
  bool can_spawn_ball_{};
};

} //namespace

#endif  // FINALPROJECT_ENGINE_H
