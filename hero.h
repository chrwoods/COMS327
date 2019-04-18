int Dungeon::move_pc(int direction){
  if(direction == 5) return 0;
  uint8_t row = pc.row;
  uint8_t col = pc.col;
  int v_dir = 1 - ((direction - 1) / 3);
  int h_dir = ((direction - 1) % 3) - 1;
  row += v_dir;
  col += h_dir;
  if(map[row][col] != 0) return -1;
  attron(COLOR_PAIR(WHITE_PAIR));
  mvaddch(pc.row, pc.col, background[pc.row][pc.col]); //remove player from the map
  pc.row = row;
  pc.col = col;
  //unalive the monsters
  for(int i = 0; i < monsters.size(); i++){
    if(monsters[i].row == row && monsters[i].col == col){
      kill_monster(i); //FATALITY
      //soon enough this comment will turn into a combat command
    }
  }
  generate_paths(); //generate paths for new PC location
  mvaddch(row, col, '@'); //add pc on map again
  attroff(COLOR_PAIR(WHITE_PAIR));
  update_fog(); //update fog around player
  refresh();
  return 0;
}

int Dungeon::use_staircase(char stair){
  if(background[pc.row][pc.col] != stair) return -1;
  uint8_t row = pc.row;
  uint8_t col = pc.col;
  generate_map_around_pc(-1); //generate new map randomly
  update_background();
  int num_mons = monsters.size();
  monsters.clear();
  for(int i = 0; i < num_mons; i++) generate_monster();
  items.clear();
  for(int i = 0; i < num_items; i++) generate_item();
  print_map();
  return 0;
}

void Dungeon::teleport(){
  bool stored_fog = fog;
  fog = false;
  print_map();
  int screen[HEIGHT][WIDTH];
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      screen[i][j] = mvinch(i, j); //save the screen
    }
  }
  Obj dest(pc.row, pc.col);
  mvaddch(dest.row, dest.col, '*');
  refresh();
  while(1){
    int key = getch();
    int direction = 0;
    if(key == '1' || key == 'b') direction = 1;
    else if(key == '2' || key == 'j') direction = 2;
    else if(key == '3' || key == 'n') direction = 3;
    else if(key == '4' || key == 'h') direction = 4;
    else if(key == '6' || key == 'l') direction = 6;
    else if(key == '7' || key == 'y') direction = 7;
    else if(key == '8' || key == 'k') direction = 8;
    else if(key == '9' || key == 'u') direction = 9;
    else if(key == 'r'){
      dest.row = 1 + rand() % (HEIGHT - 2);
      dest.col = 1 + rand() % (WIDTH - 2);
      key = 't';
    }
    if(key == 't'){
      //teleport
      pc.row = dest.row;
      pc.col = dest.col;
      break;
    } else if(direction != 0){
      int v_dir = 1 - ((direction - 1) / 3);
      int h_dir = ((direction - 1) % 3) - 1;
      if(dest.row + v_dir <= 0 || dest.row + v_dir >= HEIGHT - 1 || dest.col + h_dir <= 0 || dest.col + h_dir >= WIDTH - 1){
	update_status_text("   You cannot teleport out of the map!");
	continue;
      }
      update_status_text("");
      attron(screen[dest.row][dest.col] & A_COLOR);
      mvaddch(dest.row, dest.col, screen[dest.row][dest.col] & A_CHARTEXT);
      attroff(screen[dest.row][dest.col] & A_COLOR);
      dest.row += v_dir;
      dest.col += h_dir;
      mvaddch(dest.row, dest.col, '*');
    }
  }
  //reset visibility
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      visible[i][j] = false;
    }
  }
  fog = stored_fog;
  generate_paths(); //generate paths for new PC location
  if(!fog) update_fog();
  print_map();
}

