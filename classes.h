#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

#include <stdlib.h>
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
#define MON_PASS 16
#define MON_PICKUP 32
#define MON_DESTROY 64
#define MON_UNIQ 128
#define MON_BOSS 256

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

class Dice {
 public:
  int base;
  int dice;
  int sides;

  Dice(){
    //do nothing
  }
  
  Dice(int b, int d, int s){
    base = b;
    dice = d;
    sides = s;
  }

  Dice(string str){
    parse(str);
  }

  void parse(string str){
    base = stoi(str.substr(0, str.find_first_of('+')));
    dice = stoi(str.substr(str.find_first_of('+') + 1, str.find_first_of('d') - str.find_first_of('+')));
    sides = stoi(str.substr(str.find_first_of('d') + 1));
  }

  int roll(){
    int val = base;
    for(int i = 0; i < dice; i++){
      base += 1 + rand() % sides;
    }
    return val;
  }

  string toString(){
    string str = "";
    str += std::to_string(base);
    str += "+";
    str += std::to_string(dice);
    str += "d";
    str += std::to_string(sides);
    return str;
  }
};

class Monster;

class Lifeform {
 public:
  uint8_t row;
  uint8_t col;
  uint8_t pc_row;
  uint8_t pc_col;
  Monster *src; //reference to template
  //short int colors[8]; //max of 8 colors
  //short int num_colors;
  int speed;
  //uint16_t abilities;
  int hp;
  Dice damage;
  //char symbol;

  Lifeform(){
    //do nothing
  }
};

class Monster {
 public:
  string name;
  string desc;
  short int colors[8]; //max of 8 colors (technically 7, since black can't be placed
  short int num_colors;
  Dice speed;
  uint16_t abilities;
  Dice hp;
  Dice damage;
  char symbol;
  bool placed; //for unique monsters
  uint8_t rarity;
  
  Monster() {
    placed = false;
  }

  bool dead(){
    return false; //todo
  }

  bool smart(){ return (abilities & MON_SMART) == MON_SMART;}
  bool telepathic(){ return (abilities & MON_TELEPATHIC) == MON_TELEPATHIC;}
  bool tunnel(){ return (abilities & MON_TUNNEL) == MON_TUNNEL;}
  bool erratic(){ return (abilities & MON_ERRATIC) == MON_ERRATIC;}
  bool pass(){ return (abilities & MON_PASS) == MON_PASS;}
  bool pickup(){ return (abilities & MON_PICKUP) == MON_PICKUP;}
  bool destroy(){ return (abilities & MON_DESTROY) == MON_DESTROY;}
  bool unique(){ return (abilities & MON_UNIQ) == MON_UNIQ;}
  bool boss(){ return (abilities & MON_BOSS) == MON_BOSS;}

  Lifeform create(){
    Lifeform l;
    l.src = this;
    l.speed = speed.roll();
    l.hp = hp.roll();
    l.damage = damage;
    return l;
  }
};

class Item;

class Collectible {
 public:
  uint8_t row;
  uint8_t col;
  Item *src; //construction source
  short int color;
  int hit;
  Dice damage;
  int dodge;
  int def;
  int weight;
  int speed;
  int attr;
  int value;

  Collectible(){
    //do nothing
  }
};

class Item {
 public:
  string name;
  string desc;
  string type;
  short int color;
  Dice hit;
  Dice damage;
  Dice dodge;
  Dice def;
  Dice weight;
  Dice speed;
  int attr;
  Dice value;
  short int art; //0 if not artifact, 1 if artifact, >1 if placed
  uint8_t rarity;

  Item(){
    attr = 0;
  }

  Collectible create(){
    Collectible c;
    c.src = this;
    c.color = color;
    c.hit = hit.roll();
    c.damage = damage;
    c.dodge = dodge.roll(); //the ultimate evasive maneuver
    c.def = def.roll();
    c.weight = weight.roll();
    c.speed = speed.roll();
    c.attr = attr;
    c.value = value.roll();
    return c;
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
  vector<Monster> monster_templates;
  vector<Lifeform> monsters;
  vector<Item> item_templates;
  vector<Collectibles> items;
  char background[HEIGHT][WIDTH];
  bool fog;
  char memory[HEIGHT][WIDTH];
  bool visible[HEIGHT][WIDTH];
  
  Dungeon(){
    rooms = (Room*)malloc(1);
    up = (Obj*)malloc(1);
    down = (Obj*)malloc(1);
    num_monsters = 0;
    //monsters = (Monster*)malloc(1);
    pc.row = 0;
    fog = true;
  }

  void empty_map(){
    free(rooms);
    free(up);
    free(down);
    //free(monsters);
  }

  //from mapgen.h
  void generate_map_around_pc(int seed);
  void blur_map();
  void generate_first_room();
  void generate_room(int num);
  void generate_corridor(int start, int end);
  void generate_map(int seed);
  void generate_map();
  void old_print_map();

  //from mapsl.h
  void save_map(char* filepath);
  int load_map(char* filepath);

  //frxom pathing.h
  void weight_nontunnel(int weight[HEIGHT][WIDTH]);
  void weight_tunnel(int weight[HEIGHT][WIDTH]);
  void dijkstra(int dist[HEIGHT][WIDTH], int weight[HEIGHT][WIDTH]);
  void generate_paths();

  //from danger.h
  void generate_monster();
  int line_of_sight(int num);
  void kill_monster(int num);
  int move_monster(int num);
  void print_monster_list();

  //from drats.h
  void update_background();
  void update_fog();
  void print_fog();
  void print_map();

  //from hero.h
  int move_pc(int direction);
  int use_staircase(char stair);
  int pc_turn();
  void teleport();

  //from loader.h
  int load_monsters(string filepath);
  int load_monster(ifstream *fp);
  void print_monsters();
  void print_monster(int num);
  int load_items(string filepath);
  int load_item(ifstream *fp);
  void print_items();
  void print_item(int num);
};
