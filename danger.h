void generate_monster(struct dungeon *rlg, int num){
  (rlg->monsters)[num].type = rand() % 16;
  (rlg->monsters)[num].speed = rand() % 16 + 5; //5-20
  while(1){ //place the monster
    uint8_t row = rand() % HEIGHT;
    uint8_t col = rand() % WIDTH;
    if(rlg->map[row][col] != 0) continue;
    if(rlg->pc.row == row && rlg->pc.col) continue;
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
  uint8_t dist = abs(pc_row - row) + abs(pc_col - col);
  while(dist > 0){
    //todo
    break;
  }
  return 1;
}
  
void move(struct dungeon *rlg, int num){
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
      }
    }
  } else if((rlg->monsters)[num].pc_row + (rlg->monsters)[num].pc_col > 0){ //if there is a stored PC location
    int v_dist = row - (rlg->monsters)[num].pc_row;
    int h_dist = col - (rlg->monsters)[num].pc_col;
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
  if(rlg->map[row - 1 + destination / 3][col - 1 + destination % 3] > 0){
    if(rlg->map[row - 1 + destination / 3][col - 1 + destination % 3] < 85)
      rlg->map[row - 1 + destination / 3][col - 1 + destination % 3] = 0;
    else rlg->map[row - 1 + destination / 3][col - 1 + destination % 3] -= 85;
    generate_paths(rlg); //generate new paths because hardness changed
  }
  if(rlg->map[row - 1 + destination / 3][col - 1 + destination % 3] == 0){
    (rlg->monsters)[num].row += (destination / 3) - 1;
    (rlg->monsters)[num].col += (destination % 3) - 1;
  }
}
