#include "structs.h"
#include "mapsl.h"
#include "pathing.h"

int main(int argc, char *argv[]){
  struct dungeon *rlg = malloc(sizeof(struct dungeon));
  char* filepath = strcat(getenv("HOME"), "/.rlg327/dungeon");
  int loaded = load_map(filepath, rlg);
  if(loaded < 0){
    printf("Problem encountered with loading map, exiting.\n");
    return loaded;
  }
  //print_hex_map(rlg);
  //print_path((*rlg).map);
  int weight[HEIGHT][WIDTH];
  int dist[HEIGHT][WIDTH];
  weight_nontunnel(rlg, weight);
  dijkstra(rlg, dist, weight);
  print_path(dist);
  weight_tunnel(rlg, weight);
  dijkstra(rlg, dist, weight);
  print_path(dist);
  //deallocate memory
  free((*rlg).rooms);
  free((*rlg).up);
  free((*rlg).down);
  free(rlg);
}
