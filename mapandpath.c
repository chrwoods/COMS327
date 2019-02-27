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
  //print_map(rlg);
  //make paths
  generate_paths(rlg);
  //print nontunneling path
  //print_path(rlg->nt_path);
  //print tunneling path
  //print_path(rlg->t_path);
  //make monsters
  rlg->num_monsters = 1;
  //rlg->num_monsters = DEFAULT_MONSTERS;
  rlg->monsters = malloc(rlg->num_monsters * sizeof(struct monster));
  for(int i = 0; i < rlg->num_monsters; i++){
    generate_monster(rlg, i);
    (rlg->monsters)[i].type = 7;
    /*printf("Monster %d is type %d and:\n", i, (rlg->monsters)[i].type);
    char *spacenot[2] = {"", "not "};
    int yesno;
    if((rlg->monsters)[i].type & MON_SMART == MON_SMART) yesno = 0;
    else yesno = 1;
    printf("It is %ssmart. Constant %d. Bitwise %d. Equal? %d.\n", spacenot[yesno], MON_SMART, (rlg->monsters)[i].type & MON_SMART, (rlg->monsters)[i].type & MON_SMART == MON_SMART);
    if(((rlg->monsters)[i].type & MON_TELEPATHIC) == MON_TELEPATHIC) yesno = 0;
    else yesno = 1;
    printf("It is %stelepathic. Constant %d. Bitwise %d. Equal? %d.\n", spacenot[yesno], MON_TELEPATHIC, (rlg->monsters)[i].type & MON_TELEPATHIC, ((rlg->monsters)[i].type & MON_TELEPATHIC) == MON_TELEPATHIC);
    if(((rlg->monsters)[i].type & MON_TUNNEL) == MON_TUNNEL) yesno = 0;
    else yesno = 1;
    printf("It is %stunneling.\n", spacenot[yesno]);
    if((rlg->monsters)[i].type & MON_ERRATIC == MON_ERRATIC) yesno = 0;
    else yesno = 1;
    printf("It is %serratic.\n", spacenot[yesno]);*/
  }
  //print map
  print_map(rlg);
  //move monsters like 20 times
  for(int n = 0; n < 20; n++){
    //usleep(250000);
    for(int i = 0; i < rlg->num_monsters; i++){
      move(rlg, i);
    }
    print_map(rlg);
  }
  //print map
  //print_map(rlg);
  //deallocate memory
  empty_map(rlg);
}
