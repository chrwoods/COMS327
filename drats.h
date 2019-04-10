//In case you're curious why this is called drats.h, it's because in cartoons they often say "Drats!" in place of "Curses!", often followed by some form of "Foiled again!". If you think that's a genius naming scheme for this file... you and me both, man.

#define BLUE_PAIR 1
#define GREEN_PAIR 2
#define CYAN_PAIR 3
#define RED_PAIR 4
#define MAGENTA_PAIR 5
#define YELLOW_PAIR 6
#define WHITE_PAIR 7
#define BLACK_PAIR 8
#define GRAY_PAIR 9
#define GREY_PAIR 9 //this is for the brits

void drats(){
  initscr(); //if you don't do this, the terrorists win
  cbreak(); //changes input to character-to-character basis
  noecho(); //hides characters from echoing to screen
  keypad(stdscr, TRUE); //allows arrow keys and special characters
  curs_set(FALSE); //bye bye mr. cursor
  nodelay(stdscr, TRUE); //sends key input instantly, doesn't buffer keys
  if(has_colors()) start_color(); //everything the light touches is yours, simba (note: from here on out we assume that the terminal supports colors, because it's 2019 and if it doesn't, the program deserves not to compile.)
  init_pair(BLUE_PAIR, COLOR_BLUE + 8, COLOR_BLACK);
  init_pair(GREEN_PAIR, COLOR_GREEN + 8, COLOR_BLACK);
  init_pair(CYAN_PAIR, COLOR_CYAN + 8, COLOR_BLACK);
  init_pair(RED_PAIR, COLOR_RED + 8, COLOR_BLACK);
  init_pair(MAGENTA_PAIR, COLOR_MAGENTA + 8, COLOR_BLACK);
  init_pair(YELLOW_PAIR, COLOR_YELLOW + 8, COLOR_BLACK);
  init_pair(WHITE_PAIR, COLOR_WHITE + 8, COLOR_BLACK);
  init_pair(BLACK_PAIR, COLOR_BLACK + 8, COLOR_BLACK); //identical to gray currently
  init_pair(GRAY_PAIR, COLOR_BLACK + 8, COLOR_BLACK); //the 8 is for intensity bit
  
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
  //place items
  for(int i = 0; i < items.size(); i++){
    char bg = background[items[i].row][items[i].col];
    if(bg == '.' || bg == '#') background[items[i].row][items[i].col] = items[i].src->symbol; //checks if placing item where an item is not
    else background[items[i].row][items[i].col] = '&'; //stack
  }
}

void Dungeon::update_fog(){
  //set visibility to false, set surrounding text to grey
  attron(COLOR_PAIR(GRAY_PAIR));
  for(int i = -3; i <= 3; i++){
    for(int j = -3; j <= 3; j++){   
      if(pc.row + i < 0 || pc.row + i >= HEIGHT || pc.col + j < 0 || pc.col + j >= WIDTH) continue;
      visible[pc.row + i][pc.col + j] = false;
      if(i == 0 && j == 0) continue;
      if(fog) mvaddch(pc.row + i, pc.col + j, memory[pc.row + i][pc.col + j]);
    }
  }
  attroff(COLOR_PAIR(GRAY_PAIR));
  //update in the eight directions, two spaces away (like a queen in chess)
  attron(COLOR_PAIR(WHITE_PAIR));
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
  attroff(COLOR_PAIR(WHITE_PAIR));
  //color items
  for(int i = 0; i < items.size(); i++){
    if(!visible[items[i].row][items[i].col]) continue; //too far
    attron(COLOR_PAIR(items[i].src->color));
    mvaddch(items[i].row, items[i].col, background[items[i].row][items[i].col]);
    attroff(COLOR_PAIR(items[i].src->color));
  }
  //place player character
  attron(COLOR_PAIR(WHITE_PAIR));
  mvaddch(pc.row, pc.col, '@');
  attroff(COLOR_PAIR(WHITE_PAIR));
  //place monsters
  for(int i = 0; i < monsters.size(); i++){
    if(monsters[i].dead()) continue; //dead monster
    if(!visible[monsters[i].row][monsters[i].col]) continue; //too far
    attron(COLOR_PAIR(monsters[i].src->colors[0]));
    mvaddch(monsters[i].row, monsters[i].col, monsters[i].src->symbol);
    attroff(COLOR_PAIR(monsters[i].src->colors[0]));
  }
}

void Dungeon::print_fog(){
  //update remembered terrain
  update_fog();
  //place remembered terrain that isn't visible
  attron(COLOR_PAIR(GRAY_PAIR));
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if(visible[i][j]) continue;
      mvaddch(i, j, memory[i][j]);
    }
  }
  attroff(COLOR_PAIR(GRAY_PAIR));
  //color items
  for(int i = 0; i < items.size(); i++){
    if(!visible[items[i].row][items[i].col]) continue; //too far
    attron(COLOR_PAIR(items[i].src->color));
    mvaddch(items[i].row, items[i].col, background[items[i].row][items[i].col]);
    attroff(COLOR_PAIR(items[i].src->color));
  }
  //place player character
  attron(COLOR_PAIR(WHITE_PAIR));
  mvaddch(pc.row, pc.col, '@');
  attroff(COLOR_PAIR(WHITE_PAIR));
  //place monsters
  for(int i = 0; i < monsters.size(); i++){
    if(monsters[i].dead()) continue; //dead monster
    if(!visible[monsters[i].row][monsters[i].col]) continue; //too far
    attron(COLOR_PAIR(monsters[i].src->colors[0]));
    mvaddch(monsters[i].row, monsters[i].col, monsters[i].src->symbol);
    attroff(COLOR_PAIR(monsters[i].src->colors[0]));
  }
  refresh();
}
      

void Dungeon::print_map(){
  if(fog){
    print_fog();
    return;
  }
  attron(COLOR_PAIR(WHITE_PAIR));
  //place background
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      mvaddch(i, j, background[i][j]);
    }
  }
  attroff(COLOR_PAIR(WHITE_PAIR));
  //color items
  for(int i = 0; i < items.size(); i++){
    attron(COLOR_PAIR(items[i].src->color));
    mvaddch(items[i].row, items[i].col, background[items[i].row][items[i].col]);
    attroff(COLOR_PAIR(items[i].src->color));
  }
  //place player character
  attron(COLOR_PAIR(WHITE_PAIR));
  mvaddch(pc.row, pc.col, '@');
  attroff(COLOR_PAIR(WHITE_PAIR));
  //place monsters
  for(int i = 0; i < monsters.size(); i++){
    if(monsters[i].dead()) continue; //dead monster
    attron(COLOR_PAIR(monsters[i].src->colors[0]));
    mvaddch(monsters[i].row, monsters[i].col, monsters[i].src->symbol);
    attroff(COLOR_PAIR(monsters[i].src->colors[0]));
  }
  refresh();
}

void stard(){ //drats backwards
  //delwin(rlg->world);
  //delwin(rlg->status);
  endwin();
}

void update_status_text(char const * text){
  attron(COLOR_PAIR(WHITE_PAIR));
  for(int i = 0; i < WIDTH; i++) mvaddch(HEIGHT, i, ' '); 
  mvprintw(HEIGHT, 0, text);
  attroff(COLOR_PAIR(WHITE_PAIR));
  refresh();
}
