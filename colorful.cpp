#include "classes.h"
#include "drats.h"
#include "mapgen.h"
#include "mapsl.h"
#include "priorityq.h"
#include "pathing.h"
#include "danger.h"
#include "ascii.h"
#include "hero.h"
#include "loader.h"
#include "itemize.h"

int main(int argc, char *argv[]){
  Dungeon rlg;
  int seed = -1; //negative seed = random
  int save = 0;
  int load = 0;
  int num_mons = DEFAULT_MONSTERS;
  int num_items = DEFAULT_ITEMS;
  if(argc > 1){
    for(int i = 1; i < argc; i++){
      if(std::strcmp(argv[i], "--save") == 0) save++;
      else if(std::strcmp(argv[i], "--load") == 0) load++;
      else if(std::strcmp(argv[i], "--seed") == 0){
	if(i + 1 >= argc) {
	  printf("No seed given after seed flag.\n");
	  return -1;
	}
	i++;
	if(atoi(argv[i]) == 0 && argv[i][0] != '0'){
	  printf("Argument directly after seed flag was not a numeric seed.\n");
	  return -1;
	}
	seed = atoi(argv[i]);
      }else if(std::strcmp(argv[i], "--nummon") == 0){
	if(i + 1 >= argc) {
	  printf("No number given after nummon flag.\n");
	  return -1;
	}
	i++;
	if(atoi(argv[i]) == 0 && argv[i][0] != '0'){
	  printf("Argument directly after nummon flag was not a number.\n");
	  return -1;
	}
	if(atoi(argv[i]) < 0){
	  printf("Given number of monsters cannot be negative.\n");
	  return -1;
	}
	num_mons = atoi(argv[i]);
      }else if(std::strcmp(argv[i], "--numitem") == 0){
	if(i + 1 >= argc) {
	  printf("No number given after numitem flag.\n");
	  return -1;
	}
	i++;
	if(atoi(argv[i]) == 0 && argv[i][0] != '0'){
	  printf("Argument directly after numitem flag was not a number.\n");
	  return -1;
	}
	if(atoi(argv[i]) < 0){
	  printf("Given number of items cannot be negative.\n");
	  return -1;
	}
	num_items = atoi(argv[i]);
      }else{
        printf("Flag \'%s\' unrecognized.\n", argv[i]);
        return -1;
      }
    }
  }
  if(seed < 0) seed = time(NULL);
  string home = getenv("HOME");
  char* filepath = strcat(getenv("HOME"), "/.rlg327/dungeon");
  if(!load) rlg.generate_map(seed);
  else {
    int loadreturn = rlg.load_map(filepath);
    if(loadreturn < 0) {
      printf("Error with loading map, ending program.\n");
      return loadreturn;
    }
  }
  if(save) rlg.save_map(filepath);
  rlg.generate_paths();
  //load monsters and items
  int loadreturn = rlg.load_monsters(home + "/.rlg327/monster_desc.txt");
  if(loadreturn < 0) {
    printf("Error with loading monsters, ending program.\n");
    return loadreturn;
  }
  loadreturn = rlg.load_items(home + "/.rlg327/object_desc.txt");
  if(loadreturn < 0) {
    printf("Error with loading items, ending program.\n");
    return loadreturn;
  }
  for(int i = 0; i < num_mons; i++){
    rlg.generate_monster();
  }
  for(int i = 0; i < num_items; i++){
    rlg.generate_item();
  }
  //print map
  drats();
  rlg.update_background();
  rlg.print_map();
  //move until the player is dead, probably (or the PC can win)
  int gamestate = 0;
  PQueue q(sizeof(int));
  int negative1 = -1; //don't laugh at this line please
  q.add(&negative1, floor(1000/PC_SPEED));
  for(int i = 0; i < rlg.monsters.size(); i++){
    int speed = (rlg.monsters[i].speed);
    q.add(&i, floor(1000/speed));
  }
  while(gamestate == 0){
    int num;
    int turn;
    q.pop(&num, &turn);
    if(num >= 0){
      if(rlg.monsters[num].dead()) continue; //dead monster, do not add back to queue
    }
    if(num < 0){
      if(rlg.pc_turn() < 0){
	gamestate = -1; //aborted
	break;
      }
    }
    else gamestate = rlg.move_monster(num);
    if(gamestate == 1){ 
      rlg.print_map();
      usleep(1000000); //let the player see that they have failed this city
      break;
    }
    if(num < 0){
      gamestate = 2;
      for(int i = 0; i < rlg.monsters.size(); i++){
	if(!rlg.monsters[i].dead()){
	  gamestate = 0;
	  break;
	}
      }
      if(gamestate == 2){
	usleep(1000000); //let the player bask in their own glory
	break;
      }
      q.add(&num, turn + 1000/PC_SPEED);
    } else {
      q.add(&num, turn + 1000/(rlg.monsters[num].speed));
    }
  }
  stard();
  q.empty();
  clear();
  if(gamestate == 2) printf("%s", treasure); //printf("You win!\n");
  else if(gamestate == 1) printf("%s", tombstone); //printf("You lose.\n");
  //deallocate memory
  rlg.empty_map();
}
