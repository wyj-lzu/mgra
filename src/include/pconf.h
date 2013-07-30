/* 
** Module: MGRA Configurations support
** Version: 1.1
**
** This file is part of the 
** Multiple Genome Rearrangements and Ancestors (MGRA) 
** reconstruction software. 
** 
** Copyright (C) 2008,12 by Max Alekseyev <maxal@cse.sc.edu> 
**. 
** This program is free software; you can redistribute it and/or 
** modify it under the terms of the GNU General Public License 
** as published by the Free Software Foundation; either version 2 
** of the License, or (at your option) any later version. 
**. 
** You should have received a copy of the GNU General Public License 
** along with this program; if not, see http://www.gnu.org/licenses/gpl.html 
*/

#ifndef PCONF_H_
#define PCONF_H_

#include <unordered_map>
#include <vector>
#include <list>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <cassert>


template <typename T>
__attribute__((always_inline)) inline std::string toString(T val) { 
	std::ostringstream oss; 
	oss << val; 
	return oss.str();
}  

__attribute__((always_inline)) inline bool check_symbol(char c) { 
   if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) { 
     return true;
   } else { 
     return false;
   } 
} 


/*This structures containes information from *.cfg file */
template<class mcolor_t>
struct ProblemInstance {
	ProblemInstance(const std::unordered_map<std::string, std::vector<std::string> >& input); 

	mcolor_t name_to_mcolor(const std::string& temp) const;
	std::string mcolor_to_name(const mcolor_t& temp) const;

	inline bool is_genome_name(const std::string& i) const {
		return (genome_number.find(i) != genome_number.end());
	} 

	inline size_t get_genome_number(const std::string& str) const {
		assert (genome_number.find(str) != genome_number.end()); 
		return genome_number.find(str)->second;
	} 

	inline size_t get_count_genomes() const {
		return priority_name.size();
	} 

	inline std::string get_priority_name(size_t i) const {
		return priority_name[i];
	} 
	
	inline std::string get_blk_format() const { 
		return block_format;
	} 

	inline std::string get_blk_file() const { 
		return block_file;		
	} 

	inline std::string get_graphname() const { 
		return graphfname;		
	} 

	inline std::string get_colorscheme() const { 
		return colorscheme;		
	} 

	inline std::vector<std::string>/*std::vector<BinaryTree<std::string> >*/ get_trees() const { 
		return trees;
	} 

	inline size_t get_stages() const { 
		return stages;
	} 

	inline mcolor_t get_target() const { 
		return target;		
	} 

	inline std::list<std::vector<std::string> > get_completion() const { //del and write convert two break
		return completion;
	} 

	inline size_t RGB_color_size() { 
		return RGBcolors.size();
	} 

	inline std::string get_RGBcolor(int index) const { 
		return RGBcolors[index];
	} 

	inline int get_RGBcoeff() const { 
		return RGBcoeff;
	} 
private: 
	void init_basic_rgb_colors(bool flag = true); 
private:
	std::vector<std::string> priority_name;  
	std::unordered_map<std::string, size_t> genome_number;
	std::unordered_map<size_t, std::string> number_genome;  

	std::string block_format; 	
	std::string block_file;

	std::string graphfname;
	std::string colorscheme;

	//std::vector<BinaryTree<std::string> > trees;
	std::vector<std::string> trees;

	size_t stages;

	mcolor_t target; 		
	
	std::list<std::vector<std::string> > completion;

	std::vector<std::string> RGBcolors;
	int RGBcoeff; 
};

