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

void Dungeon::print_inventory(){
  //test add
  pc.carry[0] = &(items[0]);

  
  WINDOW *list = newwin(22, 70, 1, 5);
  wborder(list, '|', '|', '-', '-', '+', '+', '+', '+');
  short inv_start = 1;
  mvwprintw(list, 0, 28, "[ Inventory ]");
  for(int i = 0; i < 10; i++){
    mvwprintw(list, inv_start + i, 2, "%1d - (empty)", i);
    if(pc.carry[i] == 0) continue; //inventory slot is empty
    wattron(list, COLOR_PAIR(pc.carry[i]->src->color));
    mvwaddch(list, inv_start + i, 6, pc.carry[i]->src->symbol);
    wattroff(list, COLOR_PAIR(pc.carry[i]->src->color));
    mvwprintw(list, inv_start + i, 7, ", %s, HIT: %d, DAM: %s, DODGE: %d,", pc.carry[i]->src->name.c_str(), pc.carry[i]->hit, pc.carry[i]->src->damage.toString().c_str(), pc.carry[i]->dodge);
    mvwprintw(list, ++inv_start + i, 6, "DEF: %d, WEIGHT: %d, SPEED: %d, ATTR: %d, VAL: %d", pc.carry[i]->def, pc.carry[i]->weight, pc.carry[i]->speed, pc.carry[i]->attr, pc.carry[i]->value);
  }
  wrefresh(list);
  while(getch() != 27){
    //wait until escape character
  }
  wclear(list);
  wrefresh(list);
  delwin(list);
  print_map();
  refresh();
}
