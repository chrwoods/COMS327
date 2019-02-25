#include "structs.h"
#include "mapgen.h"
#include "mapsl.h"
#include "pathing.h"

int main(int argc, char *argv[]){
  struct dungeon *rlg = malloc(sizeof(struct dungeon));
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
  if(!load) generate_map(rlg, seed);
  else {
    int loadreturn = load_map(filepath, rlg);
    if(loadreturn < 0) {
      printf("Error with loading map, ending program.\n");
      return loadreturn;
    }
  }
  if(save) save_map(filepath, rlg);
  //print normal map
  print_map(rlg);
  int weight[HEIGHT][WIDTH];
  //print nontunneling path
  weight_nontunnel(rlg, weight);
  dijkstra(rlg, rlg->nt_path, weight);
  print_path(rlg->nt_path);
  //print tunneling path
  weight_tunnel(rlg, weight);
  dijkstra(rlg, rlg->t_path, weight);
  print_path(rlg->t_path);
  //deallocate memory
  empty_map(rlg);
}
