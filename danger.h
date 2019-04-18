void Dungeon::generate_monster(){
  Lifeform mon;
  while(1){ //choose the monster
    int num = rand() % monster_templates.size();
    if(monster_templates[num].unique() && monster_templates[num].placed) continue; //unique monster has been placed already
    uint8_t rare = rand() % 100;
    if(rare >= monster_templates[num].rarity) continue; //rare check failed
    mon = monster_templates[num].create();
    monster_templates[num].placed = true;
    break;
  }
  while(1){ //place the monster
    uint8_t row = rand() % HEIGHT;
    uint8_t col = rand() % WIDTH;
    if(map[row][col] != 0) continue;
    if(pc.row == row && pc.col == col) continue;
    mon.row = row;
    mon.col = col;
    break;
  }
  mon.pc_row = 0;
  mon.pc_col = 0;
  monsters.push_back(mon);
}

int Dungeon::line_of_sight(int num){
  uint8_t pc_row = pc.row;
  uint8_t pc_col = pc.col;
  uint8_t row = monsters[num].row;
  uint8_t col = monsters[num].col;
  if(pc_col != col){ //uses Bresenham's line algorithm
    double derr = ((double)(row - pc_row))/((double)(col - pc_col));
    if(derr < 0) derr *= -1;
    double err = 0.0;
    int y = pc_row;
    for(int x = pc_col; abs(col - x) > 0; x += ((col - pc_col) > 0) - ((col - pc_col) < 0)){
      err += derr;
      if(err < .5 && map[y][x] != 0) return 0;
      while(err >= .5){
	y += ((row - pc_row) > 0) - ((row - pc_row) < 0);
	err -= 1.0;
	if(map[y][x] != 0) return 0;
      }
    }
  } else {
    int dir = ((row - pc_row) > 0) - ((row - pc_row) < 0);
    while(pc_row != row){
      if(map[pc_row][pc_col] != 0) return 0;
      pc_row += dir;
    }
  }
  return 1;
}

void Dungeon::kill_monster(int num){
  monsters[num].hp = 0; //soon to be deprecated in 1.09
}
  
