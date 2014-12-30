#ifndef CONFIG_STRUCT_IMPL_HPP
#define CONFIG_STRUCT_IMPL_HPP

/*
 * Convert string to color. If string is bad, terminate program.
 */
template<class mcolor_t>
mcolor_t main_config<mcolor_t>::name_to_mcolor(std::string const & temp) const {
  mcolor_t answer; 
  if (temp[0] == '{' && temp[temp.length() - 1] == '}') { 
    std::string current = "";
    for (size_t i = 1; i < temp.length(); ++i) { 
      if (temp[i] == ',' || temp[i] == '}') { 
        if (genome_number.find(current) != genome_number.end()) {  
          answer.insert(genome_number.find(current)->second);
        }   
        current = "";
      } else if (std::isalpha(temp[i]) || std::isdigit(temp[i])) { 
        current += temp[i];
      } else { 
        ERROR("Bad format target " << temp);
        answer.clear(); 
        break;
      }  
    } 
  } else {
    ERROR("Bad format target " << temp);
  }
  return answer;
}

/*
 * Convert multicolor to string with genomes name.
 */
template<class mcolor_t>
std::string main_config<mcolor_t>::mcolor_to_name(mcolor_t const & color) const {
  if (mcolor_name.find(color) != mcolor_name.end()) { 
    return mcolor_name.find(color)->second;
  } else { 
    std::string answer = "";
  
    std::for_each(color.cbegin(), color.cend(), [&] (std::pair<size_t, size_t> const & col) -> void {
      std::string const & sym = priority_name[col.first]; 
      for(size_t i = 0; i < col.second; ++i) { 
        answer += (sym + ",");
      }
    });  
 
    answer += '}';
    return answer; 
  } 
}

/*
 * Read configure file to hashmap and run parse function.
 */
template<class mcolor_t>
void load(main_config<mcolor_t>& cfg, std::string const & filename) { 
  TRACE("Start load cfg file in MGRA1 format")
  std::unordered_map<std::string, std::vector<std::string> > problem_config;
	
  std::ifstream input(filename);
	
  if (!input.good()) {
    ERROR("Cannot open " << filename);
    exit(1); 
  }
	
  std::string section;
  while(input.good()) {
    std::string line;
    std::getline(input, line);
    boost::trim(line);

    if (line[0] == '[' && line[line.size() - 1] == ']') {
      section = line;
    } else if (!line.empty() && (line[0] != '#')) {
      problem_config[section].push_back(line);
    }
  } 
  input.close();

  cfg.parse(problem_config);
} 

/*
 * Different fucntion, which parse our input. 
 */
template<class mcolor_t>
void main_config<mcolor_t>::parse(std::unordered_map<std::string, std::vector<std::string> > const & input) {  
  if (input.find("[Genomes]") != input.cend()) {  
    TRACE("Parse genomes section")
    parse_genomes(input.find("[Genomes]")->second);
  } else { 
    ERROR("Cann't find genomes section");
    exit(1);
  }

  if (input.find("[Trees]") != input.cend()) {  
    TRACE("Parse trees section")
    parse_trees(input.find("[Trees]")->second);
  } else { 
    ERROR("Cann't find trees section");
    exit(1);
  }

  if (input.find("[Algorithm]") != input.cend()) {  
    TRACE("Parse algorithm section")
    parse_algorithm(input.find("[Algorithm]")->second);
  } else { 
    ERROR("Cann't find algorithm section");
    exit(1);
  }

  if (input.find("[Target]") != input.cend()) {  
    parse_target(input.find("[Target]")->second);
  }

  if (input.find("[Completion]") != input.cend()) {  
    parse_completion(input.find("[Completion]")->second);
  }

  init_basic_rgb_colors();
}

template<class mcolor_t>
void main_config<mcolor_t>::parse_genomes(std::vector<std::string> const & genomes) { 
  priority_name.resize(genomes.size());
      
  for (size_t k = 0; k < genomes.size(); ++k) {
    std::istringstream is(genomes[k]);
    std::string name;
    is >> name;

    if (genome_number.count(name) > 0) { 
      ERROR("Genome identificator " << name << " is not unique!")
      exit(1);
    } 

    priority_name[k] = name;  
    genome_number.insert(std::make_pair(name, k));
  
    while(!is.eof()) {
      std::string alias;
      is >> alias;

      if (genome_number.count(alias) > 0) {
        ERROR("Duplicate alias " << alias)
        exit(1);
      }

      genome_number.insert(std::make_pair(alias, k));
    }
  } 
}

template<class mcolor_t>
void main_config<mcolor_t>::parse_trees(std::vector<std::string> const & input) { 
  for (auto const & str : input) {
    phylotrees.push_back(phylogeny_tree_t(str, genome_number, priority_name)); 
    auto const & locals = phylotrees.crbegin()->get_name_for_colors();
    mcolor_name.insert(locals.cbegin(), locals.cend());
  }
}

