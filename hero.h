int Dungeon::move_pc(int direction){
  if(direction == 5) return 0;
  uint8_t row = pc.row;
  uint8_t col = pc.col;
  int v_dir = 1 - ((direction - 1) / 3);
  int h_dir = ((direction - 1) % 3) - 1;
  row += v_dir;
  col += h_dir;
  if(map[row][col] != 0) return -1;
  mvaddch(pc.row, pc.col, background[pc.row][pc.col]); //remove player from the map
  pc.row = row;
  pc.col = col;
  //unalive the monsters
  for(int i = 0; i < num_monsters; i++){
    if(monsters[i].row == row && monsters[i].col == col){
      kill_monster(i); //FATALITY
      //i--; //technically we can break here, but this checks for monster overlap just in case
    }
  }
  generate_paths(); //generate paths for new PC location
  mvaddch(row, col, '@'); //add pc on map again
  refresh();
  return 0;
}

int Dungeon::use_staircase(char stair){
  if(background[pc.row][pc.col] != stair) return -1;
  uint8_t row = pc.row;
  uint8_t col = pc.col;
  generate_map_around_pc(-1); //generate new map randomly
  update_background();
  for(int i = 0; i < num_monsters; i++) generate_monster(i);
  print_map();
  return 0;
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
    else if(key == 'q' || key == 'Q') return -1;
    if(direction > 0){
      if(move_pc(direction) == 0){
	update_status_text("");
	return 0;
      } else update_status_text("   You cannot move inside a wall.");
    }
  }
}
