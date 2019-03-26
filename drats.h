//In case you're curious why this is called drats.h, it's because in cartoons they often say "Drats!" in place of "Curses!", often followed by some form of "Foiled again!". If you think that's a genius naming scheme for this file... you and me both, man.

void drats(){
  initscr(); //if you don't do this, the terrorists win
  cbreak(); //changes input to character-to-character basis
  noecho(); //hides characters from echoing to screen
  keypad(stdscr, TRUE); //allows arrow keys and special characters
  curs_set(FALSE); //bye bye mr. cursor
  nodelay(stdscr, TRUE); //sends key input instantly, doesn't buffer keys
  
  //pardon my french, but WINDOW objects can die in a fire
  //rlg->world = newwin(HEIGHT, WIDTH, 0, 0);
  //rlg->status = newwin(3, WIDTH, HEIGHT, 0);
}

void Dungeon::update_background(){
  //place rocks and empty spaces
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if(map[i][j] == 0) background[i][j] = '#';
      else background[i][j] = ' ';
    }
  }
  //place rooms
  for(int n = 0; n < num_rooms; n++){
    for(int i = 0; i < rooms[n].height; i++){
      for(int j = 0; j < rooms[n].width; j++){
	background[rooms[n].row + i][rooms[n].col + j] = '.';
      }
    }
  }
  //place up stairs
  for(int i = 0; i < num_up; i++){
    background[up[i].row][up[i].col] = '<';
  }
  //place down stairs
  for(int i = 0; i < num_down; i++){
    background[down[i].row][down[i].col] = '>';
  }
}

void Dungeon::update_fog(){
  //set visibility to false
  for(int i = -3; i <= 3; i++){
    for(int j = -3; j <= 3; j++){   
      if(pc.row + i < 0 || pc.row + i >= HEIGHT || pc.col + j < 0 || pc.col + j >= WIDTH) continue;
      visible[pc.row + i][pc.col + j] = false;
    }
  }
  //update in the eight directions, two spaces away (like a queen in chess)
  for(int i = -1; i <= 1; i++){
    for(int j = -1; j <= 1; j++){
      //get space close to pc
      if(pc.row + i < 0 || pc.row + i >= HEIGHT || pc.col + j < 0 || pc.col + j >= WIDTH) continue;
      char temp = background[pc.row + i][pc.col + j];
      memory[pc.row + i][pc.col + j] = temp;
      visible[pc.row + i][pc.col + j] = true;
      if(i == 0 && j == 0) continue;
      if(fog) mvaddch(pc.row + i, pc.col + j, temp);
      //if space close to pc is open, get space one farther away
      if(temp != ' '){
        temp = background[pc.row + 2 * i][pc.col + 2 * j];
	memory[pc.row + 2 * i][pc.col + 2 * j] = temp;
	visible[pc.row + 2 * i][pc.col + 2 * j] = true;
	if(fog) mvaddch(pc.row + 2 * i, pc.col + 2 * j, temp);
      }
    }
  }
  //update the last eight spaces, like a knight in chess
  for(int i = -1; i <= 1; i += 2){
    for(int j = -1; j <= 1; j += 2){
      if(background[pc.row + i][pc.col + j] == ' ') continue;
      char temp;
      if(background[pc.row + i][pc.col] != ' '){
	temp = background[pc.row + 2 * i][pc.col + j];
	memory[pc.row + 2 * i][pc.col + j] = temp;
	visible[pc.row + 2 * i][pc.col + j] = true;
	if(fog) mvaddch(pc.row + 2 * i, pc.col + j, temp);
      }
      if(background[pc.row][pc.col + j] != ' '){
	temp = background[pc.row + i][pc.col + 2 * j];
	memory[pc.row + i][pc.col + 2 * j] = temp;
	visible[pc.row + i][pc.col + 2 * j] = true;
	if(fog) mvaddch(pc.row + i, pc.col + 2 * j, temp);
      }
    }
  }
  //place monsters
  char monstersyms[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  for(int i = 0; i < num_monsters; i++){
    if(monsters[i].speed == 0) continue; //dead monster
    if(!visible[monsters[i].row][monsters[i].col]) continue; //too far
    mvaddch(monsters[i].row, monsters[i].col, monstersyms[monsters[i].type]);
  }
}

void Dungeon::print_fog(){
  //update remembered terrain
  update_fog();
  //place remembered terrain
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      mvaddch(i, j, memory[i][j]);
    }
  }
  //place player character
  mvaddch(pc.row, pc.col, '@');
  //place monsters
  char monstersyms[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  for(int i = 0; i < num_monsters; i++){
    if(monsters[i].speed == 0) continue; //dead monster
    if(!visible[monsters[i].row][monsters[i].col]) continue; //too far
    mvaddch(monsters[i].row, monsters[i].col, monstersyms[monsters[i].type]);
  }
  refresh();
}
      

void Dungeon::print_map(){
  if(fog){
    print_fog();
    return;
  }
  //wclear(rlg->world);
  //place background
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      mvaddch(i, j, background[i][j]);
    }
  }
  //place player character
  mvaddch(pc.row, pc.col, '@');
  //place monsters
  char monstersyms[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  for(int i = 0; i < num_monsters; i++){
    if(monsters[i].speed == 0) continue; //dead monster
    mvaddch(monsters[i].row, monsters[i].col, monstersyms[monsters[i].type]);
  }
  refresh();
}

void stard(){ //drats backwards
  //delwin(rlg->world);
  //delwin(rlg->status);
  endwin();
}

void update_status_text(char const * text){
  for(int i = 0; i < WIDTH; i++) mvaddch(HEIGHT, i, ' '); 
  mvprintw(HEIGHT, 0, text);
  refresh();
}
