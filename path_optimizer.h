// #ifndef p_hpp
// #define path_optimizer_hpp

// #include <stdio.h>

// #endif /* path_optimizer_hpp */

#include <cmath>
#include <algorithm>
#include <vector>

using namespace std;


enum class Algo_Type {
  MST,
  TSP_FAST,
  TSP_OPTIMAL,
};


enum class Location {
  Land,
  Water,
  Coast,
};

struct Coordinate {
  int64_t x;
  int64_t y;

  Location loc;

  Coordinate(int64_t x, int64_t y, Location loc) : x(x), y(y), loc(loc) {}
};

struct Coordinate1 {
  int64_t x;
  int64_t y;

  Coordinate1(int64_t x, int64_t y) : x(x), y(y) {}
};

