struct pq_node{
  struct pq_node *next;
  void *data;
  int priority;
};

struct pq_queue{
  struct pq_node *first;
  size_t mem_size;
  int num_nodes;
};

void pq_init(struct pq_queue *q, size_t mem_size){
  q->num_nodes = 0;
  q->first = 0;
  q->mem_size = mem_size;
}

void pq_add(struct pq_queue *q, void *given_data, int weight){
  struct pq_node *node = malloc(sizeof(struct pq_node));
  void *data = malloc(q->mem_size);
  memcpy(data, given_data, q->mem_size);
  node->data = data;
  node->priority = weight;
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
  
void pq_decrease(struct pq_queue *q, void *data, int new_weight){
  struct pq_node *node = q->first;
  struct pq_node *prev = 0;
  //find given cell
  while(1){
    if(node == 0){
      printf("Given row and column to decrease was not found in queue.\n");
      return;
    }
    if(memcmp(node->data, data, q->mem_size) == 0) break;
    prev = node;
    node = node->next;
  }
  //remove node
  if(prev == 0){
    q->first=node->next;   
  } else {
    prev->next=node->next;
  }
  free(node->data);
  free(node);
  q->num_nodes--;
  pq_add(q, data, new_weight);
}

void pq_pop(struct pq_queue *q, void *data, int *priority){
  if(q->num_nodes <= 0){
    printf("Error: Trying to pop from an empty queue.\n");
    return;
  }
  //printf("Popping row %d, col %d, with this pointer being %p, and the next being pointer %p. The queue is %p.\n", (q->first)->row, (q->first)->col, q->first, (q->first)->next, q);
  memcpy(data, (q->first)->data, q->mem_size);
  *priority = (q->first)->priority;
  struct pq_node *tofree = q->first;
  q->first = (q->first)->next;
  free(tofree->data);
  free(tofree);
  q->num_nodes--;
}

void pq_peek(struct pq_queue *q, void *data, int *priority){
  if(q->num_nodes <= 0){
    printf("Error: Trying to peek at an empty queue.\n");
    return;
  }
  memcpy(data, (q->first)->data, q->mem_size);
  *priority = (q->first)->priority;
}

void pq_empty(struct pq_queue *q){
  struct pq_node *next = q->first;
  struct pq_node *cur;
  while(next != 0) {
    cur = next;
    next = next->next;
    free(cur->data);
    free(cur);
  }
  q->first = 0;
}
