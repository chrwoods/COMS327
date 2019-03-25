void Dungeon::weight_nontunnel(int weight[HEIGHT][WIDTH]){
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if(map[i][j] == 0) weight[i][j] = 1;
      else weight[i][j] = -1;
    }
  }
}

void Dungeon::weight_tunnel(int weight[HEIGHT][WIDTH]){
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if(map[i][j] == 0) weight[i][j] = 1;
      else if(map[i][j] == 255) weight[i][j] = -2;
      else weight[i][j] = 1 + map[i][j]/85;
    }
  }
}

void Dungeon::dijkstra(int dist[HEIGHT][WIDTH], int weight[HEIGHT][WIDTH]){
  PQueue q(sizeof(uint16_t));  
  dist[pc.row][pc.col] = 0;
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if(!(pc.row == i && pc.col == j)){
	dist[i][j] = 17469 * weight[i][j]; //arbitrary large(ish) number
      }
      if(weight[i][j] >= 0){
	uint16_t coords = i * 256 + j; //store both dimensions in one number
	q.add(&coords, dist[i][j]);
      }
    }
  }
  while(q.num_nodes > 0){
    uint8_t row;
    uint8_t col;
    uint16_t coords;
    int priority; //unused currently
    q.pop(&coords, &priority);
    row = coords / 256;
    col = coords % 256;
    for(int i = -1; i <= 1; i++){
      for(int j = -1; j <= 1; j++){
	if((i == 0 && j == 0) || weight[row+i][col+j] < 0) continue;
	int alt = dist[row][col] + weight[row][col];
	if(alt < dist[row + i][col + j]){
	  dist[row + i][col + j] = alt;
	  coords = (row + i) * 256 + col + j;
	  q.decrease(&coords, alt);
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

void Dungeon::generate_paths(){
  int weight[HEIGHT][WIDTH];
  //make nontunneling path
  weight_nontunnel(weight);
  dijkstra(nt_path, weight);
  //make tunneling path
  weight_tunnel(weight);
  dijkstra(t_path, weight);
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
