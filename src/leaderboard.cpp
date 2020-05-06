// Copyright (c) 2020 CS126SP20. All rights reserved.

#include <mylibrary/leaderboard.h>
#include <mylibrary/player.h>
#include <sqlite_modern_cpp.h>

#include <string>
#include <vector>

namespace fallingBalls {

using std::string;
using std::vector;

//// See examples: https://github.com/SqliteModernCpp/sqlite_modern_cpp/tree/dev

LeaderBoard::LeaderBoard(const string& db_path) : db_{db_path} {
  db_ << "CREATE TABLE if not exists leaderboard (\n"
         "  name  TEXT NOT NULL,\n"
         "  score INTEGER NOT NULL\n"
         ");";
}

void LeaderBoard::AddScoreToLeaderBoard(const Player& player) {
  // TODO(you): Add your query here.
  string name = player.name;
  int score = player.score;
  db_ << "insert into leaderboard (name,score) values (?,?);"
      << name
      << score;
}

vector<Player> GetPlayers(sqlite::database_binder* rows) {
  vector<Player> players;

  for (auto&& row : *rows) {
    string name;
    size_t score;
    row >> name >> score;
    Player player = {name, score};
    players.push_back(player);
  }

  return players;
}

vector<Player> LeaderBoard::RetrieveHighScores(const size_t limit) {
  // TODO(you): Add your query here.
  string lim = std::to_string(limit);
  auto rows = db_ << "select name,score from leaderboard order by score desc LIMIT " + lim + ";";
  return GetPlayers(&rows);
}

vector<Player> LeaderBoard::RetrieveHighScores(const Player& player,
                                               const size_t limit) {
  // TODO(you): Add your query here.
  string name = player.name;
  string lim = std::to_string(limit);
  auto rows = db_ << "select name,score from leaderboard WHERE name = ? order by score desc LIMIT " + lim + ";"
                  << name;
  return GetPlayers(&rows);
}

}  // namespace
