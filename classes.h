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

class Room {
 public:
  uint8_t row;
  uint8_t col;
  uint8_t width;
  uint8_t height;

  Room(){
    //do nothing
  }
  
  Room(uint8_t w, uint8_t h){
    width = w;
    height = h;
  }
  
  Room(uint8_t r, uint8_t c, uint8_t w, uint8_t h){
    row = r;
    col = c;
    width = w;
    height = h;
  }

  void setLocation(uint8_t r, uint8_t c){
    row = r;
    col = c;
  }
};

class Obj {
 public:
  uint8_t row;
  uint8_t col;
  int seed; //used for pseudo-persistence, TODO

  Obj(){
    //do nothing
  }

  Obj(uint8_t r, uint8_t c){
    row = r;
    col = c;
    seed = -1;
  }

  Obj(uint8_t r, uint8_t c, int s){
    row = r;
    col = c;
    seed = s;
  }
};

class Monster {
 public:
  uint8_t row;
  uint8_t col;
  uint8_t type;
  uint8_t speed;
  uint8_t pc_row;
  uint8_t pc_col;
  
  Monster(){
    //do nothing
  }

  Monster(uint8_t r, uint8_t c, uint8_t t, uint8_t s, uint8_t pr, uint8_t pc){
    row = r;
    col = c;
    type = t;
    speed = s;
    pc_row = pr;
    pc_col = pc;
  }
};

class Dungeon {
 public:
  uint8_t map[HEIGHT][WIDTH];
  int nt_path[HEIGHT][WIDTH];
  int t_path[HEIGHT][WIDTH];
  int num_rooms;
  Room *rooms;
  int num_up;
  Obj *up;
  int num_down;
  Obj *down;
  Obj pc;
  int num_monsters;
  Monster *monsters;
  char background[HEIGHT][WIDTH];

  Dungeon(){
    rooms = (Room*)malloc(1);
    up = (Obj*)malloc(1);
    down = (Obj*)malloc(1);
    num_monsters = 0;
    monsters = (Monster*)malloc(1);
    pc.row = 0;
  }

  //from mapgen.h
  void generate_map_around_pc(int seed);
  void blur_map();
  void generate_first_room();
  void generate_room(int num);
  void generate_corridor(int start, int end);
  void generate_map(int seed);
  void generate_map();
  void empty_map();
  void old_print_map();

  //from mapsl.h
  void save_map(char* filepath);
  int load_map(char* filepath);
};