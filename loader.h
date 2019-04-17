int Dungeon::load_monsters(string filepath){
  ifstream fp(filepath);
  if(!fp.is_open()){
    cout << "File to load was not found at filepath: " << filepath << endl;
    return -1;
  }
  string line;
  getline(fp, line);
  if(line.compare("RLG327 MONSTER DESCRIPTION 1") != 0){
    cout << "Filetype marker unrecognized, unable to load monsters." << endl;
    return -2;
  }
  int loadreturn;
  while(fp.is_open()){
    while(line.compare("BEGIN MONSTER") != 0 && !fp.eof()) getline(fp, line);
    if(fp.eof()) break;
    loadreturn = load_monster(&fp);
    if(loadreturn < 0){
      continue;
    }
  }
  fp.close();
  return 0;
}

int Dungeon::load_monster(ifstream *fp){
  string line;
  uint16_t complete = 0;
  Monster monster;
  while(1){
    getline(*fp, line);
    if(fp->eof()) return -2; //reached end of file!?
    if(line.compare("END") == 0) break;
    string header = line.substr(0, line.find_first_of(" "));
    if(header.compare("NAME") == 0){
      if((complete & 0x1) == 0x1) return -1; //field already filled
      monster.name = line.substr(line.find_first_of(" ") + 1);
      complete = complete | 0x1;
    } else if(header.compare("DESC") == 0){
      if((complete & 0x2) == 0x2) return -1; //field already filled
      monster.desc = "";
      getline(*fp, line);
      while(line.compare(".") != 0){
	monster.desc.append(line + '\n');
	getline(*fp, line);
      }
      complete = complete | 0x2;
    } else if(header.compare("COLOR") == 0){
      if((complete & 0x4) == 0x4) return -1; //field already filled
      monster.num_colors = 0;
      while(line.find_first_of(" ") != std::string::npos){
	line = line.substr(line.find_first_of(" ") + 1);
	//add color
	if(monster.num_colors > 7) break; //this shouldn't ever happen
	string color = line.substr(0, line.find_first_of(' '));
	if(color.compare("BLUE") == 0) monster.colors[monster.num_colors] = BLUE_PAIR;
	else if(color.compare("GREEN") == 0) monster.colors[monster.num_colors] = GREEN_PAIR;
	else if(color.compare("CYAN") == 0) monster.colors[monster.num_colors] = CYAN_PAIR;
	else if(color.compare("RED") == 0) monster.colors[monster.num_colors] = RED_PAIR;
	else if(color.compare("MAGENTA") == 0) monster.colors[monster.num_colors] = MAGENTA_PAIR;
	else if(color.compare("YELLOW") == 0) monster.colors[monster.num_colors] = YELLOW_PAIR;
	else if(color.compare("WHITE") == 0) monster.colors[monster.num_colors] = WHITE_PAIR;
	else if(color.compare("BLACK") == 0) monster.colors[monster.num_colors] = BLACK_PAIR;
	else if(color.compare("GRAY") == 0 || color.compare("GREY") == 0) monster.colors[monster.num_colors] = GRAY_PAIR;
	else return -3; //found color value not specified
	monster.num_colors++;
      }
      complete = complete | 0x4;
    } else if(header.compare("SPEED") == 0){
      if((complete & 0x8) == 0x8) return -1; //field already filled
      monster.speed.parse(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x8;
    } else if(header.compare("ABIL") == 0){
      if((complete & 0x10) == 0x10) return -1; //field already filled
      monster.abilities = 0;
      while(line.find_first_of(" ") != std::string::npos){
	line = line.substr(line.find_first_of(" ") + 1);
	//add ability
	string ability = line.substr(0, line.find_first_of(' '));
	if(ability.compare("SMART") == 0) monster.abilities = monster.abilities | MON_SMART;
	else if(ability.compare("TELE") == 0) monster.abilities = monster.abilities | MON_TELEPATHIC;
	else if(ability.compare("TUNNEL") == 0) monster.abilities = monster.abilities | MON_TUNNEL;
	else if(ability.compare("ERRATIC") == 0) monster.abilities = monster.abilities | MON_ERRATIC;
	else if(ability.compare("PASS") == 0) monster.abilities = monster.abilities | MON_PASS;
	else if(ability.compare("PICKUP") == 0) monster.abilities = monster.abilities | MON_PICKUP;
	else if(ability.compare("DESTROY") == 0) monster.abilities = monster.abilities | MON_DESTROY;
	else if(ability.compare("UNIQ") == 0) monster.abilities = monster.abilities | MON_UNIQ;
	else if(ability.compare("BOSS") == 0) monster.abilities = monster.abilities | MON_BOSS;
	else return -3; //found unspecified ability
      }
      complete = complete | 0x10;
    } else if(header.compare("HP") == 0){
      if((complete & 0x20) == 0x20) return -1; //field already filled
      monster.hp.parse(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x20;
    } else if(header.compare("DAM") == 0){
      if((complete & 0x40) == 0x40) return -1; //field already filled
      monster.damage.parse(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x40;
    } else if(header.compare("SYMB") == 0){
      if((complete & 0x80) == 0x80) return -1; //field already filled
      monster.symbol = (line.substr(line.find_first_of(' ') + 1))[0];
      complete = complete | 0x80;
    } else if(header.compare("RRTY") == 0){
      if((complete & 0x100) == 0x100) return -1; //field already filled
      monster.rarity = stoi(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x100;
    }
  }
  if(complete != 0x1ff) return -2; //not all elements were initalized
  monster_templates.push_back(monster);
  return 0;
}

void Dungeon::print_monsters(){
  for(int i = 0; i < monster_templates.size(); i++){
    print_monster(i);
  }
}

void Dungeon::print_monster(int num){
  //print name/description
  cout << "Name: " << monster_templates[num].name << endl;
  cout << "Description: " << endl;
  cout << monster_templates[num].desc;
  //print colors
  if(monster_templates[num].num_colors > 1) cout << "Colors: ";
  else cout << "Color: ";
  for(int i = 0; i < monster_templates[num].num_colors; i++){
    switch(monster_templates[num].colors[i]){
    case BLUE_PAIR:
      cout << "Blue";
      break;
    case GREEN_PAIR:
      cout << "Green";
      break;
    case CYAN_PAIR:
      cout << "Cyan";
      break;
    case RED_PAIR:
      cout << "Red";
      break;
    case MAGENTA_PAIR:
      cout << "Magenta";
      break;
    case YELLOW_PAIR:
      cout << "Yellow";
      break;
    case WHITE_PAIR:
      cout << "White";
      break;
    case BLACK_PAIR:
      cout << "Black";
      break;
    case GRAY_PAIR:
      cout << "Gray";
      break;
    default:
      cout << "Chartreuse?";
    }
    if(i != monster_templates[num].num_colors - 1) cout << ", ";
  }
  cout << endl;
  //print speed
  cout << "Speed: " << monster_templates[num].speed.toString() << endl;
  //print abilities
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
  cout << abil_header << abils << endl;
  //print hp
  cout << "HP: " << monster_templates[num].hp.toString() << endl;
  //print damage
  cout << "Damage: " << monster_templates[num].damage.toString() << endl;
  //print symbol
  cout << "Symbol: " << monster_templates[num].symbol << endl;
  //print rarity
  cout << "Rarity: " << std::to_string(monster_templates[num].rarity) << endl;
  cout << endl;
}

char item_symbol(string type){
  if(type.compare("WEAPON") == 0) return '|';
  else if(type.compare("OFFHAND") == 0) return ')'; 
  else if(type.compare("RANGED") == 0) return '}';
  else if(type.compare("ARMOR") == 0) return '[';
  else if(type.compare("HELMET") == 0) return ']';
  else if(type.compare("CLOAK") == 0) return '(';
  else if(type.compare("GLOVES") == 0) return '{';
  else if(type.compare("BOOTS") == 0) return '\\';
  else if(type.compare("RING") == 0) return '=';
  else if(type.compare("AMULET") == 0) return '"';
  else if(type.compare("LIGHT") == 0) return '_';
  else if(type.compare("SCROLL") == 0) return '~';
  else if(type.compare("BOOK") == 0) return '?';
  else if(type.compare("FLASK") == 0) return '!';
  else if(type.compare("GOLD") == 0) return '$';
  else if(type.compare("AMMUNITION") == 0) return '/';
  else if(type.compare("FOOD") == 0) return ',';
  else if(type.compare("WAND") == 0) return '-';
  else if(type.compare("CONTAINER") == 0) return '%';
  else return '*';
}

int Dungeon::load_items(string filepath){
  ifstream fp(filepath);
  if(!fp.is_open()){
    cout << "File to load was not found at filepath: " << filepath << endl;
    return -1;
  }
  string line;
  getline(fp, line);
  if(line.compare("RLG327 OBJECT DESCRIPTION 1") != 0){
    cout << "Filetype marker unrecognized, unable to load items." << endl;
    return -2;
  }
  int loadreturn;
  while(fp.is_open()){
    while(line.compare("BEGIN OBJECT") != 0 && !fp.eof()) getline(fp, line);
    if(fp.eof()) break;
    loadreturn = load_item(&fp); //nothing is done with loadreturn right now, since if an item fails to load it simply isn't added to the vector
  }
  fp.close();
  return 0;
}

int Dungeon::load_item(ifstream *fp){
  string line;
  uint16_t complete = 0;
  Item item;
  while(1){
    getline(*fp, line);
    if(fp->eof()) return -2; //reached end of file!?
    if(line.compare("END") == 0) break;
    string header = line.substr(0, line.find_first_of(" "));
    if(header.compare("NAME") == 0){
      if((complete & 0x1) == 0x1) return -1; //field already filled
      item.name = line.substr(line.find_first_of(" ") + 1);
      complete = complete | 0x1;
    } else if(header.compare("DESC") == 0){
      if((complete & 0x2) == 0x2) return -1; //field already filled
      item.desc = "";
      getline(*fp, line);
      while(line.compare(".") != 0){
	item.desc.append(line + '\n');
	getline(*fp, line);
      }
      complete = complete | 0x2;
    } else if(header.compare("TYPE") == 0){
      if((complete & 0x4) == 0x4) return -1; //field already filled
      item.type = line.substr(line.find_first_of(' ') + 1);
      item.symbol = item_symbol(item.type);
      if(item.symbol == '*') return -4; //item type unrecognized
      complete = complete | 0x4;
    } else if(header.compare("COLOR") == 0){
      if((complete & 0x8) == 0x8) return -1; //field already filled
      string color = line.substr(line.find_first_of(' ') + 1);
      if(color.compare("BLUE") == 0) item.color = BLUE_PAIR;
      else if(color.compare("GREEN") == 0) item.color = GREEN_PAIR;
      else if(color.compare("CYAN") == 0) item.color = CYAN_PAIR;
      else if(color.compare("RED") == 0) item.color = RED_PAIR;
      else if(color.compare("MAGENTA") == 0) item.color = MAGENTA_PAIR;
      else if(color.compare("YELLOW") == 0) item.color = YELLOW_PAIR;
      else if(color.compare("WHITE") == 0) item.color = WHITE_PAIR;
      else if(color.compare("BLACK") == 0) item.color = BLACK_PAIR;
      else if(color.compare("GRAY") == 0 || color.compare("GREY") == 0) item.color = GRAY_PAIR;
      else return -3; //found color value not specified
      complete = complete | 0x8;
    } else if(header.compare("HIT") == 0){
      if((complete & 0x10) == 0x10) return -1; //field already filled
      item.hit.parse(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x10;
    } else if(header.compare("DAM") == 0){
      if((complete & 0x20) == 0x20) return -1; //field already filled
      item.damage.parse(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x20;
    } else if(header.compare("DODGE") == 0){
      if((complete & 0x40) == 0x40) return -1; //field already filled
      item.dodge.parse(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x40;
    } else if(header.compare("DEF") == 0){
      if((complete & 0x80) == 0x80) return -1; //field already filled
      item.def.parse(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x80;
    } else if(header.compare("WEIGHT") == 0){
      if((complete & 0x100) == 0x100) return -1; //field already filled
      item.weight.parse(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x100;
    } else if(header.compare("SPEED") == 0){
      if((complete & 0x200) == 0x200) return -1; //field already filled
      item.speed.parse(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x200;
    } else if(header.compare("ATTR") == 0){
      if((complete & 0x400) == 0x400) return -1; //field already filled
      item.attr.parse(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x400;
    } else if(header.compare("VAL") == 0){
      if((complete & 0x800) == 0x800) return -1; //field already filled
      item.value.parse(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x800;
    } else if(header.compare("ART") == 0){
      if((complete & 0x1000) == 0x1000) return -1; //field already filled
      string art = line.substr(line.find_first_of(' ') + 1);
      if(art.compare("TRUE") == 0) item.art = 1;
      else if(art.compare("FALSE") == 0) item.art = 0;
      else return -4; //unrecognized art value
      complete = complete | 0x1000; 
    } else if(header.compare("RRTY") == 0){
      if((complete & 0x2000) == 0x2000) return -1; //field already filled
      item.rarity = stoi(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x2000;
    }
  }
  if(complete != 0x3fff) return -2; //not all elements were initalized
  item_templates.push_back(item);
  return 0;
}

void Dungeon::print_items(){
  for(int i = 0; i < item_templates.size(); i++){
    print_item(i);
  }
}

void Dungeon::print_item(int num){
  //print name/description
  cout << "Name: " << item_templates[num].name << endl;
  cout << "Description: " << endl;
  cout << item_templates[num].desc;
  //print type
  cout << "Type: " << item_templates[num].type << endl;
  //print colors
  cout << "Color: ";
  switch(item_templates[num].color){
  case BLUE_PAIR:
    cout << "Blue";
    break;
  case GREEN_PAIR:
    cout << "Green";
    break;
  case CYAN_PAIR:
    cout << "Cyan";
    break;
  case RED_PAIR:
    cout << "Red";
    break;
  case MAGENTA_PAIR:
    cout << "Magenta";
    break;
  case YELLOW_PAIR:
    cout << "Yellow";
    break;
  case WHITE_PAIR:
    cout << "White";
    break;
  case BLACK_PAIR:
    cout << "Black";
    break;
  case GRAY_PAIR:
    cout << "Gray";
    break;
  default:
    cout << "Chartreuse?"; //this shouldn't happen
  }
  cout << endl;
  //print hit
  cout << "Hit Bonus: " << item_templates[num].hit.toString() << endl;
  //print damage
  cout << "Damage Bonus: " << item_templates[num].damage.toString() << endl;
  //print dodge
  cout << "Dodge Bonus: " << item_templates[num].dodge.toString() << endl;
  //print def
  cout << "Defensive Bonus: " << item_templates[num].def.toString() << endl;
  //print weight
  cout << "Weight: " << item_templates[num].weight.toString() << endl;
  //print speed
  cout << "Speed Bonus: " << item_templates[num].speed.toString() << endl;
  //print attr
  cout << "Special Attribute: " << item_templates[num].attr.toString() << endl;
  //print val
  cout << "Value: " << item_templates[num].value.toString() << endl;
  //print art (not the felony)
  cout << "Artifact: ";
  if(item_templates[num].art) cout << "YES" << endl;
  else cout << "NO" << endl;
  cout << "Rarity: " << std::to_string(item_templates[num].rarity) << endl;
  cout << endl;
}

