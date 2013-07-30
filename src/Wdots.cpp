#include "Wdots.h"

void writer::Wdots::save_dot(const mbgraph_with_history<Mcolor>& graph, const ProblemInstance<Mcolor>& cfg, size_t stage) { 
  std::string dotname = cfg.get_graphname() + toString(stage) + ".dot";
  std::ofstream dot(dotname.c_str());

  dot << "graph {" << std::endl;
  if (!cfg.get_colorscheme().empty()) { 
    dot << "edge [colorscheme=" << cfg.get_colorscheme() << "];" << std::endl;
  } 

  int infv = 0;
  size_t ncls = 0;
  std::unordered_set<vertex_t> mark; // vertex set
  for(auto is = graph.begin_vertices(); is != graph.end_vertices(); ++is) {
    const std::string& x = *is;

    Mularcs<Mcolor> Mx = graph.get_adjacent_multiedges(x);

    if (Mx.size() == 1 && Mx.cbegin()->second == graph.get_complete_color()) { 
      continue; // trivial cycle
    } 

    for(auto im = Mx.cbegin(); im != Mx.cend(); ++im) {
      const std::string& y = im->first;

      if (mark.find(y) != mark.end()) { 
	continue; // already output
      }    

      const Mcolor& C = im->second;
      bool vec_T_color = graph.is_vec_T_color(C);
      for(auto ic = C.cbegin(); ic != C.cend(); ++ic) {
	for (size_t i = 0; i < ic->second; ++i) { 
	       	/*************** output edge (x,y) **************** */
		dot << "\t\"" << x << "\"\t--\t\"";
		if (y == Infty) {
		  if (ic == C.cbegin()) { 
		    --infv;
		  } 
		  dot << infv << "\"\t[len=0.75,";
		} else { 
		  dot << y << "\"\t[";
		} 
		if (vec_T_color) {
			dot << "color=" <<  cfg.get_RGBcolor(cfg.get_RGBcoeff() * (ic->first)) << ", penwidth=3];" << std::endl;
		} else {
			dot << "color=" <<  cfg.get_RGBcolor(cfg.get_RGBcoeff() * (ic->first)) << "];" << std::endl;	
		}
	} 
      }
    }
    mark.insert(x);
  }

  for(int i = infv; i < 0; ++i) {
    dot << "\t\"" << i << "\"\t[shape=point,color=black];" << std::endl;
  }

  dot << "}" << std::endl;
  dot.close();
} 

void writer::Wdots::save_components(const mbgraph_with_history<Mcolor>& graph, const ProblemInstance<Mcolor>& cfg, size_t stage) { 
  std::string dotname = cfg.get_graphname() + toString(stage);

  equivalence<vertex_t> CC; // connected components
  		
  for(auto is = graph.begin_vertices(); is != graph.end_vertices(); ++is) {
    CC.addrel(*is, *is);
  } 
  
  for(auto lc = graph.begin_local_graphs(); lc != graph.end_local_graphs(); ++lc) { 
    for(auto il = lc->cbegin(); il != lc->cend(); ++il) {
      CC.addrel(il->first, il->second);
    }
  }

  CC.update();    
  
  std::map<std::string, std::set<std::string> > components;
  CC.get_eclasses(components);
  
  size_t i = 0; 
  for(auto it = components.cbegin(); it != components.cend(); ++it) { 
    const std::set<std::string>& current = it->second;
    if (current.size() <= 2) {
      continue;
    }
  
    std::string namefile = dotname + "_" + toString(++i) + ".dot"; 
    std::ofstream dot(namefile.c_str());

    dot << "graph {" << std::endl;
    if (!cfg.get_colorscheme().empty()) { 
      dot << "edge [colorscheme=" << cfg.get_colorscheme() << "];" << std::endl;
    } 

    int infv = 0;
    std::unordered_set<vertex_t> mark; // vertex set
    for(auto is = current.cbegin(); is != current.cend(); ++is) {
      const std::string& x = *is;
  
      if (x == Infty) { 
	continue;
      } 

      Mularcs<Mcolor> Mx = graph.get_adjacent_multiedges(x);

      if (Mx.size() == 1 && Mx.cbegin()->second == graph.get_complete_color()) { 
	continue; // trivial cycle
      } 
      
      for(auto im = Mx.cbegin(); im != Mx.cend(); ++im) {
	const std::string& y = im->first;
	
	if (mark.find(y) != mark.end()) { 
	  continue; // already output
	} 

	const Mcolor& C = im->second;
	bool vec_T_color = graph.is_vec_T_color(C);
	for(auto ic = C.cbegin(); ic != C.cend(); ++ic) {
	  for (size_t i = 0; i < ic->second; ++i) { 
		dot << "\t\"" << x << "\"\t--\t\"";
		if (y == Infty) {
		  if (ic == C.cbegin()) { 
		    --infv;
		  } 
		  dot << infv << "\"\t[len=0.75,";
		} else { 
		  dot << y << "\"\t[";
		} 
		if (vec_T_color) {
			dot << "color=" <<  cfg.get_RGBcolor(cfg.get_RGBcoeff() * (ic->first)) << ", penwidth=3];" << std::endl;
		} else {
			dot << "color=" <<  cfg.get_RGBcolor(cfg.get_RGBcoeff() * (ic->first)) << "];" << std::endl;	
		}
	  }
	}
      }
      mark.insert(x);
    }

    for(int i = infv; i < 0; ++i) {
	dot << "\t\"" << i << "\"\t[shape=point,color=black];" << std::endl;
    }

    dot << "}" << std::endl;
    dot.close();
  } 
} 

void writer::Wdots::write_legend_dot(const ProblemInstance<Mcolor>& cfg) { 
    	std::ofstream output("legend.dot");

	output << "digraph Legend {" << std::endl;
	output << "\tnode [style=filled];" << std::endl;

	for (size_t j = 0; j < cfg.get_count_genomes(); ++j) {
	    output << "\t\"" << cfg.get_priority_name(j) << "\"\t[fillcolor=" <<  cfg.get_RGBcolor(cfg.get_RGBcoeff() * j)  << "];" << std::endl;
	} 

	std::vector<std::string> info;
	for(auto it = cfg.cbegin_trees(); it != cfg.cend_trees(); ++it) {
		it->get_nodes<ProblemInstance<Mcolor> >(info, cfg);
	}
 
	
  	for(auto it = info.cbegin(); it != info.cend(); ++it) {
		output << *it << std::endl;
	} 

	output << "}" << std::endl;
	output.close();
} 

