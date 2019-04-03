#include "classes.h"
#include "drats.h"
#include "loader.h"

int main(int argc, char* argv[]){
  Dungeon rlg;
  string filepath = getenv("HOME");
  filepath += "/.rlg327/monster_desc.txt";
  int loaded = rlg.load_monsters(filepath);
  if(loaded < 0){
    printf("Problem encountered with loading monsters, exiting.\n");
    return loaded;
  }
}
