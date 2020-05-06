// Copyright (c) 2020 CS126SP20. All rights reserved.

#ifndef FINALPROJECT_PLAYER_H_
#define FINALPROJECT_PLAYER_H_

#include <string>

#include "location.h"

namespace fallingBalls {

struct Player {
  Player(const std::string& name, size_t score) : name(name), score(score) {}
  std::string name;
  size_t score;
};

} // namespace

#endif  // FINALPROJECT_PLAYER_H_
