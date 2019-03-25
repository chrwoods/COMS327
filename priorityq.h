class PQ_node{
 public:
  PQ_node *next;
  void *data;
  int priority;
};

class PQueue{
 private:
  size_t mem_size;
  
 public:
  PQ_node *first;
  int num_nodes;

  PQueue(size_t m){
    num_nodes = 0;
    first = 0;
    mem_size = m;
  }

  void add(void *given_data, int weight){
    PQ_node *node = (PQ_node*)malloc(sizeof(PQ_node));
    void *data = malloc(mem_size);
    memcpy(data, given_data, mem_size);
    node->data = data;
    node->priority = weight;
    node->next = first;
    PQ_node *prev = 0;
    while(node->next != 0){
      //check if node is in the right spot
      if(node->priority < (node->next)->priority){
	break;
      }
      //shift the node down the list by 1
      if(prev != 0) prev->next = node->next;
      PQ_node *temp = (node->next)->next;
      (node->next)->next = node;
      prev=node->next;
      node->next = temp;
    }
    if(node->next == first){
      first = node;
    }
    num_nodes++;
  }
  
  void decrease(void *data, int new_weight){
    PQ_node *node = first;
    PQ_node *prev = 0;
    //find given cell
    while(1){
      if(node == 0){
	printf("Given row and column to decrease was not found in queue.\n");
	return;
      }
      if(memcmp(node->data, data, mem_size) == 0) break;
      prev = node;
      node = node->next;
    }
    //remove node
    if(prev == 0){
      first=node->next;   
    } else {
      prev->next=node->next;
    }
    free(node->data);
    free(node);
    num_nodes--;
    add(data, new_weight);
  }

  void pop(void *data, int *priority){
    if(num_nodes <= 0){
      printf("Error: Trying to pop from an empty queue.\n");
      return;
    }
    memcpy(data, first->data, mem_size);
    *priority = first->priority;
    PQ_node *tofree = first;
    first = first->next;
    free(tofree->data);
    free(tofree);
    num_nodes--;
  }

  void peek(void *data, int *priority){
    if(num_nodes <= 0){
      printf("Error: Trying to peek at an empty queue.\n");
      return;
    }
    memcpy(data, first->data, mem_size);
    *priority = first->priority;
  }

  void empty(){
    PQ_node *next = first;
    PQ_node *cur;
    while(next != 0) {
      cur = next;
      next = next->next;
      free(cur->data);
      free(cur);
    }
    first = 0;
  }
};
