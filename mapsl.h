void Dungeon::save_map(char* filepath){
  FILE *fp;
  fp = fopen(filepath, "w");
  fwrite("RLG327-S2019", sizeof(char), 12, fp);
  uint32_t num32 = htobe32(0);
  fwrite(&num32, sizeof(num32), 1, fp);
  num32 = htobe32(1708 + num_rooms * 4 + num_up * 2 + num_down * 2);
  fwrite(&num32, sizeof(num32), 1, fp);
  fwrite(&(pc.col), 1, 1, fp);
  fwrite(&(pc.row), 1, 1, fp);
  //write the map
  //fwrite(map8, 1, HEIGHT * WIDTH, fp); //this works too
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      fwrite(&(map[i][j]), 1, 1, fp);
    }
  }
  //write the rooms
  uint16_t num16 = htobe16(num_rooms);
  fwrite(&num16, sizeof(num16), 1, fp);
  for(int i = 0; i < num_rooms; i++){
    fwrite(&(rooms[i].col), 1, 1, fp);
    fwrite(&(rooms[i].row), 1, 1, fp);
    fwrite(&(rooms[i].width), 1, 1, fp);
    fwrite(&(rooms[i].height), 1, 1, fp);
  }
  //write stairs
  num16 = htobe16(num_up);
  fwrite(&num16, sizeof(num16), 1, fp);
  for(int i = 0; i < num_up; i++){
    fwrite(&(up[i].col), 1, 1, fp);
    fwrite(&(up[i].row), 1, 1, fp);
  }
  num16 = htobe16(num_down);
  fwrite(&num16, sizeof(num16), 1, fp);
  for(int i = 0; i < num_down; i++){
    fwrite(&(down[i].col), 1, 1, fp);
    fwrite(&(down[i].row), 1, 1, fp);
  }
  fclose(fp);
}

int Dungeon::load_map(char* filepath){
  FILE *fp;
  fp = fopen(filepath, "r");
  if(fp == 0){
    printf("File to load was not found at filepath: %s\n", filepath);
    return -4;
  }
  char filetype[13];
  fread(filetype, sizeof(char), 12, fp);
  filetype[12] = '\0';
  if(strcmp(filetype, "RLG327-S2019") != 0){
    printf("Filetype marker unrecognized, unable to load map.\n");
    return -1;
  }
  uint32_t num32;
  fread(&num32, sizeof(uint32_t), 1, fp);
  num32 = be32toh(num32);
  if(num32 != 0){
    printf("Found non-zero version marker, unable to load map.\n");
    return -2;
  }
  fread(&num32, sizeof(uint32_t), 1, fp);
  num32 = be32toh(num32); //filesize for later
  //read player character
  fread(&(pc.col), 1, 1, fp);
  fread(&(pc.row), 1, 1, fp);
  //read map grid
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      uint8_t num8;
      fread(&num8, 1, 1, fp);
      map[i][j] = num8;
    }
  }
  //read number of rooms
  uint16_t num16;
  fread(&num16, sizeof(uint16_t), 1, fp);
  num_rooms = be16toh(num16);
  rooms = (Room*)realloc(rooms, num_rooms * sizeof(Room));
  //load rooms
  for(int n = 0; n < num_rooms; n++){
    fread(&(rooms[n].col), 1, 1, fp);
    fread(&(rooms[n].row), 1, 1, fp);
    fread(&(rooms[n].width), 1, 1, fp);
    fread(&(rooms[n].height), 1, 1, fp);
    for(int i = rooms[n].row; i < rooms[n].height + rooms[n].row; i++){
      for(int j = rooms[n].col; j < rooms[n].width + rooms[n].col; j++){
	if(map[i][j] > 0){
	  printf("Room placement data does not match given map, unable to load map.\n");
	  return -3;
	}
      }
    }
    num32 -= 4; //normalizes filesize
  }
  //read staircases
  fread(&num16, sizeof(num16), 1, fp);
  num_up = be16toh(num16);
  up = (Obj*)realloc(up, num_up * sizeof(Obj));
  for(int i = 0; i < num_up; i++){
    uint8_t num8;
    fread(&(up[i].col), 1, 1, fp);
    fread(&(up[i].row), 1, 1, fp);
    num32 -= 2;
  }
  fread(&num16, sizeof(num16), 1, fp);
  num_down = be16toh(num16);
  down = (Obj*)realloc(down, num_down * sizeof(Obj));
  for(int i = 0; i < num_down; i++){
    fread(&(down[i].col), 1, 1, fp);
    fread(&(down[i].row), 1, 1, fp);
    num32 -= 2;
  }
  //check if filesize is correct and close file
  if(num32 != 1708) printf("Warning: given filesize does not match actual filesize.\n");
  fclose(fp);
  return 0;
}
