#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <endian.h>
#define WIDTH 80
#define HEIGHT 21
#define MIN_ROOMS 6
#define MIN_ROOM_WIDTH 4
#define MIN_ROOM_HEIGHT 3 

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

struct dungeon {
  uint8_t map[HEIGHT][WIDTH];
  int num_rooms;
  struct room *rooms;
  int num_up;
  struct obj *up;
  int num_down;
  struct obj *down;
  struct obj pc;
};
