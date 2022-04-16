#include <stdbool.h>

#include "vec.h"

#ifndef __PATH_FINDING_HEADER__
#define __PATH_FINDING_HEADER__

/// Represents the Maze
typedef struct Maze {
    unsigned int width;
    unsigned int height;
    /// A flattened 2d array of the map
    /// '1' is a wall
    /// '0' is empty
    /// '2' is the path
    unsigned char* map;
} Maze;

/// Gets a character inside a maze
static inline unsigned char get_pos(Maze* maze, int x, int y) {
    return maze->map[y * maze->width + x];
}

/// Represents a point in the maze
typedef struct Point {
    unsigned int x;
    unsigned int y;
    /// The point that we came from
    struct Point* parent;
    /// The distance from the exit
    unsigned int heuristic;
    /// The cost of taking this point
    unsigned int cost;
} Point;

/// Returns a vec containing the shortest path to solve the maze
Vec get_path(Maze* maze, Point start, Point end);

#endif