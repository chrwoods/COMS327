void Dungeon::generate_item(){
  if(item_templates.size() == 0) return; //no item templates
  Collectible item;
  while(1){ //choose the item to spawn
    int num = rand() % item_templates.size();
    if(item_templates[num].art > 1) continue; //artifact has been placed
    uint8_t rare = rand() % 100;
    if(rare >= item_templates[num].rarity) continue; //rare check failed
    item = item_templates[num].create();
    item_templates[num].art *= 2;
    break;
  }
  bool canplace = false;
  while(!canplace){ //place the monster
    item.row = 1 + rand() % (HEIGHT - 2);
    item.col = 1 + rand() % (WIDTH - 2);
    if(map[item.row][item.col] != 0) continue;
    canplace = true;
    for(int i = 0; i < num_up; i++){
      if(up[i].row == item.row && up[i].col == item.col){
	canplace = false;
	break;
      }
    }
    for(int i = 0; i < num_down && canplace; i++){
      if(down[i].row == item.row && down[i].col == item.col){
	canplace = false;
	break;
      }
    }
  }
  items.push_back(item);
}