int Dungeon::move_monster(int num){
  if(num < 0 || num >= monsters.size()) return -1;
  //decide where to move
  if(monsters[num].src->telepathic()){
    monsters[num].pc_row = pc.row;
    monsters[num].pc_col = pc.col;
  } else {
    if(line_of_sight(num)){
      monsters[num].pc_row = pc.row;
      monsters[num].pc_col = pc.col;
    } else if(!monsters[num].src->smart()){
      monsters[num].pc_row = 0;
      monsters[num].pc_col = 0;
    } //otherwise stored pc location will not change
  }
  
  //find the neighbors
  int neighbors[3][3];
  uint8_t row = monsters[num].row;
  uint8_t col = monsters[num].col;
  for(int i = -1; i <= 1; i++){
    for(int j = -1; j <= 1; j++){
      if(monsters[num].src->pass()){
	neighbors[i + 1][j + 1] = p_path[row + i][col + j];
      } else if(monsters[num].src->tunnel()){
	neighbors[i + 1][j + 1] = t_path[row + i][col + j];
      } else {	
	neighbors[i + 1][j + 1] = nt_path[row + i][col + j];
      }
    }
  }
  
  //decide where to move
  int destination = 4; //center
  if(monsters[num].pc_row == pc.row && monsters[num].pc_col == pc.col && monsters[num].src->smart()){
    int shortest_dist = 17469; //arbitrarily large(ish)
    for(int i = 0; i < 9; i++){	
      if(neighbors[i / 3][i % 3] < shortest_dist && neighbors[i / 3][i % 3] >= 0){
	shortest_dist = neighbors[i / 3][i % 3];
	destination = i;
      } else if (neighbors[i / 3][i % 3] == shortest_dist && (i / 3 == 1 || i % 3 == 1)){ //tries to move horizontally instead of diagonally so it doesn't look weird
	destination = i;
      }
    }
  } else if(monsters[num].pc_row > 0){ //if there is a stored PC location
    int v_dist = monsters[num].pc_row - row;
    int h_dist = monsters[num].pc_col - col;
    int v_dir = 0;
    int h_dir = 0;
    if(v_dist != 0) v_dir = abs(v_dist)/v_dist; //gets sign
    if(h_dist != 0) h_dir = abs(h_dist)/h_dist;
    if(neighbors[v_dir + 1][h_dir + 1] >= 0){
      //if can go diagonal, continue
    } else if(neighbors[1][h_dir + 1] >= 0){
      v_dir = 0; //if can go horizontal, continue
    } else if(neighbors[v_dir + 1][1] >= 0){
      h_dir = 0; //if can go vertical, continue
    } else {
      v_dir = 0;
      h_dir = 0;//otherwise it will not move
    }
    destination = v_dir * 3 + h_dir + 4;
  }
  
  //erratic behavior
  if(monsters[num].src->erratic() && rand() % 2 == 1){
    int counter = 0;
    while(1){
      if(counter++ > 50) { //checks for infinite loop from PC teleporting into rock
	destination = 4;
	break;
      }
      destination = rand() % 9;
      if(neighbors[destination / 3][destination % 3] >= 0) break; //check if moving here is possible
    }
  }
  
  //move in the given direction
  if(!fog || visible[row][col]){
    attron(COLOR_PAIR(WHITE_PAIR));
    mvaddch(row, col, background[row][col]); //reset where monster was
    attroff(COLOR_PAIR(WHITE_PAIR));
  } else {
    attron(COLOR_PAIR(GRAY_PAIR));
    mvaddch(row, col, memory[row][col]);
    attroff(COLOR_PAIR(GRAY_PAIR));
  }
  if(!monsters[num].src->pass() && map[row - 1 + destination / 3][col - 1 + destination % 3] > 0){
    if(map[row - 1 + destination / 3][col - 1 + destination % 3] < 85)
      map[row - 1 + destination / 3][col - 1 + destination % 3] = 0;
    else map[row - 1 + destination / 3][col - 1 + destination % 3] -= 85;
    generate_paths(); //generate new paths because hardness changed
    update_background(); //update background because of new possible corridor
  }
  if(monsters[num].src->pass() || map[row - 1 + destination / 3][col - 1 + destination % 3] == 0){
    row = monsters[num].row + (destination / 3) - 1;
    col = monsters[num].col + (destination % 3) - 1;

    //check combat
    if(pc.row == row && pc.col == col){
      if(fight(num, false)){
	attron(COLOR_PAIR(monsters[num].src->colors[0]));
	mvaddch(row, col, monsters[num].src->symbol); //add monster at pc's spot
	attroff(COLOR_PAIR(monsters[num].src->colors[0]));
	return 1; //player has been slain
      }
      destination = 4;
      row = monsters[num].row;
      col = monsters[num].col;
    }
    
    //place on map again
    if(!fog || visible[row][col]) {
      attron(COLOR_PAIR(monsters[num].src->colors[0])); //for now we will only print the first color (TODO)
      mvaddch(row, col, monsters[num].src->symbol); //add monster at new spot
      attroff(COLOR_PAIR(monsters[num].src->colors[0]));
    }
    monsters[num].row = row;
    monsters[num].col = col;
    if(destination == 4) {
      refresh();
      return 0;
    }
    
    //check for monsters to displace
    for(int i = 0; i < monsters.size(); i++){
      if(num == i) continue; //according to Newton, monsters can't logistically push themselves out of the way
      if(monsters[i].row == row && monsters[i].col == col){
	//displace the monster
	int v_dir = destination / 3 - 1;
	int h_dir = destination % 3 - 1;
	bool displaced = false;
	int v;
	int h;
	if(v_dir * h_dir != 0){ //monster moved diagonally
	  for(int j = 0; j < 5; j++){
	    v = v_dir - (v_dir * ((j + 1) % 2) * (j / 2)); //math magic
	    h = h_dir - (v_dir * ((j + 1) / 2) * (j % 2));
	    if(row + v <= 0 || row + v >= HEIGHT - 1 || col + h <= 0 || col + h >= WIDTH - 1) continue; //cannot push out of bounds
	    if(background[row + v][col + h] == (A_CHARTEXT & mvinch(row + v, col + h))){ //this checks if the background is equal to the
	      displaced = true;
	      break;
	    }
	  }
	} else if(v_dir == 0) { //horizontal move
	  for(int j = 1; j >= 0; j--){
	    for(int k = 0; k <= 2; k++){
	      v = k;
	      if(k == 2) v = -1;
	      h = h_dir * j;
	      if(row + v <= 0 || row + v >= HEIGHT - 1 || col + h <= 0 || col + h >= WIDTH - 1) continue; //cannot push out of bounds
	      if(background[row + v][col + h] == (A_CHARTEXT & mvinch(row + v, col + h))){
	        displaced = true;
		break;
	      }
	    }
	  }
	} else { //vertical move
	  for(int j = 1; j >= 0; j--){
	    for(int k = 0; k <= 2; k++){
	      h = k;
	      if(k == 2) h = -1;
	      v = h_dir * j;
	      if(row + v <= 0 || row + v >= HEIGHT - 1 || col + h <= 0 || col + h >= WIDTH - 1) continue; //cannot push out of bounds
	      if(background[row + v][col + h] == (A_CHARTEXT & mvinch(row + v, col + h))){
		displaced = true;
		break;
	      }
	    }
	  }
	}
	if(!displaced){
	  destination = 8 - destination; //invert destination (swap)
	  v = (destination / 3) - 1;
	  h = (destination % 3) - 1;
	}
	//move monster
	monsters[i].row += v;
	monsters[i].col += h;
	if(!fog || visible[row + v][col + h]) {
	  attron(COLOR_PAIR(monsters[i].src->colors[0]));
	  mvaddch(row + v, col + h, monsters[i].src->symbol);
	  attroff(COLOR_PAIR(monsters[i].src->colors[0]));
	}
      }
    }
  }
  
  //destroy checking
  if(monsters[num].src->destroy()){
    char bg = background[row][col];
    if(bg != ' ' && bg != '.' && bg != '#' && bg != '<' && bg != '>'){ //if new monster location containts an item (is not empty space)
      for(int i = 0; i < items.size(); i++){
	if(row == items[i].row && col == items[i].col){
	  items.erase(items.begin() + i--); //remove the item from the vector and step backwards
	}
      }
      update_background(); //update background to reflect item change
      print_map(); //print new map
    }
  }
  refresh();
  return 0;
}

