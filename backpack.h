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
  WINDOW *list = newwin(22, 74, 1, 3);
  wborder(list, '|', '|', '-', '-', '+', '+', '+', '+');
  short inv_start = 1;
  mvwprintw(list, 0, 30, "[ Inventory ]");
  for(int i = 0; i < 10; i++){
    mvwprintw(list, inv_start + i, 2, "%1d - (empty)", i);
    if(pc.carry[i].isNull()) continue; //inventory slot is empty
    wattron(list, COLOR_PAIR(pc.carry[i].src->color));
    mvwaddch(list, inv_start + i, 6, pc.carry[i].src->symbol);
    wattroff(list, COLOR_PAIR(pc.carry[i].src->color));
    mvwprintw(list, inv_start + i, 7, ", %s, TYPE: %s, HIT: %d, DAM: %s,", pc.carry[i].src->name.c_str(), pc.carry[i].src->type.c_str(), pc.carry[i].hit, pc.carry[i].src->damage.toString().c_str());
    mvwprintw(list, ++inv_start + i, 6, "DODGE: %d, DEF: %d, WEIGHT: %d, SPEED: %d, ATTR: %d, VAL: %d", pc.carry[i].dodge, pc.carry[i].def, pc.carry[i].weight, pc.carry[i].speed, pc.carry[i].attr, pc.carry[i].value);
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
      if(pc.equip[i].isNull()) continue; //inventory slot is empty
      short desc_pos = strlen(type) + 9;
      wattron(list, COLOR_PAIR(pc.equip[i].src->color));
      mvwaddch(list, eq_start + i - page * 6, desc_pos, pc.equip[i].src->symbol);
      wattroff(list, COLOR_PAIR(pc.equip[i].src->color));
      mvwprintw(list, eq_start + i - page * 6, desc_pos + 1, ", %s, HIT: %d, DAM: %s,", pc.equip[i].src->name.c_str(), pc.equip[i].hit, pc.equip[i].src->damage.toString().c_str());
      mvwprintw(list, ++eq_start + i - page * 6, 5, "DODGE: %d, DEF: %d, WEIGHT: %d, SPEED: %d, ATTR: %d, VAL: %d", pc.equip[i].dodge, pc.equip[i].def, pc.equip[i].weight, pc.equip[i].speed, pc.equip[i].attr, pc.equip[i].value);
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

void Dungeon::pickup_items(){
  //check if PC is on any items
  /*char bg = background[pc.row][pc.col];
  if(bg == '<' || bg == '>' || bg == ' ' || bg == '.' || bg == '#'){
    update_status_text("   No items were found to pick up.");
    return;
  }*/
  //find pointers to items under PC
  vector<int> found_items; 
  for(int i = 0; i < items.size(); i++){
    if(items[i].row != pc.row || items[i].col != pc.col) continue;
    found_items.push_back(i);
  }
  if(found_items.size() <= 0){
    update_status_text("   No items were found to pick up.");
    return;
  }
  short pages = found_items.size() / 3;
  pages += found_items.size() % 3 ? 1 : 0;
  short page = 0;
  //open pickup menu
  WINDOW *list = newwin(15, 58, 3, 11);
  while(1){
    wborder(list, '|', '|', '-', '-', '+', '+', '+', '+');
    short inv_start = 1;
    mvwprintw(list, 0, 21, "[ Pickup Menu ]");
    for(int i = 0; i < 3; i++){
      if(i + page * 3 >= found_items.size()) break;
      int it_num = found_items[i + page * 3];
      if(items[it_num].row != pc.row || items[it_num].col != pc.col) continue;
      mvwprintw(list, ++inv_start + i, 2, "%1d - ", i + 1);
      wattron(list, COLOR_PAIR(items[it_num].src->color));
      mvwaddch(list, inv_start + i, 6, items[it_num].src->symbol);
      wattroff(list, COLOR_PAIR(items[it_num].src->color));
      mvwprintw(list, inv_start + i, 7, ", %s", items[it_num].src->name.c_str());
      mvwprintw(list, ++inv_start + i, 6, "TYPE: %s, HIT: %d, DAM: %s, DODGE: %d,", items[it_num].src->type.c_str(), items[it_num].hit, items[it_num].src->damage.toString().c_str(), items[it_num].dodge);
      mvwprintw(list, ++inv_start + i, 6, "DEF: %d, WEIGHT: %d, SPEED: %d, ATTR: %d, VAL: %d", items[it_num].def, items[it_num].weight, items[it_num].speed, items[it_num].attr, items[it_num].value);
    }
    if(pages > 1) mvwprintw(list, 14, 25, "[%2d/%-2d]", page + 1, pages);
    wrefresh(list);
    int ch = getch();
    while(ch == ERR) ch = getch();
    wclear(list);
    if(ch == 27 || ch == 'q') break;
    else if(ch == KEY_UP && page > 0) page--;
    else if(ch == KEY_DOWN && page < pages - 1) page++;
    else if(ch == '1' || ch == '2' || ch == '3'){
      int inv_slot = -1;
      for(int i = 0; i < 10; i++){
	if(!pc.carry[i].isNull()) continue;
	inv_slot = i;
	break;
      }
      if(inv_slot < 0) {
	update_status_text("   No open carry slots!");
	break;
      }
      if(ch - 49 + page * 3 >= found_items.size()){
	continue;
      }
      //insert item into inventory and remove from map
      pc.carry[inv_slot] = items[found_items[ch - 49 + page * 3]];
      items.erase(items.begin() + found_items[ch - 49 + page * 3]);
      update_background();
      //regenerate list of found items
      found_items.clear();
      for(int i = 0; i < items.size(); i++){
	if(items[i].row != pc.row || items[i].col != pc.col) continue;
	found_items.push_back(i);
      }
      if(found_items.size() <= 0) break;
      pages = found_items.size() / 3 + found_items.size() % 3 ? 1 : 0;
      if(page >= pages) page--;
    }
  }
  wrefresh(list);
  delwin(list);
  print_map();
  refresh();
}

void Dungeon::wear_item(){
  WINDOW *list = newwin(14, 50, 3, 15);
  wborder(list, '|', '|', '-', '-', '+', '+', '+', '+');
  mvwprintw(list, 0, 19, "[ Wear Item ]");
  for(int i = 0; i < 10; i++){
    mvwprintw(list, i + 1, 2, "%1d - (empty)", i);
    if(pc.carry[i].isNull()) continue; //inventory slot is empty
    wattron(list, COLOR_PAIR(pc.carry[i].src->color));
    mvwaddch(list, i + 1, 6, pc.carry[i].src->symbol);
    wattroff(list, COLOR_PAIR(pc.carry[i].src->color));
    mvwprintw(list, i + 1, 7, ", %s, %s", pc.carry[i].src->name.c_str(), pc.carry[i].src->type.c_str());
  }
  wrefresh(list);
  int ch = getch();
  while(ch != 27 && !(ch >= 48 && ch <= 57)) ch = getch();
  if(ch != 27){
    signed short eq_slot = -1;
    short inv_slot = ch - 48;
    if(pc.carry[inv_slot].isNull()){
      update_status_text("   Given slot is empty!");
      wclear(list);
      wrefresh(list);
      delwin(list);
      print_map();
      refresh();
      return;
    }
    switch(pc.carry[inv_slot].src->symbol){
    case '|':
      eq_slot = 0;
      break;
    case ')':
      eq_slot = 1;
      break;
    case '}':
      eq_slot = 2;
      break;
    case '[':
      eq_slot = 3;
      break;
    case ']':
      eq_slot = 4;
      break;
    case '(':
      eq_slot = 5;
      break;
    case '{':
      eq_slot = 6;
      break;
    case '\\':
      eq_slot = 7;
      break;
    case '\"':
      eq_slot = 8;
      break;
    case '_':
      eq_slot = 9;
      break;
    case '=':
      eq_slot = 10;
      break;
    } 
    if(eq_slot == 10){
      mvwprintw(list, 12, 1, "Which ring slot? (l/r)");
      wrefresh(list);
      while(ch != 'l' && ch != 'r') ch = getch();
      if(ch == 'r') eq_slot++;
    } else if (eq_slot < 0){
      update_status_text("   This item is unequippable.");
      wclear(list);
      wrefresh(list);
      delwin(list);
      print_map();
      refresh();
      return;
    }
    Collectible temp_item = pc.equip[eq_slot];
    pc.equip[eq_slot] = pc.carry[inv_slot];
    pc.carry[inv_slot] = temp_item;
    string status_text = "   \'";
    status_text.append(pc.equip[eq_slot].src->name);
    status_text.append("\' was equipped");
    if(!pc.carry[inv_slot].isNull()){
      status_text.append(", swapping out \'");
      status_text.append(pc.carry[inv_slot].src->name);
      status_text.append("\'");
    }
    status_text.append(".");
    update_status_text(status_text.c_str());
  }
  wclear(list);
  wrefresh(list);
  delwin(list);
  print_map();
  refresh();
}
  
void Dungeon::takeoff_item(){
  WINDOW *list = newwin(14, 50, 3, 15);
  wborder(list, '|', '|', '-', '-', '+', '+', '+', '+');
  mvwprintw(list, 0, 17, "[ Take Off Item ]");
  for(int i = 0; i < 12; i++){
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
    mvwprintw(list, i + 1, 2, "%s (%c) - (empty)", type, i + 97);
    if(pc.equip[i].isNull()) continue; //inventory slot is empty
    short desc_pos = strlen(type) + 9;
    wattron(list, COLOR_PAIR(pc.equip[i].src->color));
    mvwaddch(list, i + 1, desc_pos, pc.equip[i].src->symbol);
    wattroff(list, COLOR_PAIR(pc.equip[i].src->color));
    mvwprintw(list, i + 1, desc_pos + 1, ", %s", pc.equip[i].src->name.c_str());
  }
  wrefresh(list);
  int ch = getch();
  while(ch != 27 && !(ch >= 97 && ch <= 108)) ch = getch();
  if(ch != 27){
    //check if there is equipment to take off
    short eq_slot = ch - 97;
    if(pc.equip[eq_slot].isNull()){
      update_status_text("   Given slot is empty!");
      wclear(list);
      wrefresh(list);
      delwin(list);
      print_map();
      refresh();
      return;
    }
    //find carry slot
    signed short inv_slot = -1;
    for(int i = 0; i < 10; i++){
      if(!pc.carry[i].isNull()) continue;
      inv_slot = i;
      break;
    }
    if(inv_slot < 0) {
      update_status_text("   No open carry slots!");
      wclear(list);
      wrefresh(list);
      delwin(list);
      print_map();
      refresh();
      return;
    }
    pc.carry[inv_slot] = pc.equip[eq_slot];
    pc.equip[eq_slot] = NULL_ITEM;
    string status_text = "   \'";
    status_text.append(pc.carry[inv_slot].src->name);
    status_text.append("\' was taken off into slot ");
    status_text.append(to_string(inv_slot));
    status_text.append(".");
    update_status_text(status_text.c_str());
  }
  wclear(list);
  wrefresh(list);
  delwin(list);
  print_map();
  refresh();
} 

void Dungeon::drop_item(){
  WINDOW *list = newwin(12, 50, 3, 15);
  wborder(list, '|', '|', '-', '-', '+', '+', '+', '+');
  mvwprintw(list, 0, 19, "[ Drop Item ]");
  for(int i = 0; i < 10; i++){
    mvwprintw(list, i + 1, 2, "%1d - (empty)", i);
    if(pc.carry[i].isNull()) continue; //inventory slot is empty
    wattron(list, COLOR_PAIR(pc.carry[i].src->color));
    mvwaddch(list, i + 1, 6, pc.carry[i].src->symbol);
    wattroff(list, COLOR_PAIR(pc.carry[i].src->color));
    mvwprintw(list, i + 1, 7, ", %s, %s", pc.carry[i].src->name.c_str(), pc.carry[i].src->type.c_str());
  }
  wrefresh(list);
  int ch = getch();
  while(ch != 27 && !(ch >= 48 && ch <= 57)) ch = getch();
  if(ch != 27){
    short inv_slot = ch - 48;
    if(pc.carry[inv_slot].isNull()){
      update_status_text("   Given slot is empty!");
      wclear(list);
      wrefresh(list);
      delwin(list);
      print_map();
      refresh();
      return;
    }
    Collectible temp_item = pc.carry[inv_slot];
    pc.carry[inv_slot] = NULL_ITEM;
    temp_item.row = pc.row;
    temp_item.col = pc.col;
    items.push_back(temp_item);
    update_background();
    string status_text = "   \'";
    status_text.append(temp_item.src->name);
    status_text.append("\' was dropped.");
    update_status_text(status_text.c_str());
  }
  wclear(list);
  wrefresh(list);
  delwin(list);
  print_map();
  refresh();
}

void Dungeon::destroy_item(){
  WINDOW *list = newwin(12, 50, 3, 15);
  wborder(list, '|', '|', '-', '-', '+', '+', '+', '+');
  mvwprintw(list, 0, 16, "[ Destroy Item ]");
  for(int i = 0; i < 10; i++){
    mvwprintw(list, i + 1, 2, "%1d - (empty)", i);
    if(pc.carry[i].isNull()) continue; //inventory slot is empty
    wattron(list, COLOR_PAIR(pc.carry[i].src->color));
    mvwaddch(list, i + 1, 6, pc.carry[i].src->symbol);
    wattroff(list, COLOR_PAIR(pc.carry[i].src->color));
    mvwprintw(list, i + 1, 7, ", %s, %s", pc.carry[i].src->name.c_str(), pc.carry[i].src->type.c_str());
  }
  wrefresh(list);
  int ch = getch();
  while(ch != 27 && !(ch >= 48 && ch <= 57)) ch = getch();
  if(ch != 27){
    short inv_slot = ch - 48;
    if(pc.carry[inv_slot].isNull()){
      update_status_text("   Given slot is empty!");
      wclear(list);
      wrefresh(list);
      delwin(list);
      print_map();
      refresh();
      return;
    }
    string temp_name = pc.carry[inv_slot].src->name;
    pc.carry[inv_slot] = NULL_ITEM;
    if(temp_name.compare("the One Ring") == 0){
      update_status_text("   The One Ring has been dropped into Mount Doom!");
    } else if(temp_name.compare("Mjolnir") == 0){
      update_status_text("   Mjolnir has been shattered!");
    } else if(temp_name.compare("the Heart of the Ocean") == 0){
      update_status_text("   The Heart of the Ocean has been returned to the sea!");
    } else {
      string status_text = "   \'";
      status_text.append(temp_name);
      status_text.append("\' was destroyed!");
      update_status_text(status_text.c_str());
    }
  }
  wclear(list);
  wrefresh(list);
  delwin(list);
  print_map();
  refresh();
}

void Dungeon::display_item_info(int num){
  if(num < 0 || num >= items.size()) return;
  WINDOW *list = newwin(15, 80, 4, 0);
  wborder(list, '|', '|', '-', '-', '+', '+', '+', '+');
  mvwprintw(list, 0, 36, "[ Item ]");
  mvwprintw(list, 1, 1, "x - %s, %s", items[num].src->name.c_str(), items[num].src->type.c_str());
  wattron(list, COLOR_PAIR(items[num].src->color));
  mvwaddch(list, 1, 1, items[num].src->symbol);
  wattroff(list, COLOR_PAIR(items[num].src->color));
  string desc = items[num].src->desc;
  short offset = 0;
  while(desc.length() > 1){
    string desc_line = desc.substr(0, desc.find_first_of("\n"));
    mvwprintw(list, 2 + ++offset, 1, desc_line.c_str());
    desc = desc.substr(desc.find_first_of("\n") + 1);
  }
  mvwprintw(list, offset + 4, 1, "HIT BONUS: %d, DAMAGE BONUS: %s, DODGE BONUS: %d", items[num].hit, items[num].src->damage.toString().c_str(), items[num].dodge);
  mvwprintw(list, offset + 5, 1, "DEFENSE BONUS: %d, WEIGHT: %d, SPEED BONUS: %d", items[num].def, items[num].weight, items[num].speed);
  mvwprintw(list, offset + 6, 1, "SPECIAL ATTRIBUTE: %d, VALUE: %d", items[num].attr, items[num].value);
  char vert[6] = "north";
  char horz[5] = "east";
  int v_dist = pc.row - items[num].row;
  int h_dist = items[num].col - pc.col;
  if(v_dist < 0) {
    v_dist *= -1;
    vert[0] = 's';
    vert[2] = 'u';
  }
  if(h_dist < 0) {
    h_dist *= -1;
    horz[0] = 'w';
    horz[1] = 'e';
  }
  mvwprintw(list, offset + 8, 1, "DISTANCE: %d (%d %s, %d %s)", max(abs(items[num].row - pc.row), abs(items[num].col - pc.col)), v_dist, vert, h_dist, horz);
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
  
void Dungeon::inspect_item(){
  bool empty = true;
  for(int i = 0; i < 10; i++){
    if(!pc.carry[i].isNull()){
      empty = false;
      break;
    }
  }
  if(empty){
    update_status_text("   Inventory is empty, nothing to inspect.");
    refresh();
    return;
  }
  update_status_text("   Which inventory slot? (0-9)");
  refresh();
  int num = -1;
  while(1){
    int ch = getch();
    if(ch == 27){
      update_status_text("");
      refresh();
      return;
    } else if (!(ch >= 48 && ch <= 57)) continue;
    if(pc.carry[ch - 48].isNull()){
      update_status_text("   Given slot is empty, please choose another slot.");
      refresh();
      continue;
    }
    num = ch - 48;
    break;
  }
  update_status_text("");
  refresh();
  WINDOW *list = newwin(15, 80, 4, 0);
  wborder(list, '|', '|', '-', '-', '+', '+', '+', '+');
  mvwprintw(list, 0, 36, "[ Item ]");
  mvwprintw(list, 1, 1, "x - %s, %s", pc.carry[num].src->name.c_str(), pc.carry[num].src->type.c_str());
  wattron(list, COLOR_PAIR(pc.carry[num].src->color));
  mvwaddch(list, 1, 1, pc.carry[num].src->symbol);
  wattroff(list, COLOR_PAIR(pc.carry[num].src->color));
  string desc = pc.carry[num].src->desc;
  short offset = 0;
  while(desc.length() > 1){
    string desc_line = desc.substr(0, desc.find_first_of("\n"));
    mvwprintw(list, 2 + ++offset, 1, desc_line.c_str());
    desc = desc.substr(desc.find_first_of("\n") + 1);
  }
  mvwprintw(list, offset + 4, 1, "HIT BONUS: %d, DAMAGE BONUS: %s, DODGE BONUS: %d", pc.carry[num].hit, pc.carry[num].src->damage.toString().c_str(), pc.carry[num].dodge);
  mvwprintw(list, offset + 5, 1, "DEFENSE BONUS: %d, WEIGHT: %d, SPEED BONUS: %d", pc.carry[num].def, pc.carry[num].weight, pc.carry[num].speed);
  mvwprintw(list, offset + 6, 1, "SPECIAL ATTRIBUTE: %d, VALUE: %d", pc.carry[num].attr, pc.carry[num].value);
  char vert[6] = "north";
  char horz[5] = "east";
  int v_dist = pc.row - pc.carry[num].row;
  int h_dist = pc.carry[num].col - pc.col;
  if(v_dist < 0) {
    v_dist *= -1;
    vert[0] = 's';
    vert[2] = 'u';
  }
  if(h_dist < 0) {
    h_dist *= -1;
    horz[0] = 'w';
    horz[1] = 'e';
  }
  mvwprintw(list, offset + 8, 1, "DISTANCE: %d (%d %s, %d %s)", max(abs(pc.carry[num].row - pc.row), abs(pc.carry[num].col - pc.col)), v_dist, vert, h_dist, horz);
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
