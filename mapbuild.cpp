#include "classes.h"
#include "mapgen.h"
#include "mapsl.h"

int main(int argc, char *argv[]){
  Dungeon rlg;
  int seed = -1; //negative seed = random
  int save = 0;
  int load = 0;
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
  rlg.old_print_map();
  rlg.empty_map();
  return 0;
}
