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
  WINDOW *list = newwin(24, 80, 0, 0);
  wborder(list, '|', '|', '-', '-', '+', '+', '+', '+');
  short inv_start = 1;
  for(int i = 0; i < 10; i++){
    mvwprintw(list, inv_start + i, 2, "%1d: ", i);
    if(pc.carry[i] == 0) continue; //inventory slot is empty
    wattron(list, COLOR_PAIR(pc.carry[i]->src->color));
    mvwaddch(list, inv_start + i, 5, pc.carry[i]->src->symbol);
    wattroff(list, COLOR_PAIR(pc.carry[i]->src->color));
    mvwprintw(list, inv_start + i, 6, ", Name: %s, Damage: %s, Speed: %s", pc.carry[i]->src->name, pc.carry[i]->src->damage.toString(), pc.carry[i]->speed);
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