void Dungeon::look_at_something(bool target_is_item){
  print_map();
  int screen[HEIGHT][WIDTH];
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      screen[i][j] = mvinch(i, j); //save the screen
    }
  }
  Obj dest(pc.row, pc.col);
  mvaddch(dest.row, dest.col, '*');
  refresh();
  while(1){
    int key = getch();
    int direction = 0;
    if(key == 27) break;
    else if(key == '1' || key == 'b') direction = 1;
    else if(key == '2' || key == 'j') direction = 2;
    else if(key == '3' || key == 'n') direction = 3;
    else if(key == '4' || key == 'h') direction = 4;
    else if(key == '6' || key == 'l') direction = 6;
    else if(key == '7' || key == 'y') direction = 7;
    else if(key == '8' || key == 'k') direction = 8;
    else if(key == '9' || key == 'u') direction = 9;
    if(key == 't'){
      //look
      if(fog && !visible[dest.row][dest.col]){
	update_status_text("   You cannot look where you cannot see!");
	continue;
      }
      bool looked = false;
      if(!target_is_item){
	for(int i = 0; i < monsters.size(); i++){
	  if(dest.row != monsters[i].row || dest.col != monsters[i].col) continue;
	  if(monsters[i].dead()) continue;
	  display_monster_info(i);
	  looked = true;
	}
	mvaddch(dest.row, dest.col, '*');
	if(!looked) update_status_text("   No monster found to look at.");      
	//else break;
      } else {
	for(int i = 0; i < items.size(); i++){
	  if(dest.row != items[i].row || dest.col != items[i].col) continue;
	  display_item_info(i);
	  looked = true;
	}
	mvaddch(dest.row, dest.col, '*');
	if(!looked) update_status_text("   No item found to look at.");
	//else break;
      }
    } else if(direction != 0){
      update_status_text("");
      int v_dir = 1 - ((direction - 1) / 3);
      int h_dir = ((direction - 1) % 3) - 1;
      if(dest.row + v_dir <= 0 || dest.row + v_dir >= HEIGHT - 1 || dest.col + h_dir <= 0 || dest.col + h_dir >= WIDTH - 1){
	update_status_text("   You cannot select outside of the map!");
	continue;
      }
      attron(screen[dest.row][dest.col] & A_COLOR);
      mvaddch(dest.row, dest.col, screen[dest.row][dest.col] & A_CHARTEXT);
      attroff(screen[dest.row][dest.col] & A_COLOR);
      dest.row += v_dir;
      dest.col += h_dir;
      mvaddch(dest.row, dest.col, '*');
    }
  }
  print_map();
}

int Dungeon::pc_turn(){
  while(1){
    int key = getch();
    int direction = 0;
    if(key == 'm') print_monster_list();
    else if(key == '1' || key == 'b') direction = 1;
    else if(key == '2' || key == 'j') direction = 2;
    else if(key == '3' || key == 'n') direction = 3;
    else if(key == '4' || key == 'h') direction = 4;
    else if(key == '5' || key == '.' || key == ' ') direction = 5;
    else if(key == '6' || key == 'l') direction = 6;
    else if(key == '7' || key == 'y') direction = 7;
    else if(key == '8' || key == 'k') direction = 8;
    else if(key == '9' || key == 'u') direction = 9;
    else if(key == '<' || key == '>') {
      if(use_staircase((char)key) == 0){ 
        update_status_text("");
	return 0;
      } else {
        update_status_text("   Staircase not found.");
	refresh();
      }
    }
    else if(key == 'f') {
      if(fog){
	fog = false;
	print_map();
      } else {
	fog = true;
	print_fog();
      }
    }
    else if(key == 'g'){
      teleport();
      return 0;
    }
    else if(key == 'i') print_inventory();
    else if(key == 'e') print_equipment();
    else if(key == 'L') look_at_something(false);
    else if(key == ',') pickup_items();
    else if(key == 'w') wear_item();
    else if(key == 't') takeoff_item();
    else if(key == 'd') drop_item();
    else if(key == 'x') destroy_item();
    else if(key == 'O') look_at_something(true); //look at an item similarly to look at a monster
    else if(key == 'I') inspect_item();
    else if(key == 'q' || key == 'Q') return -1;
    if(direction > 0){
      if(move_pc(direction) == 0){
	update_status_text("");
	return 0;
      } else update_status_text("   You cannot move inside a wall.");
    }
  }
}
