int Dungeon::load_monsters(string filepath){
  ifstream fp(filepath);
  if(!fp.is_open()){
    //printf("File to load was not found at filepath: %s\n", filepath);
    cout << "File to load was not found at filepath: " << filepath << endl;
    return -1;
  }
  string line;
  getline(fp, line);
  if(line.compare("RLG327 MONSTER DESCRIPTION 1") != 0){
    cout << "Filetype marker unrecognized, unable to load monsters." << endl;
    return -2;
  }
  num_monsters = 0;
  int loadreturn;
  while(fp.is_open()){
    while(line.compare("BEGIN MONSTER") != 0 && !fp.eof()) getline(fp, line);
    if(fp.eof()) break;
    loadreturn = load_monster(&fp);
    if(loadreturn < 0){
      //num_monsters--; //delete the failed monster
      // monsters = (Monster*)realloc(monsters, num_monsters * sizeof(Monster));
      continue;
    }
    num_monsters++;
  }
  fp.close();
  return 0;
}

int Dungeon::load_monster(ifstream *fp){
  string line;
  uint16_t complete = 0;
  Monster monster;
  //monsters = (Monster*)realloc(monsters, num_monsters * sizeof(Monster));
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
	else if(color.compare("BLACK") == 0) monster.colors[monster.num_colors] = GRAY_PAIR; //use gray for now
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
  monsters.push_back(monster);
  return 0;
}

void Dungeon::print_monsters(){
  for(int i = 0; i < num_monsters; i++){
    print_monster(i);
  }
}

void Dungeon::print_monster(int num){
  //print name/description
  cout << "Name: " << monsters[num].name << endl;
  cout << "Description: " << endl;
  cout << monsters[num].desc;
  //print colors
  if(monsters[num].num_colors > 1) cout << "Colors: ";
  else cout << "Color: ";
  for(int i = 0; i < monsters[num].num_colors; i++){
    switch(monsters[num].colors[i]){
    case BLUE_PAIR:
      cout << "Blue";
      break;
    case GREEN_PAIR:
      cout << "Green";
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
    case GRAY_PAIR:
      //cout << "Gray";
      cout << "Black";
      break;
    default:
      cout << "Chartreuse?";
    }
    if(i != monsters[num].num_colors - 1) cout << ", ";
  }
  cout << endl;
  //print speed
  cout << "Speed: " << monsters[num].speed.toString() << endl;
  //print abilities
  string abil_header = "Ability: ";
  string abils = "";
  if(monsters[num].smart()) abils.append("Smart");
  if(monsters[num].telepathic()){
    if(abils.length() > 0){
      abils.append(", ");
      if(abil_header.compare("Ability: ") == 0) abil_header = "Abilities: ";
    }
    abils.append("Telepathic");
  }
  if(monsters[num].tunnel()){
    if(abils.length() > 0){
      abils.append(", ");
      if(abil_header.compare("Ability: ") == 0) abil_header = "Abilities: ";
    }
    abils.append("Tunnel");
  }
  if(monsters[num].erratic()){
    if(abils.length() > 0){
      abils.append(", ");
      if(abil_header.compare("Ability: ") == 0) abil_header = "Abilities: ";
    }
    abils.append("Erratic");
  }
  if(monsters[num].pass()){
    if(abils.length() > 0){
      abils.append(", ");
      if(abil_header.compare("Ability: ") == 0) abil_header = "Abilities: ";
    }
    abils.append("Pass");
  }
  if(monsters[num].pickup()){
    if(abils.length() > 0){
      abils.append(", ");
      if(abil_header.compare("Ability: ") == 0) abil_header = "Abilities: ";
    }
    abils.append("Pickup");
  }
  if(monsters[num].destroy()){
    if(abils.length() > 0){
      abils.append(", ");
      if(abil_header.compare("Ability: ") == 0) abil_header = "Abilities: ";
    }
    abils.append("Destroy");
  }
  if(monsters[num].unique()){
    if(abils.length() > 0){
      abils.append(", ");
      if(abil_header.compare("Ability: ") == 0) abil_header = "Abilities: ";
    }
    abils.append("Unique");
  }
  if(monsters[num].boss()){
    if(abils.length() > 0){
      abils.append(", ");
      if(abil_header.compare("Ability: ") == 0) abil_header = "Abilities: ";
    }
    abils.append("Boss");
  }
  cout << abil_header << abils << endl;
  //print hp
  cout << "HP: " << monsters[num].hp.toString() << endl;
  //print damage
  cout << "Damage: " << monsters[num].damage.toString() << endl;
  //print symbol
  cout << "Symbol: " << monsters[num].symbol << endl;
  //print rarity
  cout << "Rarity: " << std::to_string(monsters[num].rarity) << endl;
  cout << endl;
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
      else if(color.compare("BLACK") == 0) item.color = GRAY_PAIR; //use gray for now instead of black
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
      item.rarity = stoi(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x400;
    } else if(header.compare("VAL") == 0){
      if((complete & 0x800) == 0x800) return -1; //field already filled
      item.value.parse(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x800;
    } else if(header.compare("ART") == 0){
      if((complete & 0x1000) == 0x1000) return -1; //field already filled
      string art = line.substr(line.find_first_of(' ') + 1);
      if(art.compare("TRUE") == 0) item.art = true;
      else if(art.compare("FALSE") == 0) item.art = false;
      else return -4; //unrecognized art value
      complete = complete | 0x1000; 
    } else if(header.compare("RRTY") == 0){
      if((complete & 0x2000) == 0x2000) return -1; //field already filled
      item.rarity = stoi(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x2000;
    }
  }
  if(complete != 0x3fff) return -2; //not all elements were initalized
  items.push_back(item);
  return 0;
}

void Dungeon::print_items(){
  for(int i = 0; i < items.size(); i++){
    print_item(i);
  }
}

void Dungeon::print_item(int num){
  //print name/description
  cout << "Name: " << items[num].name << endl;
  cout << "Description: " << endl;
  cout << items[num].desc;
  //print type
  cout << items[num].type << endl;
  //print colors
  cout << "Color: ";
  switch(items[num].color){
  case BLUE_PAIR:
    cout << "Blue";
    break;
  case GREEN_PAIR:
    cout << "Green";
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
  case GRAY_PAIR:
    //cout << "Gray";
    cout << "Black";
    break;
  default:
    cout << "Chartreuse?"; //this shouldn't happen
  }
  cout << endl;
  //print hit
  cout << "Hit Bonus: " << items[num].hit.toString() << endl;
  //print damage
  cout << "Damage Bonus: " << items[num].damage.toString() << endl;
  //print dodge
  cout << "Dodge Bonus: " << items[num].dodge.toString() << endl;
  //print def
  cout << "Defensive Bonus: " << items[num].def.toString() << endl;
  //print weight
  cout << "Weight: " << items[num].weight.toString() << endl;
  //print speed
  cout << "Speed Bonus: " << items[num].speed.toString() << endl;
  //print attr
  cout << "Special Attribute: " << std::to_string(items[num].attr) << endl;
  //print val
  cout << "Value: " << items[num].value.toString() << endl;
  //print art (not the felony)
  cout << "Artifact: ";
  if(items[num].art) cout << "YES" << endl;
  else cout << "NO" << endl;
  cout << "Rarity: " << std::to_string(monsters[num].rarity) << endl;
  cout << endl;
}
