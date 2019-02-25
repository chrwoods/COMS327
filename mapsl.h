//#include "structs.h"

void save_map(char* filepath, struct dungeon *rlg){
  FILE *fp;
  fp = fopen(filepath, "w");
  fwrite("RLG327-S2019", sizeof(char), 12, fp);
  uint32_t num32 = htobe32(0);
  fwrite(&num32, sizeof(num32), 1, fp);
  num32 = htobe32(1708 + (*rlg).num_rooms * 4 + (*rlg).num_up * 2 + (*rlg).num_down * 2);
  fwrite(&num32, sizeof(num32), 1, fp);
  fwrite(&((*rlg).pc.col), 1, 1, fp);
  fwrite(&((*rlg).pc.row), 1, 1, fp);
  //write the map
  //fwrite(map8, 1, HEIGHT * WIDTH, fp); //this works too
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      fwrite(&((*rlg).map[i][j]), 1, 1, fp);
    }
  }
  //write the rooms
  uint16_t num16 = htobe16((*rlg).num_rooms);
  fwrite(&num16, sizeof(num16), 1, fp);
  for(int i = 0; i < (*rlg).num_rooms; i++){
    fwrite(&((*rlg).rooms[i].col), 1, 1, fp);
    fwrite(&((*rlg).rooms[i].row), 1, 1, fp);
    fwrite(&((*rlg).rooms[i].width), 1, 1, fp);
    fwrite(&((*rlg).rooms[i].height), 1, 1, fp);
  }
  //write stairs
  num16 = htobe16((*rlg).num_up);
  fwrite(&num16, sizeof(num16), 1, fp);
  for(int i = 0; i < (*rlg).num_up; i++){
    fwrite(&((*rlg).up[i].col), 1, 1, fp);
    fwrite(&((*rlg).up[i].row), 1, 1, fp);
  }
  num16 = htobe16((*rlg).num_down);
  fwrite(&num16, sizeof(num16), 1, fp);
  for(int i = 0; i < (*rlg).num_down; i++){
    fwrite(&((*rlg).down[i].col), 1, 1, fp);
    fwrite(&((*rlg).down[i].row), 1, 1, fp);
  }
  fclose(fp);
}

int load_map(char* filepath, struct dungeon *rlg){
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
  fread(&((*rlg).pc.col), 1, 1, fp);
  fread(&((*rlg).pc.row), 1, 1, fp);
  //read map grid
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      uint8_t num8;
      fread(&num8, 1, 1, fp);
      (*rlg).map[i][j] = num8;
    }
  }
  //read number of rooms
  uint16_t num16;
  fread(&num16, sizeof(uint16_t), 1, fp);
  (*rlg).num_rooms = be16toh(num16);
  (*rlg).rooms = malloc((*rlg).num_rooms * sizeof(struct room));
  //load rooms
  for(int n = 0; n < (*rlg).num_rooms; n++){
    fread(&((*rlg).rooms[n].col), 1, 1, fp);
    fread(&((*rlg).rooms[n].row), 1, 1, fp);
    fread(&((*rlg).rooms[n].width), 1, 1, fp);
    fread(&((*rlg).rooms[n].height), 1, 1, fp);
    for(int i = (*rlg).rooms[n].row; i < (*rlg).rooms[n].height + (*rlg).rooms[n].row; i++){
      for(int j = (*rlg).rooms[n].col; j < (*rlg).rooms[n].width + (*rlg).rooms[n].col; j++){
	if((*rlg).map[i][j] > 0){
	  printf("Room placement data does not match given map, unable to load map.\n");
	  return -3;
	}
      }
    }
    num32 -= 4; //normalizes filesize
  }
  //read staircases
  fread(&num16, sizeof(num16), 1, fp);
  (*rlg).num_up = be16toh(num16);
  (*rlg).up = malloc((*rlg).num_up * sizeof(struct obj));
  for(int i = 0; i < (*rlg).num_up; i++){
    uint8_t num8;
    fread(&((*rlg).up[i].col), 1, 1, fp);
    fread(&((*rlg).up[i].row), 1, 1, fp);
    num32 -= 2;
  }
  fread(&num16, sizeof(num16), 1, fp);
  (*rlg).num_down = be16toh(num16);
  (*rlg).down = malloc((*rlg).num_down * sizeof(struct obj));
  for(int i = 0; i < (*rlg).num_down; i++){
    fread(&((*rlg).down[i].col), 1, 1, fp);
    fread(&((*rlg).down[i].row), 1, 1, fp);
    num32 -= 2;
  }
  //check if filesize is correct and close file
  if(num32 != 1708) printf("Warning: given filesize does not match actual filesize.\n");
  fclose(fp);
  return 0;
}
