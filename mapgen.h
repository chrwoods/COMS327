void Dungeon::generate_map(int seed){
  if(seed < 0) seed = time(NULL); //if given seed is negative, pick random seed
  srand(seed);
  pc.row = 1 + rand() % (HEIGHT - 1);
  pc.col = 1 + rand() % (WIDTH - 1);
  pc.seed = seed;
  generate_map_around_pc(seed);
}

void Dungeon::generate_map(){
  generate_map(-1);
}

void Dungeon::generate_map_around_pc(int seed){
  if(seed < 0) seed = time(NULL); //if given seed is negative, pick random seed
  srand(seed);
  pc.seed = seed;
  //generate rocks
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if(i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) map[i][j] = 255;
      else map[i][j] = 1 + rand() % 254;
    }
  }
  //add rooms
  num_rooms = MIN_ROOMS + rand() % (MIN_ROOMS);
  rooms = (Room*)realloc(rooms, num_rooms * sizeof(Room));
  generate_first_room(); //make first room around the PC
  for(int i = 1; i < num_rooms; i++) generate_room(i); //generate other rooms
  //blur map to make rocks more uniform
  blur_map();
  //add up staircases at random positions
  num_up = 1 + rand() % 2;
  up = (Obj*)realloc(up, num_up * sizeof(Obj));
  for(int i = 0; i < num_up; i += 0){ //only advances if placed
    int r = rand() % HEIGHT;
    int c = rand() % WIDTH;
    if(map[r][c] == 0){
      up[i].row = r;
      up[i].col = c;
      //up[i](r, c); //oops
      i++;
    }
  }  
  //add down staircases at random positions
  num_down = 1 + rand() % 2;
  down = (Obj*)realloc(down, num_down * sizeof(Obj));
  for(int i = 0; i < num_down; i += 0){ //only advances if placed
    int r = rand() % HEIGHT;
    int c = rand() % WIDTH;
    if(map[r][c] == 0){
      down[i].row = r;
      down[i].col = c;
      i++;
    }
    for(int j = 0; j < num_up; j++){
      if(up[j].row == r && up[j].col == c){
	i--; //if stairs overlap, place stairs again
	break;
      }
    }
  }  
  //add corridors
  int8_t connected[num_rooms + 1]; //final space is 0 if graph incomplete, 1 if complete
  for(int i = 1; i <= num_rooms; i++) connected[i] = 0;
  connected[0] = 1;
  int cur = 0;
  while(connected[num_rooms] == 0){
    int closest = 0;
    double dist = 255; //arbitrary large(ish) number
    for(int i = 0; i < num_rooms; i++){
      //calculate the closest unconnected room to the current room, and connect to it 
      //(will not choose shortest distance for last room on purpose to add spontaneity to corridor generation)
      if(i == cur) continue;
      if(connected[i] == 1) continue;
      double tempdist = sqrt(pow(rooms[cur].row - rooms[i].row, 2.0) + pow(rooms[cur].col - rooms[i].col, 2.0));
      if(tempdist < dist) {
	closest = i;
	dist = tempdist;
      }
    }
    generate_corridor(cur, closest);
    cur = closest;
    connected[cur] = 1;    
    //check if all are connected
    connected[num_rooms] = 1;
    for(int i = 0; i < num_rooms; i++){
      if(connected[i] == 0){
	connected[num_rooms] = 0;
	break;
      }
    }
  }  
  //for(int i = 0; i < num_rooms - 1; i++) generate_corridor(map, rooms, i, i+1); old way to generate maps
  //return num_rooms;
}

void Dungeon::blur_map(){
  //apply blur filter
  uint8_t blurred[HEIGHT][WIDTH];
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if(map[i][j] == 0 || map[i][j] == 255){
	blurred[i][j] = map[i][j];
	continue;
      }
      int gaussian = 0;
      int filter[3][3] = {{1, 4, 1},
			  {4, 10, 4},
			  {1, 4, 1}};
      for(int x = -1; x <= 1; x++){
	for(int y = -1; y <= 1; y++){
	  gaussian += map[i + y][j + x] * filter[y + 1][x + 1]; //won't go OOB because edge is detected above
	}
      }
      blurred[i][j] = gaussian / 30;
    }
  }
  //set main map to blurred
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      map[i][j] = blurred[i][j];
    }
  }
}

