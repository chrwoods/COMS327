void generate_monster(struct dungeon *rlg, int num){
  (rlg->monsters)[num].type = rand() % 16;
  (rlg->monsters)[num].speed = rand() % 16 + 5; //5-20
  while(1){ //place the monster
    uint8_t row = rand() % HEIGHT;
    uint8_t col = rand() % WIDTH;
    if(rlg->map[row][col] != 0) continue;
    if(rlg->pc.row == row && rlg->pc.col == col) continue;
    (rlg->monsters)[num].row = row;
    (rlg->monsters)[num].col = col;
    break;
  }
  (rlg->monsters)[num].pc_row = 0;
  (rlg->monsters)[num].pc_col = 0;
}

int line_of_sight(struct dungeon *rlg, int num){
  uint8_t pc_row = rlg->pc.row;
  uint8_t pc_col = rlg->pc.col;
  uint8_t row = (rlg->monsters)[num].row;
  uint8_t col = (rlg->monsters)[num].col;
  if(pc_col != col){ //uses Bresenham's line algorithm
    double derr = ((double)(row - pc_row))/((double)(col - pc_col));
    if(derr < 0) derr *= -1;
    double err = 0.0;
    int y = pc_row;
    for(int x = pc_col; abs(col - x) > 0; x += ((col - pc_col) > 0) - ((col - pc_col) < 0)){
      err += derr;
      if(err < .5 && rlg->map[y][x] != 0) return 0;
      while(err >= .5){
	y += ((row - pc_row) > 0) - ((row - pc_row) < 0);
	err -= 1.0;
	if(rlg->map[y][x] != 0) return 0;
      }
    }
  } else {
    int dir = ((row - pc_row) > 0) - ((row - pc_row) < 0);
    while(pc_row != row){
      if(rlg->map[pc_row][pc_col] != 0) return 0;
      pc_row += dir;
    }
  }
  return 1;
}

/*void kill_monster(struct dungeon *rlg, int num){
  if(rlg->num_monsters == 0){
    printf("No monsters left.\n"); 
    return;
  }
  if(num >= rlg->num_monsters){
    printf("Trying to kill a nonexistent monster.\n");
    return;
  }
  if(rlg->num_monsters == 1){
    rlg->num_monsters = 0;
    free(rlg->monsters);
    return;
  }
  if(rlg->num_monsters - 1 == num){
    rlg->num_monsters--;
    rlg->monsters = realloc(rlg->monsters, rlg->num_monsters * sizeof(struct monster));
    return;
  }
  rlg->num_monsters--;
  (rlg->monsters)[num].row = (rlg->monsters)[rlg->num_monsters].row;
  (rlg->monsters)[num].col = (rlg->monsters)[rlg->num_monsters].col;
  (rlg->monsters)[num].type = (rlg->monsters)[rlg->num_monsters].type;
  (rlg->monsters)[num].speed = (rlg->monsters)[rlg->num_monsters].speed;
  (rlg->monsters)[num].pc_row = (rlg->monsters)[rlg->num_monsters].pc_row;
  (rlg->monsters)[num].pc_col = (rlg->monsters)[rlg->num_monsters].pc_col;
  rlg->monsters = realloc(rlg->monsters, rlg->num_monsters * sizeof(struct monster));
}*/

void kill_monster(struct dungeon *rlg, int num){
  (rlg->monsters)[num].speed = 0;
}
  
