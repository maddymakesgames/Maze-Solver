#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "path_finding.h"
#include "vec.h"

void print_help() {
    printf(
        "Usage:\n\
mopsolver [-hdsp] [-i INFILE] [-o OUTFILE]\n\
Options:\n\
  -h          Print usage and options list to stdout only.    (Default: off)\n\
  -d          Pretty-print (display) the maze after reading.  (Default: off)\n\
  -s          Print length of shortest path or 'No solution'. (Default: off)\n\
  -p          Pretty-print maze with the path, if one exists. (Default: off)\n\
  -i infile   Read maze from infile.                          (Default: stdin)\n\
  -o outfile  Write all output to outfile.                    (Default: stdout)\n");
}

/// Holds the processed command line arguments
typedef struct CmdOpts {
    bool print_maze;
    bool print_len;
    bool print_path;
    FILE* in_file;
    FILE* out_file;
} CmdOpts;

bool read_args(CmdOpts* opt, int argc, const char** argv) {
    bool infile = false;
    bool outfile = false;
    for (int i = 1; i < argc; i++) {
        // if the last argument told us to read in a file
        // we open the file and go to the next argument
        if (infile) {
            opt->in_file = fopen(argv[i], "r");
            continue;
        }

        if (outfile) {
            opt->out_file = fopen(argv[i], "w");
            continue;
        }

        // If the argument doesn't start with - we have an invalid argument
        if (argv[i][0] != '-') return false;

        // set e to 1 to skip the - character
        for (size_t e = 1; e < strlen(argv[i]); e++) {
            switch (argv[i][e]) {
                case 'h':
                    print_help();
                    exit(EXIT_SUCCESS);
                case 'd':
                    opt->print_maze = true;
                    break;
                case 's':
                    opt->print_len = true;
                    break;
                case 'p':
                    opt->print_path = true;
                    break;
                case 'i':
                    infile = true;
                    break;
                case 'o':
                    outfile = true;
                    break;
                default:
                    return false;
            }
        }
    }

    return true;
}

const int LINE_CHUNK = 100;

Maze read_maze(CmdOpts* opts) {
    Maze m;
    m.width = 0;
    m.height = 0;

    if (opts->in_file == NULL || opts->out_file == NULL) exit(EXIT_FAILURE);

    int buf_size = LINE_CHUNK;
    unsigned char* buf = malloc(buf_size);
    int stream_position = ftell(opts->in_file);

    fgets((char*)buf, LINE_CHUNK, opts->in_file);

    int line_len = ftell(opts->in_file) - stream_position;

    // Read in the first line in chunks to find the width of the maze
    while (!feof(opts->in_file) && buf[line_len - 1] != '\n') {
        if (line_len >= LINE_CHUNK) {
            buf_size += LINE_CHUNK;
            buf = realloc(buf, buf_size);
        }

        fgets(
            ((char*)buf) + line_len, LINE_CHUNK - (line_len % LINE_CHUNK),
            opts->in_file);
        line_len = ftell(opts->in_file) - stream_position;
    }

    if (feof(opts->in_file)) {
        free(buf);
        exit(EXIT_FAILURE);
    }

    m.width = (line_len - 1);
    // Alias buf into m.map, not allowed to use buf after this
    m.map = buf;
    int bytes_read = m.width;

    // Read the rest of the maze line by line
    while (!feof(opts->in_file)) {
        ++m.height;
        m.map = realloc(m.map, bytes_read + m.width);
        fread(m.map + bytes_read, m.width, 1, opts->in_file);
        bytes_read += m.width;
        // advance off the newline
        fgetc(opts->in_file);
    }

    // We need to clean the map of ' ' chars
    // this also saves us some space in the heap
    Maze cleaned;
    cleaned.height = m.height;
    // Some maps have a space at the end of a row
    // We have to make sure we have the right width for both cases
    if (m.map[m.width - 1] == ' ') {
        cleaned.width = m.width / 2;
    } else {
        cleaned.width = (m.width + 1) / 2;
    }
    cleaned.map = malloc(cleaned.width * cleaned.height);

    for (unsigned int y = 0; y < cleaned.height; ++y) {
        for (unsigned int x = 0; x < cleaned.width; ++x) {
            cleaned.map[y * cleaned.width + x] = m.map[y * m.width + x * 2];
        }
    }

    free(m.map);

    return cleaned;
}

/// Prints the maze
void print_maze(Maze* maze) {
    printf("|-");
    for (unsigned int x = 0; x < maze->width; ++x) {
        printf("--");
    }
    printf("|\n");

    for (unsigned int y = 0; y < maze->height; ++y) {
        if (y != 0)
            printf("| ");
        else
            printf("  ");
        for (unsigned int x = 0; x < maze->width; ++x) {
            char c = get_pos(maze, x, y);
            switch (c) {
                case '0':
                    c = '.';
                    break;
                case '1':
                    c = '#';
                    break;
                case '2':
                    c = '+';
                    break;
            }

            printf("%c ", c);
        }
        if (y != maze->height - 1)
            printf("|\n");
        else
            printf(" \n");
    }

    printf("|-");
    for (unsigned int x = 0; x < maze->width; ++x) {
        printf("--");
    }
    printf("|\n");
}

int main(int argc, const char** argv) {
    CmdOpts options = (CmdOpts){
        .in_file = stdin,
        .out_file = stdout,
        .print_len = false,
        .print_maze = false,
        .print_path = false};
    // read in the cli arguments
    if (!read_args(&options, argc, argv)) {
        print_help();
        return EXIT_FAILURE;
    };

    Maze maze = read_maze(&options);

    if (options.print_maze) print_maze(&maze);

    // initialize the start and end points
    Point start_point = (Point){
        .x = 0,
        .y = 0,
        .parent = NULL,
        .heuristic = 0,
        .cost = 0,
    };
    Point end_point = (Point){
        .x = maze.width - 1,
        .y = maze.height - 1,
        .parent = NULL,
        .heuristic = 0,
        .cost = 0,
    };

    // find the path
    Vec path = get_path(&maze, start_point, end_point);

    if (options.print_len) {
        if (path != NULL) {
            printf("Solution in %d steps.\n", vec_len(path));
        } else {
            printf("No solution.\n");
        }
    }

    // Update the maze to include the path
    if (path != NULL) {
        while (!vec_empty(path)) {
            Point* p = vec_remove(path);
            if (options.print_path) maze.map[p->y * maze.width + p->x] = '2';
            free(p);
        }
        vec_free(path);

        if (options.print_maze && options.print_path) print_maze(&maze);
    }

    free(maze.map);

    // We can't fclose stdin or stdout
    if (options.in_file != stdin) fclose(options.in_file);
    if (options.out_file != stdout) fclose(options.out_file);

    return EXIT_SUCCESS;
}