#include "classes.h"
#include "drats.h"
#include "loader.h"

int main(int argc, char* argv[]){
  Dungeon rlg;
  string home = getenv("HOME");
  //load monsters
  string filepath = home + "/.rlg327/monster_desc.txt";
  int loaded = rlg.load_monsters(filepath);
  if(loaded < 0){
    printf("Problem encountered with loading monsters, exiting.\n");
    return loaded;
  }
  //print monsters
  rlg.print_monsters();
  //load items
  filepath = home + "/.rlg327/object_desc.txt";
  loaded = rlg.load_items(filepath);
  if(loaded < 0){
    printf("Problem encountered with loading items, exiting.\n");
    return loaded;
  }
  //print items
  rlg.print_items();
  rlg.empty_map();
}