int move_monster(struct dungeon *rlg, int num){
  if(num < 0){//move player
    //move_pc(rlg);
    return 0;
  }
  //decide where to move
  if(((rlg->monsters)[num].type & MON_TELEPATHIC) == MON_TELEPATHIC){
    (rlg->monsters)[num].pc_row = rlg->pc.row;
    (rlg->monsters)[num].pc_col = rlg->pc.col;
  } else {
    if(line_of_sight(rlg, num)){
      (rlg->monsters)[num].pc_row = rlg->pc.row;
      (rlg->monsters)[num].pc_col = rlg->pc.col;
    } else if(((rlg->monsters)[num].type & MON_SMART) != MON_SMART){
      (rlg->monsters)[num].pc_row = 0;
      (rlg->monsters)[num].pc_col = 0;
    }
  }
  //printf("Monster number %d has stored pc values of %d, %d.\nActual values are %d, %d.\n", num, (rlg->monsters)[num].pc_row, (rlg->monsters)[num].pc_col, rlg->pc.row, rlg->pc.col);
  
  //find the neighbors
  int neighbors[3][3];
  uint8_t row = (rlg->monsters)[num].row;
  uint8_t col = (rlg->monsters)[num].col;
  for(int i = -1; i <= 1; i++){
    for(int j = -1; j <= 1; j++){
      if(((rlg->monsters)[num].type & MON_TUNNEL) == MON_TUNNEL){
	neighbors[i + 1][j + 1] = rlg->t_path[row + i][col + j];
      } else {	
	neighbors[i + 1][j + 1] = rlg->nt_path[row + i][col + j];
      }
    }
  }
  /*printf("Neighbors array of monster number %d is:\n", num);
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      printf("%d ", neighbors[i][j]);
    }
    printf("\n");
  }*/
  
  //decide where to move
  int destination = 4; //center
  if((rlg->monsters)[num].pc_row == rlg->pc.row && (rlg->monsters)[num].pc_col == rlg->pc.col && (((rlg->monsters)[num].type & MON_SMART) == MON_SMART)){
    int shortest_dist = 17469; //arbitrarily large(ish)
    for(int i = 0; i < 9; i++){	
      if(neighbors[i / 3][i % 3] < shortest_dist && neighbors[i / 3][i % 3] >= 0){
	shortest_dist = neighbors[i / 3][i % 3];
	destination = i;
      } else if (neighbors[i / 3][i % 3] == shortest_dist && (i / 3 == 1 || i % 3 == 1)){ //tries to move horizontally instead of diagonally so it doesn't look weird
	destination = i;
      }
    }
  } else if((rlg->monsters)[num].pc_row + (rlg->monsters)[num].pc_col > 0){ //if there is a stored PC location
    int v_dist = (rlg->monsters)[num].pc_row - row;
    int h_dist = (rlg->monsters)[num].pc_col - col;
    int v_dir = 0;
    int h_dir = 0;
    if(v_dist != 0) v_dir = abs(v_dist)/v_dist; //gets sign
    if(h_dist != 0) h_dir = abs(h_dist)/h_dist;
    if(neighbors[v_dir + 1][h_dir + 1] >= 0){
      //if can go diagonal, continue
    } else if(neighbors[1][h_dir + 1] >= 0){
      v_dir = 0; //if can go horizontal, continue
    } else if(neighbors[v_dir + 1][1] >= 0){
      h_dir = 0; //if can go vertical, continue
    } //otherwise it will not move
    destination = v_dir * 3 + h_dir + 4;
  }
  //printf("Calculated destination %d, %d.\n", destination / 3, destination % 3);
  //printf("Note: monster type is %d, and it's erratic value is %d.\n", (rlg->monsters)[num].type, ((rlg->monsters)[num].type & MON_ERRATIC) == MON_ERRATIC);
  
  //erratic behavior
  if(((rlg->monsters)[num].type & MON_ERRATIC) == MON_ERRATIC && rand() % 2 == 1){
    while(1){
      destination = rand() % 9;
      if(neighbors[destination / 3][destination % 3] >= 0) break; //check if moving here is possible
    }
  }
  //printf("Moving to destination %d, %d.\n", destination / 3, destination % 3);
  //printf("Moving from %d, %d, to %d, %d.\n", row, col, row - 1 + destination / 3, col - 1 + destination % 3);
  //printf("Hardness at destination is %d.\n", rlg->map[row - 1 + destination / 3][col - 1 + destination % 3]);

  //move in the given direction
  mvaddch(row, col, rlg->background[row][col]); //reset where monster was
  if(rlg->map[row - 1 + destination / 3][col - 1 + destination % 3] > 0){
    if(rlg->map[row - 1 + destination / 3][col - 1 + destination % 3] < 85)
      rlg->map[row - 1 + destination / 3][col - 1 + destination % 3] = 0;
    else rlg->map[row - 1 + destination / 3][col - 1 + destination % 3] -= 85;
    generate_paths(rlg); //generate new paths because hardness changed
    update_background(rlg); //update background because of new possible corridor
  }
  if(rlg->map[row - 1 + destination / 3][col - 1 + destination % 3] == 0){
    (rlg->monsters)[num].row += (destination / 3) - 1;
    (rlg->monsters)[num].col += (destination % 3) - 1;
    row = (rlg->monsters)[num].row;
    col = (rlg->monsters)[num].col;
    //check combat
    if(row == rlg->pc.row && col == rlg->pc.col) return 1; //the player has been killed
    for(int i = 0; i < rlg->num_monsters; i++){
      if(num == i) continue; //suicide is not the answer, monsters can only commit homicide
      if((rlg->monsters)[i].row == row && (rlg->monsters)[i].col == col){
	kill_monster(rlg, i); //FATALITY
	//i--; //technically we can break here, but this checks for monster overlap just in case
      }
    }
  }
  char type = (rlg->monsters)[num].type < 10 ? (char)(48 + (rlg->monsters)[num].type) : (char)(87 + (rlg->monsters)[num].type);
  mvaddch(row, col, type); //add monster at new spot
  refresh();
  return 0;
}

