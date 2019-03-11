//In case you're curious why this is called drats.h, it's because in cartoons they often say "Drats!" in place of "Curses!", often followed by some form of "Foiled again!". If you think that's a genius naming scheme for this file... you and me both, man.

#include <curses.h>

void drats(){
  initscr(); //if you don't do this, the terrorists win
  cbreak(); //changes input to character-to-character basis
  noecho(); //hides characters from echoing to screen
  keypad(stdscr, TRUE); //allows arrow keys and special characters
}

void print_map(struct dungeon *rlg){
  clear();
  //place rocks and empty spaces
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if((*rlg).map[i][j] == 0) mvaddch(i, j, '#');
    }
  }
  //place rooms
  for(int n = 0; n < (*rlg).num_rooms; n++){
    for(int i = 0; i < (*rlg).rooms[n].height; i++){
      for(int j = 0; j < (*rlg).rooms[n].width; j++){
	mvaddch((*rlg).rooms[n].row + i, (*rlg).rooms[n].col + j, '.');
      }
    }
  }
  //place up stairs
  for(int i = 0; i < (*rlg).num_up; i++){
    mvaddch((*rlg).up[i].row, (*rlg).up[i].col, '<');
  }
  //place down stairs
  for(int i = 0; i < (*rlg).num_down; i++){
    mvaddch((*rlg).down[i].row, (*rlg).down[i].col, '>');
  }
  //place player character
  mvaddch((*rlg).pc.row, (*rlg).pc.col, '@');
  //place monsters
  char monstersyms[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  for(int i = 0; i < rlg->num_monsters; i++){
    if((rlg->monsters)[i].speed == 0) continue; //dead monster
    mvaddch((rlg->monsters)[i].row, (rlg->monsters)[i].col, monstersyms[(rlg->monsters)[i].type]);
  }
  refresh();
}