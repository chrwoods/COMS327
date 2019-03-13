//#include "structs.h"

void generate_map_around_pc(struct dungeon *rlg, int seed);

void blur_map(struct dungeon *rlg);

void generate_first_room(struct dungeon *rlg);

void generate_room(struct dungeon *rlg, int num);

void generate_corridor(struct dungeon *rlg, int start, int end);

void init_map(struct dungeon *rlg){
  rlg->rooms = malloc(1);
  rlg->up = malloc(1);
  rlg->down = malloc(1);
  rlg->num_monsters = 0;
  rlg->monsters = malloc(1);
  rlg->pc.row = 0;
}

void generate_map(struct dungeon *rlg, int seed){
  if(seed < 0) srand(time(NULL)); //if given seed is negative, pick random seed
  else srand(seed);
  rlg->pc.row = 1 + rand() % (HEIGHT - 1);
  rlg->pc.col = 1 + rand() % (WIDTH - 1);
  generate_map_around_pc(rlg, seed);
}

void generate_map_around_pc(struct dungeon *rlg, int seed){
  if(seed < 0) srand(time(NULL)); //if given seed is negative, pick random seed
  else srand(seed);
  //generate rocks
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if(i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) (*rlg).map[i][j] = 255;
      else (*rlg).map[i][j] = 1 + rand() % 254;
    }
  }
  //add rooms
  (*rlg).num_rooms = MIN_ROOMS + rand() % (MIN_ROOMS);
  (*rlg).rooms = realloc(rlg->rooms, (*rlg).num_rooms * sizeof(struct room));
  generate_first_room(rlg); //make first room around the PC
  for(int i = 1; i < (*rlg).num_rooms; i++) generate_room(rlg, i); //generate other rooms
  //blur map to make rocks more uniform
  blur_map(rlg);
  //add up staircases at random positions
  (*rlg).num_up = 1 + rand() % 2;
  (*rlg).up = realloc(rlg->up, (*rlg).num_up * sizeof(struct obj));
  for(int i = 0; i < (*rlg).num_up; i += 0){ //only advances if placed
    int r = rand() % HEIGHT;
    int c = rand() % WIDTH;
    if((*rlg).map[r][c] == 0){
      (*rlg).up[i].row = r;
      (*rlg).up[i].col = c;
      i++;
    }
  }  
  //add down staircases at random positions
  (*rlg).num_down = 1 + rand() % 2;
  (*rlg).down = realloc(rlg->down, (*rlg).num_down * sizeof(struct obj));
  for(int i = 0; i < (*rlg).num_down; i += 0){ //only advances if placed
    int r = rand() % HEIGHT;
    int c = rand() % WIDTH;
    if((*rlg).map[r][c] == 0){
      (*rlg).down[i].row = r;
      (*rlg).down[i].col = c;
      i++;
    }
    for(int j = 0; j < (*rlg).num_up; j++){
      if((*rlg).up[j].row == r && (*rlg).up[j].col == c){
	i--; //if stairs overlap, place stairs again
	break;
      }
    }
  }
  /*while(1) { place PC
    int r = rand() % HEIGHT;
    int c = rand() % WIDTH;
    if((*rlg).map[r][c] == 0){
      (*rlg).pc.row = r;
      (*rlg).pc.col = c;
      break;
    }
  }*/  
  //add corridors
  int8_t connected[(*rlg).num_rooms + 1]; //final space is 0 if graph incomplete, 1 if complete
  for(int i = 1; i <= (*rlg).num_rooms; i++) connected[i] = 0;
  connected[0] = 1;
  int cur = 0;
  while(connected[(*rlg).num_rooms] == 0){
    int closest = 0;
    double dist = 255; //arbitrary large(ish) number
    for(int i = 0; i < (*rlg).num_rooms; i++){
      //calculate the closest unconnected room to the current room, and connect to it 
      //(will not choose shortest distance for last room on purpose to add spontaneity to corridor generation)
      if(i == cur) continue;
      if(connected[i] == 1) continue;
      double tempdist = sqrt(pow((*rlg).rooms[cur].row - (*rlg).rooms[i].row, 2.0) + pow((*rlg).rooms[cur].col - (*rlg).rooms[i].col, 2.0));
      if(tempdist < dist) {
	closest = i;
	dist = tempdist;
      }
    }
    generate_corridor(rlg, cur, closest);
    cur = closest;
    connected[cur] = 1;    
    //check if all are connected
    connected[(*rlg).num_rooms] = 1;
    for(int i = 0; i < (*rlg).num_rooms; i++){
      if(connected[i] == 0){
	connected[(*rlg).num_rooms] = 0;
	break;
      }
    }
  }  
  //for(int i = 0; i < num_rooms - 1; i++) generate_corridor(map, rooms, i, i+1); old way to generate maps
  //return num_rooms;
}