template<class mcolor_t>
ProblemInstance<mcolor_t>::ProblemInstance(const std::unordered_map<std::string, std::vector<std::string> >& input) { 
	for(auto ip = input.cbegin(); ip != input.cend(); ++ip) {
		if (ip->first == "[Genomes]") {  
			this->priority_name.resize(ip->second.size());
	    
			size_t k = 0;	    
			for(auto js = ip->second.cbegin(); js != ip->second.cend(); ++js, ++k) {
				std::istringstream is(*js);
				std::string name;
				is >> name;

				if (genome_number.count(name) > 0) { 
					std::cerr << "ERROR: Genome identificator " << name << " is not unique!" << std::endl;
					exit(1);
				} 

				priority_name[k] = name;	
				genome_number.insert(std::make_pair(name, k));
				number_genome.insert(std::make_pair(k, name));

				while(!is.eof()) {
					std::string alias;
					is >> alias;

					if (this->genome_number.count(alias) > 0) {
						std::cerr << "ERROR: Duplicate alias " << alias << std::endl;
						exit(1);
					}

					number_genome.insert(std::make_pair(k, alias));
					genome_number.insert(std::make_pair(alias, k));
				}
			}
		} else if (ip->first == "[Blocks]") {
			for (auto js = ip->second.cbegin(); js != ip->second.cend(); ++js) {
				std::istringstream is(*js);
				std::string name;
				is >> name;
				if(name == "format") {  
					is >> this->block_format;
				} else if(name == "file") {  
					is >> this->block_file;
				} else {
					std::cerr << "Unknown option " << name << std::endl;
					exit(1);
				}			} 
		} else if (ip->first == "[Trees]") {
			trees = ip->second;
			//for (auto it = ip->second.cbegin(); it != ip->second.cend(); ++it) { 
			//	this->trees.push_back(BinaryTree<std::string>(*it));	
			//} 
		} else if (ip->first == "[Graphs]") {
			for(auto js = ip->second.cbegin(); js != ip->second.cend(); ++js) {
				std::istringstream is(*js);
				std::string name;
				is >> name;

				if (name == "filename") { 
					is >> graphfname;
				} else if (name == "colorscheme") { 
					is >> colorscheme; 
				} else {
					std::cerr << "Unknown option " << name << std::endl;
					exit(1);
				}
			}
		} else if (ip->first == "[Algorithm]") {
			for(auto js = ip->second.cbegin(); js != ip->second.cend(); ++js) {
				std::istringstream is(*js);
				std::string name;
				is >> name;

				if (name == "stages") { 
					is >> stages;
				} else {
					std::cerr << "Unknown option " << name << std::endl;
					exit(1);
				}
			}
		} else if (ip->first == "[Target]" || ip->first == "[Completion]") {
			continue;  
		} else { 
			std::cerr << "Unknown section " << ip->first << std::endl;
			exit(1);
		}
	} 

	for(auto ip = input.cbegin(); ip != input.cend(); ++ip) {
		if (ip->first == "[Target]") { 
			std::istringstream is(*ip->second.cbegin());
			std::string temp; 
			is >> temp; 
			std::remove_if(temp.begin(), temp.end(), (int(*)(int)) isspace);
			target = name_to_mcolor(temp);
		} else if (ip->first == "[Completion]") {
			for(auto js = ip->second.cbegin(); js != ip->second.cend(); ++js) {
				std::vector<std::string> mc(5);
				std::istringstream is(*js);
				is >> mc[0] >> mc[1] >> mc[2] >> mc[3] >> mc[4];
				completion.push_back(mc);
			}
		}
	}
	init_basic_rgb_colors(colorscheme.empty());
} 

template<class mcolor_t>
void ProblemInstance<mcolor_t>::init_basic_rgb_colors(bool flag) { 
	const size_t number_colors = 136;

	if (flag) { 	
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
	} else { 
		for(size_t i = 0; i < number_colors; ++i) 
			RGBcolors.push_back(toString(i + 1));
		RGBcoeff = 1;
	}
} 


template<class mcolor_t>
mcolor_t ProblemInstance<mcolor_t>::name_to_mcolor(const std::string& temp) const {
	mcolor_t answer; 
	if (temp[0] == '{' && temp[temp.length() - 1] == '}') { 
		std::string current = "";
		for (size_t i = 1; i < temp.length(); ++i) { 
			if (temp[i] == ',' || temp[i] == '}') { 
				if (genome_number.find(current) != genome_number.end()) {  
					answer.insert(genome_number.find(current)->second);
				}  	
				current = "";
			} else if (check_symbol(temp[i])) { 
				current += temp[i];
			} else { 
				std::cerr << "Bad format target " << temp << std::endl;
				exit(1);
			}  
		} 
	} else {
		std::cerr << "Bad format target " << temp << std::endl;
		exit(1);
	}
	return answer;
}

template<class mcolor_t>
std::string ProblemInstance<mcolor_t>::mcolor_to_name(const mcolor_t& color) const {
	std::string answer = "{";
	if (color.empty()) {
		answer += "}";
	} 

	for (auto it = color.cbegin(); it != color.cend(); ++it) {
		const std::string& sym = priority_name[it->first]; 
		for(size_t i = 0; i < it->second; ++i) { 
			answer += (sym + ",");
		} 
	}

	answer[answer.size() - 1] = '}';
	return answer; 
}
#endif
