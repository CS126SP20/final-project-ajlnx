//
// Created by Andy Lennox on 5/1/20.
//

#include "mylibrary/engine.h"

#include <cinder/gl/gl.h>

#include <utility>

namespace fallingBalls {

using std::chrono::duration_cast;
using std::chrono::seconds;
using std::chrono::system_clock;

Engine::Engine(Player player)
    : player_(std::move(player)) {
  // Set up world
  b2Vec2 gravity(kX_GRAV,kY_GRAV);
  world = new b2World(gravity);

  initialize();
}

void Engine::startGame() {
  hasStarted = true;
  is_two_player_ = false;
  state_ = GameState::kPlaying;
  time_started = system_clock::now();
  state_ = GameState::kPlaying;
  last_ball_spawn = time_started;
}

void Engine::start2PGame() {
  hasStarted = true;
  is_two_player_ = true;
  state_ = GameState::kPlaying;
  time_started = system_clock::now();
  state_ = GameState::kPlaying;
  last_ball_spawn = time_started;
}

void Engine::pauseGame() {
  state_ = GameState::kPaused;
  time_paused = system_clock::now();
}

void Engine::resumeGame() {
  state_ = GameState::kPlaying;
  seconds_paused +=
      duration_cast<seconds>(system_clock::now() - time_paused).count();
}

void Engine::reset() {
  // delete old world and bodies
  b2Body* body = world->GetBodyList();
  while (body) {
    b2Body* next = body->GetNext();
    world->DestroyBody(body);
    body = next;
  }
  delete world;

  //create new world
  b2Vec2 gravity(kX_GRAV, kY_GRAV);
  world = new b2World(gravity);
  initialize();
}

void Engine::addCircle(int x, int y, float radius, float density,
                       float friction) {
  // reset ball timer
  if (is_two_player_) {
    can_spawn_ball_ = false;
  }

  // Create Dynamic Circle
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position.Set(x / PPM, y / PPM);
  b2Body* body = world->CreateBody(&bodyDef);
  b2CircleShape dynamicCircle;
  dynamicCircle.m_radius = radius / PPM;
  b2FixtureDef fixtureDef;
  fixtureDef.shape = &dynamicCircle;
  fixtureDef.restitution = kRESTITUTION;
  fixtureDef.density = density;
  fixtureDef.friction = friction;

  body->CreateFixture(&fixtureDef);
}

void Engine::drawBodies() {
  b2Body* body = world->GetBodyList();
  while (body) {
    if (body->GetNext() == nullptr) {
      break;
    }

    b2Vec2 position = body->GetPosition();
    if (b2CircleShape* c =
        dynamic_cast<b2CircleShape*>(body->GetFixtureList()->GetShape())) {
      cinder::gl::drawSolidCircle(
          cinder::vec2(position.x * PPM,
                       position.y * PPM), c->m_radius * PPM);
    }

    if (body->GetUserData() == "player") {
      float x = position.x * PPM;
      float y = position.y * PPM;
      int offset = kPLAYER_SIZE / 2;
      cinder::Rectf rectf = cinder::Rectf(x - offset, y - offset,
                                          x + offset, y + offset);
      cinder::gl::drawSolidRect(rectf);
    }

    body = body->GetNext();
  }
}

void Engine::Step() {
  world->Step(kTIME_STEP, kVELOCITY_ITERATIONS, kPOSITION_ITERATIONS);

  if (num_balls_spawned == kRATE_INCREASE_INTERVAL) {
    ball_spawn_rate -= kSPAWN_RATE_DECREASE;
    num_balls_spawned = 0;
  }

  if (duration_cast<seconds>(system_clock::now() - last_ball_spawn).count()
  > ball_spawn_rate) {
    if (is_two_player_) {
      can_spawn_ball_ = true;
      ++num_balls_spawned;
      last_ball_spawn = system_clock::now();
    } else {
      float x = static_cast <float> (rand())
          / (static_cast <float> (RAND_MAX / kWIDTH));
      float y = static_cast <float> (rand())
          / (static_cast <float> (RAND_MAX / kMIN_BALL_HEIGHT));
      float radius = kBALL_MIN_RADIUS + static_cast <float> (rand())
          / ( static_cast <float> (RAND_MAX/(kBALL_MAX_RADIUS - kBALL_MIN_RADIUS)));
      addCircle(x, y, radius, 1, .3);
      ++num_balls_spawned;
      last_ball_spawn = system_clock::now();
    }
  }

  // ensures player isn't out of bounds, checks for collision,
  // and delete out of bounds balls
  updateBodies();
}

void Engine::movePlayer(double direction) {
  b2Body* body = world->GetBodyList();
  while (body) {
    if (body->GetUserData() == "player") {
      body->SetLinearVelocity(b2Vec2(direction, 0));
    }
    body = body->GetNext();
  }
}

Player Engine::getPlayer() const {
  return player_;
}

bool Engine::isTwoPlayer() const { return is_two_player_; }

bool Engine::canSpawnBall() const { return can_spawn_ball_; }

bool Engine::isStarted() const { return hasStarted; }

void Engine::initialize() {
  // Add ground
  b2BodyDef groundBodyDef;
  groundBodyDef.type = b2_staticBody;
  groundBodyDef.userData = (void *)"ground";
  groundBodyDef.position.Set((kWIDTH / 2.0) / PPM,
      (kHEIGHT + 10) / PPM);
  b2Body* groundBody = world->CreateBody(&groundBodyDef);
  b2PolygonShape groundBox;
  groundBox.SetAsBox((kWIDTH / 2.0) / PPM, kGROUND_HEIGHT / PPM);
  groundBody->CreateFixture(&groundBox, 0.0f);

  // Add player
  b2BodyDef playerBodyDef;
  playerBodyDef.userData = (void *)"player";
  playerBodyDef.type = b2_dynamicBody;
  playerBodyDef.position.Set((kWIDTH / 2.0) / PPM, (kHEIGHT / 2.0) / PPM);
  b2Body* playerBody = world->CreateBody(&playerBodyDef);
  b2PolygonShape playerBox;
  playerBox.SetAsBox((kPLAYER_SIZE / 2) / PPM, (kPLAYER_SIZE / 2) / PPM);
  b2FixtureDef fixtureDef;
  fixtureDef.shape = &playerBox;
  fixtureDef.friction = 0;
  playerBody->CreateFixture(&fixtureDef);

  // initialize fields
  ball_spawn_rate = 1;
  num_balls_spawned = 0;
  seconds_paused = 0;
  state_ = GameState::kPaused;
}

void Engine::updateBodies() {
  b2Body* body = world->GetBodyList();
  while (body) {
    if (body->GetNext() == nullptr) {
      break;
    }
    b2Body* next = body->GetNext();
    b2Vec2 position = body->GetPosition();

    if(body->GetUserData() == "player") {
      if (position.x > (kWIDTH - kPLAYER_SIZE) / PPM) {
        body->SetLinearVelocity(b2Vec2(0, 0));
      } else if (position.x < kPLAYER_SIZE / PPM) {
        body->SetLinearVelocity(b2Vec2(0, 0));
      }
    }

    if (position.x > kWIDTH / PPM || position.x < 0.0f / PPM) {
        world->DestroyBody(body);
        body = next;
    }

    if (body->GetUserData() == "player") {
      for (b2ContactEdge* edge = body->GetContactList(); edge;
           edge = edge->next) {
        b2Fixture* touching = edge->contact->GetFixtureB();
        if (b2CircleShape* c =
            dynamic_cast<b2CircleShape*>(touching->GetShape())) {
          state_ = GameState::kGameOver;
          player_.score = std::__1::chrono::duration_cast<seconds>(system_clock::now()
                                                                   - time_started).count() -
                          seconds_paused;
          break;
        }
      }

    }
    body = body->GetNext();
  }
}

} // namespace