void blur_map(struct dungeon *rlg){
  //apply blur filter
  uint8_t blurred[HEIGHT][WIDTH];
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if((*rlg).map[i][j] == 0 || (*rlg).map[i][j] == 255){
	blurred[i][j] = (*rlg).map[i][j];
	continue;
      }
      int gaussian = 0;
      int filter[3][3] = {{1, 4, 1},
			  {4, 10, 4},
			  {1, 4, 1}};
      for(int x = -1; x <= 1; x++){
	for(int y = -1; y <= 1; y++){
	  gaussian += (*rlg).map[i + y][j + x] * filter[y + 1][x + 1]; //won't go OOB because edge is detected above
	}
      }
      blurred[i][j] = gaussian / 30;
    }
  }
  //set main map to blurred
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      (*rlg).map[i][j] = blurred[i][j];
    }
  }
}

void generate_first_room(struct dungeon *rlg){
  //note: since it's the first room, no checking needs to be done
  //this method makes the first room contain the PC
  //set size of room
  rlg->rooms[0].width = MIN_ROOM_WIDTH + rand() % (MIN_ROOM_WIDTH * 2);
  rlg->rooms[0].height = MIN_ROOM_HEIGHT + rand() % (MIN_ROOM_HEIGHT * 2);
  do{
    rlg->rooms[0].row = rlg->pc.row - rand() % rlg->rooms[0].height;
  }while(rlg->rooms[0].row > (HEIGHT - rlg->rooms[0].height - 1) || rlg->rooms[0].row <= 0);
  do{
    rlg->rooms[0].col = rlg->pc.col - rand() % rlg->rooms[0].width;
  }while(rlg->rooms[0].col > (WIDTH - rlg->rooms[0].width - 1) || rlg->rooms[0].col <= 0);
  //place the room
  for(int i = 0; i < rlg->rooms[0].height; i++){
    for(int j = 0; j < rlg->rooms[0].width; j++){
      rlg->map[rlg->rooms[0].row + i][rlg->rooms[0].col + j] = 0;	
    }
  }
}

void generate_room(struct dungeon *rlg, int num){
  //set size of room
  (*rlg).rooms[num].width = MIN_ROOM_WIDTH + rand() % (MIN_ROOM_WIDTH * 2);
  (*rlg).rooms[num].height = MIN_ROOM_HEIGHT + rand() % (MIN_ROOM_HEIGHT * 2);
  int placed = 0;
  while(placed == 0){
    //set potential top left corner of room
    (*rlg).rooms[num].row = rand() % (HEIGHT - (*rlg).rooms[num].height - 2) + 1;
    (*rlg).rooms[num].col = rand() % (WIDTH - (*rlg).rooms[num].width - 2) + 1;
    //check to see if room space is free
    for(int i = -1; i <= (*rlg).rooms[num].width && placed >= 0; i++){
      for(int j = -1; j <= (*rlg).rooms[num].height; j++){
	if((*rlg).map[(*rlg).rooms[num].row + j][(*rlg).rooms[num].col + i] <= 0){
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
    for(int i = 0; i < (*rlg).rooms[num].height; i++){
      for(int j = 0; j < (*rlg).rooms[num].width; j++){
	(*rlg).map[(*rlg).rooms[num].row + i][(*rlg).rooms[num].col + j] = 0;	
      }
    }
    placed = 1; //same as a break statement
  }
}

void generate_corridor(struct dungeon *rlg, int start, int end){
  int cur_r = (*rlg).rooms[start].row + (*rlg).rooms[start].height/2;
  int cur_c = (*rlg).rooms[start].col + (*rlg).rooms[start].width/2;
  int dest_r = (*rlg).rooms[end].row + (*rlg).rooms[end].height/2;
  int dest_c = (*rlg).rooms[end].col + (*rlg).rooms[end].width/2;
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
    (*rlg).map[cur_r][cur_c] = 0;
  }
}

void old_print_map(struct dungeon *rlg){ //deprecated
  //build the 2D character array
  char chargrid[HEIGHT][WIDTH];
  //place rocks and empty spaces
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if((*rlg).map[i][j] == 0) chargrid[i][j] = '#';
      else chargrid[i][j] = ' ';
    }
  }
  //place rooms
  for(int n = 0; n < (*rlg).num_rooms; n++){
    for(int i = 0; i < (*rlg).rooms[n].height; i++){
      for(int j = 0; j < (*rlg).rooms[n].width; j++){
	chargrid[(*rlg).rooms[n].row + i][(*rlg).rooms[n].col + j] = '.';
      }
    }
  }
  //place up stairs
  for(int i = 0; i < (*rlg).num_up; i++){
    chargrid[(*rlg).up[i].row][(*rlg).up[i].col] = '<';
  }
  //place down stairs
  for(int i = 0; i < (*rlg).num_down; i++){
    chargrid[(*rlg).down[i].row][(*rlg).down[i].col] = '>';
  }
  //place player character
  chargrid[(*rlg).pc.row][(*rlg).pc.col] = '@';
  //place monsters
  char monstersyms[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  for(int i = 0; i < rlg->num_monsters; i++){
    if((rlg->monsters)[i].speed == 0) continue; //dead monster
    chargrid[(rlg->monsters)[i].row][(rlg->monsters)[i].col] = monstersyms[(rlg->monsters)[i].type];
  }
  //print the character grid
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      printf("%c", chargrid[i][j]);
    }
    printf("\n");
  }
}

void empty_map(struct dungeon *rlg){
  free(rlg->rooms);
  free(rlg->up);
  free(rlg->down);
  free(rlg->monsters);
  free(rlg);
}