void Dungeon::print_monster_list(){
  WINDOW *list = newwin(HEIGHT + 3, WIDTH, 0, 0);
  wattron(list, COLOR_PAIR(WHITE_PAIR));
  int col = 17;
  mvwprintw(list, 0, col, "                           _                ");
  mvwprintw(list, 1, col, "                          | |               ");
  mvwprintw(list, 2, col, " _ __ ___   ___  _ __  ___| |_ ___ _ __ ___ ");
  mvwprintw(list, 3, col, "| \'_ ` _ \\ / _ \\| \'_ \\/ __| __/ _ \\ \'__/ __|");
  mvwprintw(list, 4, col, "| | | | | | (_) | | | \\__ \\ ||  __/ |  \\__ \\");
  mvwprintw(list, 5, col, "|_| |_| |_|\\___/|_| |_|___/\\__\\___|_|  |___/");
  col += 6;
  mvwprintw(list, 7, col, "+------------------------------+");
  mvwprintw(list, 8, col, "|                              |");
  mvwprintw(list, 22, col, "+------------------------------+");
  int key;
  int alive_monsters = 0;
  int monster_array_locs[monsters.size()];
  for(int i = 0; i < monsters.size(); i++){
    if(monsters[i].hp != 0) monster_array_locs[alive_monsters++] = i;
  }
  int page = 0;
  int num_pages = alive_monsters / 6 + (alive_monsters % 6 > 0);
  do{
    if(page < 0) page = 0;
    for(int i = 0; i < 6; i++){
      if(page * 6 + i >= alive_monsters){
	mvwprintw(list, 9 + i * 2, col, "|                              |");
	mvwprintw(list, 10 + i * 2, col, "|                              |");
	continue;
      }
      int num = monster_array_locs[page * 6 + i];
      char vert[6] = "north";
      char horz[5] = "east";
      int v_dist = pc.row - monsters[num].row;
      int h_dist = monsters[num].col - pc.col;
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
      mvwprintw(list, 9 + i * 2, col, "|     +, %2d %s, %2d %s     |", v_dist, vert, h_dist, horz);
      //print monster symbol in its own color
      wattroff(list, COLOR_PAIR(WHITE_PAIR));
      wattron(list, COLOR_PAIR(monsters[num].src->colors[0]));
      mvwaddch(list, 9 + i * 2, col + 6, monsters[num].src->symbol);
      wattroff(list, COLOR_PAIR(monsters[num].src->colors[0]));
      wattron(list, COLOR_PAIR(WHITE_PAIR));
      mvwprintw(list, 10 + i * 2, col, "|                              |");
    }
    if(alive_monsters <= 0) page = -1;
    mvwprintw(list, 21, col, "|           %4d/%-4d          |", page + 1, num_pages);
    wrefresh(list);
    key = getch();
    if(key == KEY_DOWN && (page < num_pages - 1)) page++;
    if(key == KEY_UP && (page > 0)) page--;
  }while(key != 27); //27 = escape key (also is alt key, which will also close monster list), causing a ~.5 second pause
  wattroff(list, COLOR_PAIR(WHITE_PAIR));  
  wclear(list);
  delwin(list);
  clear();
  print_map();
}
    
