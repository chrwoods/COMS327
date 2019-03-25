#include "classes.h"
#include "mapsl.h"
#include "priorityq.h"
#include "pathing.h"

int main(int argc, char *argv[]){
  Dungeon rlg;
  char* filepath = strcat(getenv("HOME"), "/.rlg327/dungeon");
  int loaded = rlg.load_map(filepath);
  if(loaded < 0){
    printf("Problem encountered with loading map, exiting.\n");
    return loaded;
  }
  //print_hex_map(rlg);
  //print_path((*rlg).map);
  /*int weight[HEIGHT][WIDTH];
  weight_nontunnel(rlg, weight);
  dijkstra(rlg, rlg->nt_path, weight);
  print_path(rlg->nt_path);
  weight_tunnel(rlg, weight);
  dijkstra(rlg, rlg->t_path, weight);
  print_path(rlg->t_path);*/
  rlg.generate_paths();
  print_path(rlg.nt_path);
  print_path(rlg.t_path);
  //deallocate memory
  /*free((*rlg).rooms);
  free((*rlg).up);
  free((*rlg).down);
  free(rlg);*/
  rlg.empty_map();
}
