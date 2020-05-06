//
// Created by Andy Lennox on 5/1/20.
//

#include "mylibrary/engine.h"

#include <cinder/gl/gl.h>

namespace fallingBalls {

using std::chrono::duration_cast;
using std::chrono::seconds;
using std::chrono::system_clock;

Engine::Engine(float x_gravity, float y_gravity, Player player)
    : player_(player) {
  // Set up world
  b2Vec2 gravity(x_gravity, y_gravity);
  world = new b2World(gravity);

  // Add ground
  b2BodyDef groundBodyDef;
  groundBodyDef.type = b2_staticBody;
  groundBodyDef.userData = (void *)"ground";
  groundBodyDef.position.Set(400.0f / PPM, 810.0f / PPM);
  b2Body* groundBody = world->CreateBody(&groundBodyDef);

  b2PolygonShape groundBox;
  groundBox.SetAsBox(400.0f / PPM, 10.0f / PPM);
  groundBody->CreateFixture(&groundBox, 0.0f);

  // Add player
  b2BodyDef playerBodyDef;
  playerBodyDef.userData = (void *)"player";
  playerBodyDef.type = b2_dynamicBody;
  playerBodyDef.position.Set(400.0f / PPM, 400.0f / PPM);
  b2Body* playerBody = world->CreateBody(&playerBodyDef);

  b2PolygonShape playerBox;
  playerBox.SetAsBox(15.0f / PPM, 15.0f / PPM);
  b2FixtureDef fixtureDef;
  fixtureDef.shape = &playerBox;
  fixtureDef.friction = 0;
  playerBody->CreateFixture(&fixtureDef);

  time_started = system_clock::now();
  last_ball_spawn = time_started;
  ball_spawn_rate = 1;
  num_balls_spawned = 0;
}

void Engine::Step() {
  world->Step(kTIME_STEP, kVELOCITY_ITERATIONS, kPOSITION_ITERATIONS);
  if (num_balls_spawned == kRATE_INCREASE_INTERVAL) {
    ball_spawn_rate -= .2;
    num_balls_spawned = 0;
  }
  if (duration_cast<seconds>(system_clock::now() - last_ball_spawn).count() > ball_spawn_rate) {
    float x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/800));
    float y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100));
    float radius = 10 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(kBALL_MAX_RADIUS-10)));
    addCircle(x, y, radius, 1, .3);
    ++num_balls_spawned;
    last_ball_spawn = system_clock::now();
  }

  b2Body* tmp = world->GetBodyList();
  while (tmp) {
    if (tmp->GetNext() == nullptr) {
      break;
    }
    b2Body* next = tmp->GetNext();
    b2Vec2 position = tmp->GetPosition();
    if (position.x > 800.0f / PPM || position.x < 0.0f / PPM) {
      if(tmp->GetUserData() == "player") {
        if (position.x > 800.0f / PPM) {
          tmp->SetLinearVelocity(b2Vec2(0,0));
        } else if (position.x < 0.0f / PPM) {
          tmp->SetLinearVelocity(b2Vec2(0,0));
        }
      } else {
        world->DestroyBody(tmp);
        tmp = next;
      }
    }

    if (tmp->GetUserData() == "player") {
      for (b2ContactEdge* edge = tmp->GetContactList(); edge;
           edge = edge->next) {
        b2Fixture* touching = edge->contact->GetFixtureB();
        if (b2CircleShape* c =
            dynamic_cast<b2CircleShape*>(touching->GetShape())) {
          gameEnded_ = true;
          player_.score = duration_cast<seconds>(system_clock::now() - time_started).count();
          break;
        }
      }

    }
    tmp = tmp->GetNext();
  }
}

void Engine::addCircle(int x, int y, float radius, float density,
                       float friction) {
  // Dynamic Circle
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position.Set(x / PPM, y / PPM);
  b2Body* body = world->CreateBody(&bodyDef);

  b2CircleShape dynamicCircle;
  dynamicCircle.m_radius = radius / PPM;

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &dynamicCircle;
  fixtureDef.restitution = 1;
  fixtureDef.density = density;
  fixtureDef.friction = friction;

  body->CreateFixture(&fixtureDef);
}

void Engine::drawBodies() {
  b2Body* tmp = world->GetBodyList();
  while (tmp) {
    if (tmp->GetNext() == nullptr) {
      break;
    }

    b2Vec2 position = tmp->GetPosition();
    if (b2CircleShape* c =
            dynamic_cast<b2CircleShape*>(tmp->GetFixtureList()->GetShape())) {
      cinder::gl::drawSolidCircle(
          cinder::vec2(position.x * PPM, position.y * PPM), c->m_radius * PPM);
    }

    if (b2PolygonShape* b =
        dynamic_cast<b2PolygonShape*>(tmp->GetFixtureList()->GetShape())) {
      float x = position.x * PPM;
      float y = position.y * PPM;
      cinder::Rectf rectf = cinder::Rectf(x - 15, y - 15, x + 15, y + 15);
      cinder::gl::drawSolidRect(rectf);
    }

    tmp = tmp->GetNext();
  }
}

Engine::~Engine() { delete world; }

Player Engine::getPlayer() const {
  return player_;
}
void Engine::movePlayer(int direction) {
  b2Body* tmp = world->GetBodyList();
  while (tmp) {
    if (tmp->GetNext() == nullptr) {
      break;
    }

    b2Vec2 position = tmp->GetPosition();
    if (b2PolygonShape* b =
        dynamic_cast<b2PolygonShape*>(tmp->GetFixtureList()->GetShape())) {
      tmp->SetLinearVelocity(b2Vec2(direction, 0));
    }

    tmp = tmp->GetNext();
  }
}

} // namespace
