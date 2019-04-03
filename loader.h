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
      num_monsters--; //delete the failed monster
      monsters = (Monster*)realloc(monsters, num_monsters * sizeof(Monster));
    }
  }
  fp.close();
  return 0;
}

int Dungeon::load_monster(ifstream *fp){
  string line;
  uint16_t complete = 0;
  int num = num_monsters++;
  cout << num << endl;
  monsters = (Monster*)realloc(monsters, num_monsters * sizeof(Monster));
  while(1){
    getline(*fp, line);
    cout << "Read line: " << line << endl;
    if(fp->eof()) return -2; //reached end of file!?
    if(line.compare("END") == 0) break;
    string header = line.substr(0, line.find_first_of(" "));
    cout << "Header: " << header << endl;
    if(header.compare("NAME") == 0){
      if((complete & 0x1) == 0x1) return -1; //field already filled
      monsters[num].name = line.substr(line.find_first_of(" ") + 1);
      complete = complete | 0x1;
    } else if(header.compare("DESC") == 0){
      if((complete & 0x2) == 0x2) return -1; //field already filled
      //monsters[num].desc = "";
      getline(*fp, line);
      while(line.compare(".") != 0){
	monsters[num].desc.append(line + " ");
	getline(*fp, line);
      }
      complete = complete | 0x2;
      cout << monsters[num].desc << endl;
    } else if(header.compare("COLOR") == 0){
      if((complete & 0x4) == 0x4) return -1; //field already filled
      line = line.substr(line.find_first_of(" ") + 1);
      monsters[num].num_colors = 0;
      while(line.find_first_not_of(" ") != std::string::npos){
	//add color
	if(monsters[num].num_colors > 7) break; //this shouldn't ever happen
	string color = line.substr(0, line.find_first_of(' '));
	if(color.compare("BLUE") == 0) monsters[num].colors[monsters[num].num_colors] = BLUE_PAIR;
	else if(color.compare("GREEN") == 0) monsters[num].colors[monsters[num].num_colors] = GREEN_PAIR;
	else if(color.compare("CYAN") == 0) monsters[num].colors[monsters[num].num_colors] = CYAN_PAIR;
	else if(color.compare("RED") == 0) monsters[num].colors[monsters[num].num_colors] = RED_PAIR;
	else if(color.compare("MAGENTA") == 0) monsters[num].colors[monsters[num].num_colors] = MAGENTA_PAIR;
	else if(color.compare("YELLOW") == 0) monsters[num].colors[monsters[num].num_colors] = YELLOW_PAIR;
	else if(color.compare("WHITE") == 0) monsters[num].colors[monsters[num].num_colors] = WHITE_PAIR;
	else if(color.compare("GRAY") == 0 || color.compare("GREY") == 0) monsters[num].colors[monsters[num].num_colors] = GRAY_PAIR;
	else return -3; //found color value not specified
	monsters[num].num_colors++;
	line = line.substr(line.find_first_of(' ') + 1);
      }
      complete = complete | 0x4;
    } else if(header.compare("SPEED") == 0){
      if((complete & 0x8) == 0x8) return -1; //field already filled
      monsters[num].speed.parse(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x8;
    } else if(header.compare("ABIL") == 0){
      if((complete & 0x10) == 0x10) return -1; //field already filled
      //todo
      complete = complete | 0x10;
    } else if(header.compare("HP") == 0){
      if((complete & 0x20) == 0x20) return -1; //field already filled
      monsters[num].hp.parse(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x20;
    } else if(header.compare("DAM") == 0){
      if((complete & 0x40) == 0x40) return -1; //field already filled
      monsters[num].damage.parse(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x40;
    } else if(header.compare("SYMB") == 0){
      if((complete & 0x80) == 0x80) return -1; //field already filled
      monsters[num].symbol = (line.substr(line.find_first_of(' ') + 1))[0];
      complete = complete | 0x80;
    } else if(header.compare("RRTY") == 0){
      if((complete & 0x100) == 0x100) return -1; //field already filled
      monsters[num].rarity = stoi(line.substr(line.find_first_of(' ') + 1));
      complete = complete | 0x100;
    }
  }
  if(complete != 0x1ff) return -2; //not all elements were initalized
  return 0;
}
