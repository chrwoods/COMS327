int move_pc(struct dungeon *rlg, int direction){
  if(direction == 5) return 0;
  uint8_t row = rlg->pc.row;
  uint8_t col = rlg->pc.col;
  int v_dir = 1 - ((direction - 1) / 3);
  int h_dir = ((direction - 1) % 3) - 1;
  row += v_dir;
  col += h_dir;
  if(rlg->map[row][col] != 0) return -1;
  mvaddch(rlg->pc.row, rlg->pc.col, rlg->background[row][col]); //remove player from the map
  rlg->pc.row = row;
  rlg->pc.col = col;
  //unalive the monsters
  for(int i = 0; i < rlg->num_monsters; i++){
    if((rlg->monsters)[i].row == row && (rlg->monsters)[i].col == col){
      kill_monster(rlg, i); //FATALITY
      //i--; //technically we can break here, but this checks for monster overlap just in case
    }
  }
  generate_paths(rlg); //generate paths for new PC location
  mvaddch(row, col, '@'); //add pc on map again
  refresh();
  return 0;
}

int use_staircase(struct dungeon *rlg, char stair){
  if(rlg->background[rlg->pc.row][rlg->pc.col] != stair) return -1;
  uint8_t row = rlg->pc.row;
  uint8_t col = rlg->pc.col;
  generate_map_around_pc(rlg, -1); //generate new map randomly
  update_background(rlg);
  for(int i = 0; i < rlg->num_monsters; i++) generate_monster(rlg, i);
  print_map(rlg);
  return 0;
}

int pc_turn(struct dungeon *rlg){
  while(1){
    int key = getch();
    int direction = 0;
    if(key == 'm') print_monster_list(rlg);
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
      if(use_staircase(rlg, (char)key) == 0) return 0;
    }
    else if(key == 'q' || key == 'Q') return -1;
    if(direction > 0){
      if(move_pc(rlg, direction) == 0) return 0;
    }
  }
}