void print_monster_list(struct dungeon *rlg){
  WINDOW *list = newwin(HEIGHT + 3, WIDTH, 0, 0);
  int col = 17;
  mvwprintw(list, 0, col, "                           _                ");
  mvwprintw(list, 1, col, "                          | |               ");
  mvwprintw(list, 2, col, " _ __ ___   ___  _ __  ___| |_ ___ _ __ ___ ");
  mvwprintw(list, 3, col, "| \'_ ` _ \\ / _ \\| \'_ \\/ __| __/ _ \\ \'__/ __|");
  mvwprintw(list, 4, col, "| | | | | | (_) | | | \\__ \\ ||  __/ |  \\__ \\");
  mvwprintw(list, 5, col, "|_| |_| |_|\\___/|_| |_|___/\\__\\___|_|  |___/");
  col += 6;
  mvwprintw(list, 7, col, "+------------------------------+");
  mvwprintw(list, 8, col, "|                              |");
  mvwprintw(list, 22, col, "+------------------------------+");
  int key;
  int alive_monsters = 0;
  int monster_array_locs[rlg->num_monsters];
  for(int i = 0; i < rlg->num_monsters; i++){
    if((rlg->monsters)[i].speed != 0) monster_array_locs[alive_monsters++] = i;
  }
  int page = 0;
  int num_pages = alive_monsters / 6 + (alive_monsters % 6 > 0);
  do{
    if(page < 0) page = 0;
    for(int i = 0; i < 6; i++){
      if(page * 6 + i >= alive_monsters){
	mvwprintw(list, 9 + i * 2, col, "|                              |");
	mvwprintw(list, 10 + i * 2, col, "|                              |");
	continue;
      }
      int num = monster_array_locs[page * 6 + i];
      char vert[6] = "north";
      char horz[5] = "east";
      int v_dist = rlg->pc.row - (rlg->monsters)[num].row;
      int h_dist = (rlg->monsters)[num].col - rlg->pc.col;
      if(v_dist < 0) {
	v_dist *= -1;
	vert[0] = 's';
	vert[2] = 'u';
      }
      if(h_dist < 0) {
	h_dist *= -1;
	horz[0] = 'w';
	horz[1] = 'e';
      }
      mvwprintw(list, 9 + i * 2, col, "|     %x, %2d %s, %2d %s     |",
	       (rlg->monsters)[num].type, v_dist, vert, h_dist, horz);
      mvwprintw(list, 10 + i * 2, col, "|                              |");
    }
    if(alive_monsters <= 0) page = -1;
    mvwprintw(list, 21, col, "|           %4d/%-4d          |", page + 1, num_pages);
    wrefresh(list);
    key = getch();
    if(key == KEY_DOWN && (page < num_pages - 1)) page++;
    if(key == KEY_UP && (page > 0)) page--;
  }while(key != 27); //27 = escape key (also is alt key, which will also close monster list)
  wclear(list);
  delwin(list);
  clear();
  print_map(rlg);
}
    
    
  