template<class mcolor_t>
void main_config<mcolor_t>::parse_algorithm(std::vector<std::string> const & input) { 
  for(auto const & str : input) {
    std::istringstream is(str);
    std::string name;
    is >> name;

    if (name == "stages") { 
      is >> stages;
    } else if (name == "rounds") {
      is >> rounds;
      if (rounds > 3) {
        ERROR("Large number of rounds") 
        exit(1);
      } 
    } else if (name == "bruteforce") {
      is >> size_component_in_bruteforce;
      is_bruteforce = (size_component_in_bruteforce > 0); 
    } else if (name == "recostructed_tree") {
      is_reconstructed_trees = true; 
    } else { 
      ERROR("Unknown option " << name)
      exit(1);
    }
  }

  is_linearization_algo = true;
}

template<class mcolor_t>
void main_config<mcolor_t>::parse_target(std::vector<std::string> const & input) { 
  std::istringstream is(*input.cbegin());
  std::string temp; 
  is >> temp; 
  std::remove_if(temp.begin(), temp.end(), (int(*)(int)) isspace); //FIXME NOT WORKED
  is_target_build = true;
  target_mcolor = name_to_mcolor(temp);
}

template<class mcolor_t>
void main_config<mcolor_t>::parse_completion(std::vector<std::string> const & input) { 
  for(auto const & event: input) {
    std::vector<std::string> mc(5);
    std::istringstream is(event);
    is >> mc[0] >> mc[1] >> mc[2] >> mc[3] >> mc[4];
    std::remove_if(mc[4].begin(), mc[4].end(), (int(*)(int)) isspace); //FIXME NOT WORKED   
    mcolor_t color = name_to_mcolor(mc[4]);
    completion.push_back(twobreak_t(mc[0], mc[1], mc[2], mc[3], color));
  }
}

template<class mcolor_t>
void main_config<mcolor_t>::init_basic_rgb_colors() { 

  if (priority_name.size() < 10) {
    colorscheme = "set19";
    for(size_t i = 1; i < priority_name.size() + 1; ++i) {
      RGBcolors.push_back(std::to_string(i)); 
    } 
    RGBcoeff = 1; 
  } else if (priority_name.size() < 13) {
    colorscheme = "";
    size_t const number_colors = 13;
    std::string cols[number_colors] = { 
      "red3", "green3", "blue", "purple", "black", "orange", "greenyellow", "pink",
      "cyan", "magenta",  "yellow3", "grey70", "darkorange4"
    };    
  
    for(size_t i = 0; i < number_colors; ++i) { 
      RGBcolors.push_back(cols[i]);       
    }   
  
    RGBcoeff = 1; 
  } else {  
    colorscheme = "";
    size_t const number_colors = 136;
    std::string cols[number_colors] = {
      "#C91F16","#CA2316","#CC2A13","#D03815","#CC3615","#D23D16","#D34810","#D44B0D",
      "#D9580E","#D95B0F","#DA5F0E","#DB640D","#DD680B","#DC6E0D","#E37509","#E7860B",
      "#E68601","#ED9E0A","#F1A60A","#F1AD06","#EEAD00","#F7BD00","#F5B600","#F9C60F",
      "#FCCF03","#FBD50A","#FCE503","#F6E60A","#EDE400","#E6E209","#DFDC09","#D8DB09",
      "#CED500","#C5D300","#BED20F","#B5CC0A","#B0C50F","#A5C50F","#9FC40D","#98C100",
      "#8FBB0C","#92C00E","#7FB513","#80B812","#77B312","#71B30E","#6FB20F","#69AC12",
      "#69B011","#55A41C","#5AAA1D","#4EA41D","#47A41E","#3A9B20","#349B26","#339C1E",
      "#2D9C1F","#2B9B22","#209426","#1B9324","#189425","#039331","#008C33","#098D3A",
      "#088343","#0C8C4B","#00844A","#048D5C","#0E8C62","#088D6C","#008C7C","#089394",
      "#05949D","#0B8D94","#0994A6","#009DBE","#0894B6","#0D9BC4","#009DCF","#159BD4",
      "#1F9AD7","#148DC6","#1D93D1","#147CB5","#0D84C4","#0C7BBB","#0D73B3","#106CAC",
      "#0363A3","#135A9E","#085293","#0A5397","#094A91","#0D4284","#06438A","#0D3981",
      "#113279","#123179","#152C74","#182A71","#182369","#1B1A5F","#181C67","#1D1A62",
      "#1D1259","#1E1059","#230E59","#300D5A","#310C5A","#3A0B59","#4B0A5B","#51095A",
      "#5B0B5A","#730861","#6B015A","#7C0362","#820060","#8B0A62","#940B63","#A40563",
      "#AE0964","#C20F68","#BD0062","#C40063","#C50059","#C6004B","#C70542","#C60A39",
      "#C2224A","#C50A2A","#C50B21","#C5121B","#C4121A","#C4232B","#C3332B","#C2452A"
    };
  
    for(size_t i = 0; i < number_colors; ++i) { 
      RGBcolors.push_back("\"" + cols[i] + "\"");       
    }   

    RGBcoeff = (number_colors - 1) / (get_count_genomes() - 1);
  } 
} 


#endif