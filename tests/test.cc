// Copyright (c) 2020 CS126SP20. All rights reserved.

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "mylibrary/engine.h"
#include "mylibrary/leaderboard.h"
#include "mylibrary/player.h"

using std::chrono::duration_cast;
using std::chrono::seconds;
using std::chrono::system_clock;
using namespace fallingBalls;

TEST_CASE("Leaderboard", "[leaderboard][player]") {
  LeaderBoard board = LeaderBoard("fallingBalls.db");
  Player player = Player("test", 10);

  SECTION("leaderboard maintains records") {
    board.AddScoreToLeaderBoard(player);
    std::vector<Player> high_scores = board.RetrieveHighScores(1);
    REQUIRE(!high_scores.empty());
  }

  SECTION("leaderboard adds scores") {
    player.score = 11;
    board.AddScoreToLeaderBoard(player);
    std::vector<Player> high_scores = board.RetrieveHighScores(player, 2);
    REQUIRE(high_scores.size() == 2);

    // check for correct ordering
    REQUIRE(high_scores[0].score == 11);
  }
}

TEST_CASE("Game Engine", "[engine][player]") {
  Player player = Player("test", 0);
  Engine engine = Engine(player);

  SECTION("Created Player") {
    REQUIRE(player.name == "test");
    REQUIRE(player.score == 0);
  }

  SECTION("Created Game Engine") {
    REQUIRE(engine.getPlayer().name == "test");
    REQUIRE(engine.getPlayer().score == 0);
    REQUIRE(engine.state_ == GameState::kPaused);
    REQUIRE(engine.isStarted() == false);
  }

  SECTION("Start 1P Game") {
    engine.startGame();
    REQUIRE(engine.isStarted() == true);
    REQUIRE(engine.isTwoPlayer() == false);
    REQUIRE(engine.state_ == GameState::kPlaying);
  }

  SECTION("Reset Game") {
    engine.reset();
    REQUIRE(engine.state_ == GameState::kPaused);
    REQUIRE(engine.isStarted() == false);
  }

  SECTION("Start 2P Game") {
    engine.start2PGame();
    REQUIRE(engine.isStarted() == true);
    REQUIRE(engine.isTwoPlayer() == true);
    REQUIRE(engine.state_ == GameState::kPlaying);
  }

  SECTION("Pause Game") {
    engine.pauseGame();
    REQUIRE(engine.state_ == GameState::kPaused);
  }

  SECTION("Resume Game") {
    engine.resumeGame();
    REQUIRE(engine.state_ == GameState::kPlaying);
  }

  SECTION("Collision Ended Game") {
    engine.addCircle(400, 400, 30, 1, .3);
    for (int32 i = 0; i < 60; ++i)
    {
      engine.Step();
    }
    REQUIRE(engine.state_ == GameState::kGameOver);
  }

  SECTION("2P Ball follow's spawn rate") {
    engine.reset();
    engine.start2PGame();
    engine.addCircle(0, 400, 30, 1, .3);
    std::chrono::time_point<std::chrono::system_clock> last_ball_spawn
    = system_clock::now();
    REQUIRE(engine.canSpawnBall() == false);

    while (duration_cast<seconds>(system_clock::now() - last_ball_spawn).count()
        < 10) {
      engine.Step();
    }

    REQUIRE(engine.canSpawnBall() == true);
  }

  SECTION("Move Player") {
    engine.reset();
    engine.startGame();

    //get player body
    b2Body* body = engine.world->GetBodyList();
    while (body) {
      if(body->GetUserData() == "player") {
        break;
      }
      body = body->GetNext();
    }

    // player starts at middle
    REQUIRE(body->GetPosition().x * 32 == 400);

    // player moves right
    engine.movePlayer(5);
    engine.Step();
    REQUIRE(body->GetPosition().x * 32 > 400);

    // player moves left
    engine.movePlayer(-20);
    engine.Step();
    REQUIRE(body->GetPosition().x * 32 < 400);
  }
}