void Dungeon::display_monster_info(int num){
  if(num < 0 || num >= monsters.size()) return;
  WINDOW *list = newwin(24, 80, 0, 0);
  wborder(list, '|', '|', '-', '-', '+', '+', '+', '+');
  mvwprintw(list, 0, 34, "[ Monster ]");
  mvwprintw(list, 1, 1, "x - %s", monsters[num].src->name.c_str());
  wattron(list, COLOR_PAIR(monsters[num].src->colors[0]));
  mvwaddch(list, 1, 1, monsters[num].src->symbol);
  wattroff(list, COLOR_PAIR(monsters[num].src->colors[0]));
  string desc = monsters[num].src->desc;
  short offset = 0;
  while(desc.length() > 1){
    string desc_line = desc.substr(0, desc.find_first_of("\n"));
    mvwprintw(list, 2 + ++offset, 1, desc_line.c_str());
    desc = desc.substr(desc.find_first_of("\n") + 1);
  }
  string abil_header = "Ability: ";
  string abils = "";
  if(monster_templates[num].smart()) abils.append("Smart");
  if(monster_templates[num].telepathic()){
    if(abils.length() > 0){
      abils.append(", ");
      if(abil_header.compare("Ability: ") == 0) abil_header = "Abilities: ";
    }
    abils.append("Telepathic");
  }
  if(monster_templates[num].tunnel()){
    if(abils.length() > 0){
      abils.append(", ");
      if(abil_header.compare("Ability: ") == 0) abil_header = "Abilities: ";
    }
    abils.append("Tunnel");
  }
  if(monster_templates[num].erratic()){
    if(abils.length() > 0){
      abils.append(", ");
      if(abil_header.compare("Ability: ") == 0) abil_header = "Abilities: ";
    }
    abils.append("Erratic");
  }
  if(monster_templates[num].pass()){
    if(abils.length() > 0){
      abils.append(", ");
      if(abil_header.compare("Ability: ") == 0) abil_header = "Abilities: ";
    }
    abils.append("Pass");
  }
  if(monster_templates[num].pickup()){
    if(abils.length() > 0){
      abils.append(", ");
      if(abil_header.compare("Ability: ") == 0) abil_header = "Abilities: ";
    }
    abils.append("Pickup");
  }
  if(monster_templates[num].destroy()){
    if(abils.length() > 0){
      abils.append(", ");
      if(abil_header.compare("Ability: ") == 0) abil_header = "Abilities: ";
    }
    abils.append("Destroy");
  }
  if(monster_templates[num].unique()){
    if(abils.length() > 0){
      abils.append(", ");
      if(abil_header.compare("Ability: ") == 0) abil_header = "Abilities: ";
    }
    abils.append("Unique");
  }
  if(monster_templates[num].boss()){
    if(abils.length() > 0){
      abils.append(", ");
      if(abil_header.compare("Ability: ") == 0) abil_header = "Abilities: ";
    }
    abils.append("Boss");
  }
  mvwprintw(list, offset + 4, 1, "%s%s", abil_header.c_str(), abils.c_str());
  mvwprintw(list, offset + 5, 1, "HP: %d, Damage: %s, Speed: %d", monsters[num].hp, monsters[num].src->damage.toString().c_str(), monsters[num].speed);
  char vert[6] = "north";
  char horz[5] = "east";
  int v_dist = pc.row - monsters[num].row;
  int h_dist = monsters[num].col - pc.col;
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
  mvwprintw(list, offset + 7, 1, "Distance: %d (%d %s, %d %s)", max(abs(monsters[num].row - pc.row), abs(monsters[num].col - pc.col)), v_dist, vert, h_dist, horz);
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

bool Dungeon::fight(int num, bool player_attacking){ //return true if defending dies
  if(player_attacking){
    uint16_t damage = pc.damage.roll();
    //if there's a weapon equipped, use that instead of punching
    if(!pc.equip[0].isNull()) damage = pc.equip[0].src->damage.roll();
    for(int i = 1; i < 12; i++){
      if(pc.equip[i].isNull()) continue;
      damage += pc.equip[i].src->damage.roll();
    }
    monsters[num].hp -= damage;
    string status_text = "   You hit ";
    if(!monsters[num].src->unique()) status_text.append("the ");
    status_text.append(monsters[num].src->name);
    status_text.append(" for ");
    status_text.append(to_string(damage));
    if(monsters[num].hp > 0){
      status_text.append(" damage, leaving them at ");
      status_text.append(to_string(monsters[num].hp));
      status_text.append(" HP.");
    } else {
      status_text.append(" damage, killing it!");
    }
    update_status_text(status_text.c_str());
    refresh();
    if(monsters[num].hp <= 0) return true;
    return false;
  } else {
    int damage = monsters[num].src->damage.roll();
    pc.hp -= damage;
    string status_text = "   ";
    if(monsters[num].src->unique()) status_text.append("A ");
    status_text.append(monsters[num].src->name);
    status_text.append(" hits you for ");
    status_text.append(to_string(damage));
    if(pc.hp > 0){
      status_text.append(" damage. Current HP: ");
      status_text.append(to_string(pc.hp));
    } else {
      status_text.append(" damage, killing you!");
    }
    update_status_text(status_text.c_str());
    refresh();
    if(pc.hp <= 0) return true;
    return false;
  }
}
