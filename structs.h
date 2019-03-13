#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <endian.h>
#include <unistd.h>
#include <curses.h>
#define WIDTH 80
#define HEIGHT 21
#define MIN_ROOMS 6
#define MIN_ROOM_WIDTH 4
#define MIN_ROOM_HEIGHT 3 
#define DEFAULT_MONSTERS 10
#define PC_SPEED 10
#define MON_SMART 1
#define MON_TELEPATHIC 2
#define MON_TUNNEL 4
#define MON_ERRATIC 8

struct room {
  uint8_t row;
  uint8_t col;
  uint8_t width;
  uint8_t height;
};

struct obj {
  uint8_t row;
  uint8_t col;
};

struct monster {
  uint8_t row;
  uint8_t col;
  uint8_t type;
  uint8_t speed;
  uint8_t pc_row;
  uint8_t pc_col;
};

struct dungeon {
  uint8_t map[HEIGHT][WIDTH];
  int nt_path[HEIGHT][WIDTH];
  int t_path[HEIGHT][WIDTH];
  int num_rooms;
  struct room *rooms;
  int num_up;
  struct obj *up;
  int num_down;
  struct obj *down;
  struct obj pc;
  int num_monsters;
  struct monster *monsters;
  char background[HEIGHT][WIDTH];
  //WINDOW *world;
  //WINDOW *status;
};