void Dungeon::generate_first_room(){
  //note: since it's the first room, no checking needs to be done
  //this method makes the first room contain the PC
  //set size of room
  rooms[0].width = MIN_ROOM_WIDTH + rand() % (MIN_ROOM_WIDTH * 2);
  rooms[0].height = MIN_ROOM_HEIGHT + rand() % (MIN_ROOM_HEIGHT * 2);
  do{
    rooms[0].row = pc.row - rand() % rooms[0].height;
  }while(rooms[0].row > (HEIGHT - rooms[0].height - 1) || rooms[0].row <= 0);
  do{
    rooms[0].col = pc.col - rand() % rooms[0].width;
  }while(rooms[0].col > (WIDTH - rooms[0].width - 1) || rooms[0].col <= 0);
  //place the room
  for(int i = 0; i < rooms[0].height; i++){
    for(int j = 0; j < rooms[0].width; j++){
      map[rooms[0].row + i][rooms[0].col + j] = 0;	
    }
  }
}

void Dungeon::generate_room(int num){
  //set size of room
  rooms[num].width = MIN_ROOM_WIDTH + rand() % (MIN_ROOM_WIDTH * 2);
  rooms[num].height = MIN_ROOM_HEIGHT + rand() % (MIN_ROOM_HEIGHT * 2);
  int placed = 0;
  while(placed == 0){
    //set potential top left corner of room
    rooms[num].row = rand() % (HEIGHT - rooms[num].height - 2) + 1;
    rooms[num].col = rand() % (WIDTH - rooms[num].width - 2) + 1;
    //check to see if room space is free
    for(int i = -1; i <= rooms[num].width && placed >= 0; i++){
      for(int j = -1; j <= rooms[num].height; j++){
	if(map[rooms[num].row + j][rooms[num].col + i] <= 0){
	  placed--;
	  break;
	}
      }
    }
    if(placed < 0){ //wasn't able to place, try again
      placed = 0;
      continue;
    }
    //place the room
    for(int i = 0; i < rooms[num].height; i++){
      for(int j = 0; j < rooms[num].width; j++){
	map[rooms[num].row + i][rooms[num].col + j] = 0;	
      }
    }
    placed = 1; //same as a break statement
  }
}

void Dungeon::generate_corridor(int start, int end){
  int cur_r = rooms[start].row + rooms[start].height/2;
  int cur_c = rooms[start].col + rooms[start].width/2;
  int dest_r = rooms[end].row + rooms[end].height/2;
  int dest_c = rooms[end].col + rooms[end].width/2;
  while(cur_r != dest_r || cur_c != dest_c){
    if(rand() % 2 == 0){ //choose randomly to go vertically or horizontally
      if(cur_r == dest_r) continue;
      else if(cur_r < dest_r) cur_r++;
      else cur_r--;
    } else {
      if(cur_c == dest_c) continue;
      else if(cur_c < dest_c) cur_c++;
      else cur_c--;
    }
    map[cur_r][cur_c] = 0;
  }
}

void Dungeon::old_print_map(){ //deprecated
  //build the 2D character array
  char chargrid[HEIGHT][WIDTH];
  //place rocks and empty spaces
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if(map[i][j] == 0) chargrid[i][j] = '#';
      else chargrid[i][j] = ' ';
    }
  }
  //place rooms
  for(int n = 0; n < num_rooms; n++){
    for(int i = 0; i < rooms[n].height; i++){
      for(int j = 0; j < rooms[n].width; j++){
	chargrid[rooms[n].row + i][rooms[n].col + j] = '.';
      }
    }
  }
  //place up stairs
  for(int i = 0; i < num_up; i++){
    chargrid[up[i].row][up[i].col] = '<';
  }
  //place down stairs
  for(int i = 0; i < num_down; i++){
    chargrid[down[i].row][down[i].col] = '>';
  }
  //place player character
  chargrid[pc.row][pc.col] = '@';
  //place monsters
  char monstersyms[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  for(int i = 0; i < num_monsters; i++){
    if((monsters)[i].speed == 0) continue; //dead monster
    chargrid[(monsters)[i].row][(monsters)[i].col] = monstersyms[(monsters)[i].type];
  }
  //print the character grid
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      printf("%c", chargrid[i][j]);
    }
    printf("\n");
  }
}
