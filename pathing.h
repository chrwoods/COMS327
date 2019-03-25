void print_hex_map(struct dungeon *rlg){
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      printf("%02x", (*rlg).map[i][j]);
    }
    printf("\n");
  }
}

void weight_nontunnel(struct dungeon *rlg, int weight[HEIGHT][WIDTH]){
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if(rlg->map[i][j] == 0) weight[i][j] = 1;
      else weight[i][j] = -1;
    }
  }
}

void weight_tunnel(struct dungeon *rlg, int weight[HEIGHT][WIDTH]){
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if(rlg->map[i][j] == 0) weight[i][j] = 1;
      else if(rlg->map[i][j] == 255) weight[i][j] = -2;
      else weight[i][j] = 1 + (rlg->map[i][j])/85;
    }
  }
}

void dijkstra(struct dungeon *rlg, int dist[HEIGHT][WIDTH], int weight[HEIGHT][WIDTH]){
  struct pq_queue q;
  pq_init(&q, sizeof(uint16_t));  
  dist[(*rlg).pc.row][(*rlg).pc.col] = 0;
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if(!((*rlg).pc.row == i && (*rlg).pc.col == j)){
	dist[i][j] = 17469 * weight[i][j]; //arbitrary large(ish) number
      }
      if(weight[i][j] >= 0/* && !((*rlg).pc.row == i && (*rlg).pc.col == j)*/){
	uint16_t coords = i * 256 + j; //store both dimensions in one number
	pq_add(&q, &coords, dist[i][j]);
      }
    }
  }
  while(q.num_nodes > 0){
    uint8_t row;
    uint8_t col;
    uint16_t coords;
    int priority; //unused
    pq_pop(&q, &coords, &priority);
    row = coords / 256;
    col = coords % 256;
    for(int i = -1; i <= 1; i++){
      for(int j = -1; j <= 1; j++){
	if((i == 0 && j == 0) || weight[row+i][col+j] < 0) continue;
	int alt = dist[row][col] + weight[row][col];
	if(alt < dist[row + i][col + j]){
	  dist[row + i][col + j] = alt;
	  coords = (row + i) * 256 + col + j;
	  pq_decrease(&q, &coords, alt);
	}
      }
    }
  }
  //normalize map, set unreachables to negative numbers
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if(dist[i][j] == 17469 * -2 || dist[i][j] == 17469) dist[i][j] = -2;
      if(dist[i][j] == -17469) dist[i][j] = -1;
    }
  }
}

void generate_paths(struct dungeon *rlg){
  int weight[HEIGHT][WIDTH];
  //make nontunneling path
  weight_nontunnel(rlg, weight);
  dijkstra(rlg, rlg->nt_path, weight);
  //make tunneling path
  weight_tunnel(rlg, weight);
  dijkstra(rlg, rlg->t_path, weight);
}

void print_path(int path[HEIGHT][WIDTH]){
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if(path[i][j] == -2) printf("X"); //unreachable
      else if(path[i][j] < 0) printf(" "); //unmineable (non-tunneling only)
      else if(path[i][j] == 0) printf("@");
      else printf("%d", path[i][j] % 10);
    }
    printf("\n");
  }
}
