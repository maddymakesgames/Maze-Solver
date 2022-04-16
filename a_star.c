
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "path_finding.h"
#include "vec.h"

/// Sorts points by priority
int point_sort(void* a, void* b) {
    Point* pa = (Point*)a;
    Point* pb = (Point*)b;

    double priority = (pa->heuristic + pa->cost) - (pb->heuristic + pb->cost);
    // ensures that value is -1, 0, or 1 if |h| < 1
    return (int)(priority / fabs(priority));
}

/// calculates the heuristic of the point
static inline void heuristic(Point end, Point* curr) {
    curr->heuristic = (end.x - curr->x) + (end.y - curr->y);
}

/// gets the char at a location in an flattened array
static inline unsigned char get_location(
    unsigned char* arr, int x, int y, int width) {
    return arr[y * width + x];
}

/// sets the char at a location in an flattened array
static inline void set_location(
    unsigned char* arr, int x, int y, int width, unsigned char val) {
    arr[y * width + x] = val;
}

/// Gets the path using the A* algorithm
Vec get_path(Maze* maze, Point start, Point end) {
    // Store the points we have need to travel to
    Vec to_traverse = vec_new(point_sort);
    // and have already traversed
    Vec traversed = vec_new(NULL);
    // create an array to hold the places we have already traveled
    unsigned char paths[maze->height * maze->width];
    // clear the array
    memset(paths, 0, maze->width * maze->height);

    bool no_solution = false;

    // set the enterence to be traveled already
    set_location(paths, start.x, start.y, maze->width, 1);

    Point* curr_point = &start;

    // loop over neighbors and add them to the queue
    while (curr_point->x != end.x || curr_point->y != end.y) {
        vec_insert(traversed, curr_point);

        if (curr_point->x + 1 < maze->width &&
            get_pos(maze, curr_point->x + 1, curr_point->y) == '0' &&
            !get_location(
                paths, curr_point->x + 1, curr_point->y, maze->width)) {
            set_location(
                paths, curr_point->x + 1, curr_point->y, maze->width, 1);
            Point* right = malloc(sizeof(Point));
            right->x = curr_point->x + 1;
            right->y = curr_point->y;
            right->parent = curr_point;
            right->cost = curr_point->cost++;

            heuristic(end, right);
            vec_insert(to_traverse, right);
        }

        if (curr_point->x > 0 &&
            get_pos(maze, curr_point->x - 1, curr_point->y) == '0' &&
            !get_location(
                paths, curr_point->x - 1, curr_point->y, maze->width)) {
            set_location(
                paths, curr_point->x - 1, curr_point->y, maze->width, 1);
            Point* left = malloc(sizeof(Point));
            left->x = curr_point->x - 1;
            left->y = curr_point->y;
            left->parent = curr_point;
            left->cost = curr_point->cost++;

            heuristic(end, left);
            vec_insert(to_traverse, left);
        }

        if (curr_point->y + 1 < maze->height &&
            get_pos(maze, curr_point->x, curr_point->y + 1) == '0' &&
            !get_location(
                paths, curr_point->x, curr_point->y + 1, maze->width)) {
            set_location(
                paths, curr_point->x, curr_point->y + 1, maze->width, 1);
            Point* down = malloc(sizeof(Point));
            down->x = curr_point->x;
            down->y = curr_point->y + 1;
            down->parent = curr_point;
            down->cost = curr_point->cost++;

            heuristic(end, down);
            vec_insert(to_traverse, down);
        }

        if (curr_point->y > 0 &&
            get_pos(maze, curr_point->x, curr_point->y - 1) == '0' &&
            !get_location(
                paths, curr_point->x, curr_point->y - 1, maze->width)) {
            set_location(
                paths, curr_point->x, curr_point->y - 1, maze->width, 1);
            Point* up = malloc(sizeof(Point));
            up->x = curr_point->x;
            up->y = curr_point->y - 1;
            up->parent = curr_point;
            up->cost = curr_point->cost++;

            heuristic(end, up);
            vec_insert(to_traverse, up);
        }

        if (vec_empty(to_traverse)) {
            no_solution = true;
            break;
        }

        curr_point = vec_remove(to_traverse);
    }

    Vec path = NULL;

    if (!no_solution) {
        /// Create a new vec to hold the path
        path = vec_new(NULL);
        Point* curr_path = curr_point;

        while (curr_path != NULL) {
            Point* p = malloc(sizeof(Point));
            *p = *curr_path;
            vec_insert(path, p);
            curr_path = curr_path->parent;
        }

        free(curr_point);
    }

    // Free all the points
    while (!vec_empty(to_traverse)) {
        free(vec_remove(to_traverse));
    }

    while (!vec_empty(traversed)) {
        Point* p = vec_remove(traversed);
        if (p->x != 0 || p->y != 0) free(p);
    }

    vec_free(to_traverse);
    vec_free(traversed);

    return path;
}