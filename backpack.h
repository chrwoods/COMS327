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
  for(int i = 0; i < 10; i++) pc.equip[i] = &(items[i]);
  
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
    mvwprintw(list, inv_start + i, 7, ", %s, TYPE: %s, HIT: %d, DAM: %s,", pc.carry[i]->src->name.c_str(), pc.carry[i]->src->type.c_str(), pc.carry[i]->hit, pc.carry[i]->src->damage.toString().c_str());
    mvwprintw(list, ++inv_start + i, 6, "DODGE: %d, DEF: %d, WEIGHT: %d, SPEED: %d, ATTR: %d, VAL: %d", pc.carry[i]->dodge, pc.carry[i]->def, pc.carry[i]->weight, pc.carry[i]->speed, pc.carry[i]->attr, pc.carry[i]->value);
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

void Dungeon::print_equipment(){
  //test add
  for(int i = 0; i < 5; i++) pc.equip[i] = &(items[i]);
  WINDOW *list = newwin(22, 78, 1, 1);
  short eq_start;
  short page = 0;
  while(1){
    wborder(list, '|', '|', '-', '-', '+', '+', '+', '+');
    mvwprintw(list, 0, 32, "[ Equipment ]");
    eq_start = 1;
    for(int i = 0 + page * 6; i < 6 + page * 6; i++){
      const char *type;
      switch(i){
      case 0:
	type = "Weapon";
	break;
      case 1:
	type = "Offhand";
	break;
      case 2:
	type = "Ranged";
	break;
      case 3:
	type = "Armor";
	break;
      case 4:
	type = "Helmet";
	break;
      case 5:
	type = "Cloak";
	break;
      case 6:
	type = "Gloves";
	break;
      case 7:
	type = "Boots";
	break;
      case 8:
	type = "Amulet";
	break;
      case 9:
	type = "Light";
	break;
      case 10:
      case 11:
	type = "Ring";
	break;
      default:
	type = "Underwear?"; //this should never happen
      }
      mvwprintw(list, ++eq_start + i - page * 6, 2, "%s (%c) - (empty)", type, i + 97);
      if(pc.equip[i] == 0) continue; //inventory slot is empty
      short desc_pos = strlen(type) + 9;
      wattron(list, COLOR_PAIR(pc.equip[i]->src->color));
      mvwaddch(list, eq_start + i - page * 6, desc_pos, pc.equip[i]->src->symbol);
      wattroff(list, COLOR_PAIR(pc.equip[i]->src->color));
      mvwprintw(list, eq_start + i - page * 6, desc_pos + 1, ", %s, HIT: %d, DAM: %s,", pc.equip[i]->src->name.c_str(), pc.equip[i]->hit, pc.equip[i]->src->damage.toString().c_str());
      mvwprintw(list, ++eq_start + i - page * 6, 5, "DODGE: %d, DEF: %d, WEIGHT: %d, SPEED: %d, ATTR: %d, VAL: %d", pc.equip[i]->dodge, pc.equip[i]->def, pc.equip[i]->weight, pc.equip[i]->speed, pc.equip[i]->attr, pc.equip[i]->value);
    }
    mvwprintw(list, 21, 35, "[ %d/2 ]", page + 1);
    wrefresh(list);
    int ch = 0;
    while(ch != KEY_DOWN && ch != KEY_UP && ch != 27 && ch != 'q') ch = getch();
    wclear(list);
    if(ch == 27 || ch == 'q') break;
    if(ch == KEY_UP && page == 1) page--;
    if(ch == KEY_DOWN && page == 0) page++;
  }  
  wrefresh(list);
  delwin(list);
  print_map();
  refresh();
}  
