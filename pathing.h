#include "priorityq.h"

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
  q.num_nodes = 0;  
  dist[(*rlg).pc.row][(*rlg).pc.col] = 0;
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if(!((*rlg).pc.row == i && (*rlg).pc.col == j)){
	dist[i][j] = 17469 * weight[i][j]; //arbitrary large(ish) number
      }
      if(weight[i][j] >= 0/* && !((*rlg).pc.row == i && (*rlg).pc.col == j)*/){
	pq_add(&q, i, j, dist[i][j]);
      }
    }
  }
  while(q.num_nodes > 0){
    uint8_t row;
    uint8_t col;
    pq_pop(&q, &row, &col);
    for(int i = -1; i <= 1; i++){
      for(int j = -1; j <= 1; j++){
	if((i == 0 && j == 0) || weight[row+i][col+j] < 0) continue;
	int alt = dist[row][col] + weight[row][col];
	if(alt < dist[row + i][col + j]){
	  dist[row + i][col + j] = alt;
	  pq_decrease(&q, row + i, col + j, alt);
	}
      }
    }
  } 
}


void print_path(int path[HEIGHT][WIDTH]){
  for(int i = 0; i < HEIGHT; i++){
    for(int j = 0; j < WIDTH; j++){
      if(path[i][j] == 17469 || path[i][j] == -2 * 17469) printf("X");
      else if(path[i][j] < 0) printf(" ");
      else if(path[i][j] == 0) printf("@");
      else printf("%d", path[i][j] % 10);
    }
    printf("\n");
  }
}
