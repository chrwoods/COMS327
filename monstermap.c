#include "structs.h"
#include "mapgen.h"
#include "mapsl.h"
#include "pathing.h"
#include "danger.h"

int main(int argc, char *argv[]){
  struct dungeon *rlg = malloc(sizeof(struct dungeon));
  int seed = -1; //negative seed = random
  int save = 0;
  int load = 0;
  rlg->num_monsters = DEFAULT_MONSTERS;
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
      }else if(strcmp(argv[i], "--nummons") == 0){
	if(i + 1 >= argc) {
	  printf("No number given after nummons flag.\n");
	  return -1;
	}
	i++;
	if(atoi(argv[i]) == 0 && argv[i][0] != '0'){
	  printf("Argument directly after nummons flag was not a number.\n");
	  return -1;
	}
	if(atoi(argv[i]) < 0){
	  printf("Given number of monsters cannot be negative.\n");
	  return -1;
	}
	rlg->num_monsters = atoi(argv[i]);
      }else{
        printf("Flag \'%s\' unrecognized.\n", argv[i]);
        return -1;
      }
    }
  }
  if(seed < 0) seed = time(NULL);
  char* filepath = strcat(getenv("HOME"), "/.rlg327/dungeon");
  if(!load) generate_map(rlg, seed);
  else {
    int loadreturn = load_map(filepath, rlg);
    if(loadreturn < 0) {
      printf("Error with loading map, ending program.\n");
      return loadreturn;
    }
  }
  if(save) save_map(filepath, rlg);
  generate_paths(rlg);
  rlg->monsters = malloc(rlg->num_monsters * sizeof(struct monster));
  for(int i = 0; i < rlg->num_monsters; i++){
    generate_monster(rlg, i);
  }
  //print map
  print_map(rlg);
  //move until the player is dead, probably (or the PC can win)
  int gamestate = 0;
  while(gamestate == 0){
    usleep(250000);
    for(int i = -1; i < rlg->num_monsters; i++){
      gamestate = move(rlg, i);
      if(gamestate == 1) break;
    }
    if(rlg->num_monsters == 0) gamestate = 2;
    print_map(rlg);
  }
  if(gamestate == 2) printf("You win!\n");
  else if(gamestate == 1) printf("You lose.\n");
  //deallocate memory
  empty_map(rlg);
}
