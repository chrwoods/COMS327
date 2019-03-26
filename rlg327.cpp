#include "classes.h"
#include "drats.h"
#include "mapgen.h"
#include "mapsl.h"
#include "priorityq.h"
#include "pathing.h"
#include "danger.h"
#include "ascii.h"
#include "hero.h"

int main(int argc, char *argv[]){
  Dungeon rlg;
  int seed = -1; //negative seed = random
  int save = 0;
  int load = 0;
  rlg.num_monsters = DEFAULT_MONSTERS;
  if(argc > 1){
    for(int i = 1; i < argc; i++){
      if(strcmp(argv[i], "--save") == 0) save++;
      else if(strcmp(argv[i], "--load") == 0) load++;
      else if(strcmp(argv[i], "--seed") == 0){
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
      }else if(strcmp(argv[i], "--nummon") == 0){
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
	rlg.num_monsters = atoi(argv[i]);
      }else{
        printf("Flag \'%s\' unrecognized.\n", argv[i]);
        return -1;
      }
    }
  }
  if(seed < 0) seed = time(NULL);
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
  rlg.monsters = (Monster*)realloc(rlg.monsters, rlg.num_monsters * sizeof(Monster));
  for(int i = 0; i < rlg.num_monsters; i++){
    rlg.generate_monster(i);
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
  for(int i = 0; i < rlg.num_monsters; i++){
    int speed = (rlg.monsters[i].speed);
    q.add(&i, floor(1000/speed));
  }
  while(gamestate == 0){
    int num;
    int turn;
    q.pop(&num, &turn);
    if(num >= 0){
      if(rlg.monsters[num].speed == 0) continue; //dead monster, do not add back to queue
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
      usleep(1000000); //let the player see that they have failed
      break;
    }
    if(num < 0){
      gamestate = 2;
      for(int i = 0; i < rlg.num_monsters; i++){
	if(rlg.monsters[i].speed != 0){
	  gamestate = 0;
	  break;
	}
      }
      q.add(&num, turn + 1000/PC_SPEED);
      //print_map(rlg);
      //print_monster_list(rlg);
      //usleep(250000);
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
