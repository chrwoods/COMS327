struct pq_node{
  struct pq_node *next;
  uint8_t row;
  uint8_t col;
  int priority;
};

struct pq_queue{
  struct pq_node *first;
  int num_nodes;
};

void pq_add(struct pq_queue *q, uint8_t row, uint8_t col, int weight){
  struct pq_node *node = malloc(sizeof(struct pq_node));
  node->row = row;
  node->col = col;
  node->priority = weight;
  if(q->num_nodes == 0){
    //q = malloc(sizeof(struct pq_queue));
    q->num_nodes = 1;
    node->next = 0;
    q->first = node;
    //printf("Adding first node as row %d, col %d, next %p, priority %d, and this address as %p.\n", (q->first)->row, (q->first)->col, (q->first)->next, (q->first)->priority, (q->first));
    return;
  }
  node->next = q->first;
  struct pq_node *prev = 0;
  while(node->next != 0){
    //check if node is in the right spot
    if(node->priority < (node->next)->priority){
      break;
    }
    //shift the node down the list by 1
    if(prev != 0) prev->next = node->next;
    struct pq_node *temp = (node->next)->next;
    (node->next)->next = node;
    prev=node->next;
    node->next = temp;
  }
  if(node->next == q->first){
    q->first = node;
  }
  q->num_nodes++;
  //printf("Added node as row %d, col %d, next %p, priority %d, and this address as %p.\n    Num nodes is %d, and the address of the first node is %p.\n", node->row, node->col, node->next, node->priority, node, q->num_nodes, q->first);
}
  
void pq_decrease(struct pq_queue *q, uint8_t row, uint8_t col, int new_weight){
  struct pq_node *node = q->first;
  struct pq_node *prev = 0;
  //find given cell
  while(1){
    if(node == 0){
      printf("Given row and column to decrease was not found in queue.\n");
      return;
    }
    if(node->row == row && node->col == col) break;
    prev = node;
    node = node->next;
  }
  //remove node
  if(prev == 0){
    q->first=node->next;
    free(node);
  } else {
    prev->next=node->next;
    free(node);
  }
  q->num_nodes--;
  pq_add(q, row, col, new_weight);
  /*node->priority = new_weight;
  //move node down in the queue
  while(node->next != 0){
    //check if node is in the right spot
    if(node->priority < (node->next)->priority){
      break;
    }
    //shift the node down the list by 1
    if(prev != 0) prev->next = node->next;
    struct pq_node *temp = (node->next)->next;
    (node->next)->next = node;
    prev->next = node->next;
    node->next = temp;
  }*/
}

void pq_pop(struct pq_queue *q, uint8_t *row, uint8_t *col){
  if(q->num_nodes <= 0){
    printf("Error: Trying to pop from an empty queue.\n");
    return;
  }
  //printf("Popping row %d, col %d, with this pointer being %p, and the next being pointer %p. The queue is %p.\n", (q->first)->row, (q->first)->col, q->first, (q->first)->next, q);
  *row = (q->first)->row;
  *col = (q->first)->col;
  struct pq_node *tofree = q->first;
  q->first = (q->first)->next;
  free(tofree);
  q->num_nodes--;
}

//void pq_empty(struct pq_queue *q);
