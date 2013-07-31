#ifndef STAGE_2_H_ 
#define STAGE_2_H_ 

/*
canformQ(x,Q) говорит, можно ли из мультицветов мультиребер инцидентных вершине x образовать мультицвет Q.
can incident multiedges of x form multicolor Q (current don't check T-consistent formation)
if return false, then Q cannot be formed
if true - who knows...
*/
template<class graph_t>
bool Algorithm<graph_t>::canformQ(const std::string& x, const Mcolor& Q) const {
    if (x == Infty) {
	return canformQoo;
    }

    // color Q can be formed if some it adjacent multicolors form a partition of Q
    // OR 
    // if every intersection Q \cap QQ = \emptyset or QQ.

    Mularcs<Mcolor> M = graph.get_adjacent_multiedges(x, split_bad_colors);
    
    for(auto im = M.cbegin(); im != M.cend(); ++im) { 
	Mcolor C(Q, im->second, Mcolor::Intersection); 
	if (C.size() > 0 && C.size() < im->second.size()) { 
		return false;
	} 
    }
    return true;
}

template<class graph_t> 
bool Algorithm<graph_t>::stage2() { 
  bool symplified = false; 
  size_t nr = 0; // number of rearrangements 
  size_t nf = 0; // number of fussions/fissions

  do {
    nr = 0; 
    nf = 0;
	
    for(auto is = graph.begin_vertices(); is != graph.end_vertices(); ++is) {  
      const std::string& x = *is;

      if (graph.is_duplication_vertex(*is) || graph.is_indel_vertex(*is)) { 
	continue; 
      } 

      Mularcs<Mcolor> oldM = graph.get_adjacent_multiedges(x);
      Mularcs<Mcolor> oldCx = graph.get_adjacent_multiedges(x, split_bad_colors);	
      Mularcs<Mcolor> M;
      Mularcs<Mcolor> Cx;
      for(auto im = oldM.cbegin(); im != oldM.cend(); ++im) { 
	if (im->first != Infty && (graph.is_duplication_vertex(im->first) || graph.is_indel_vertex(im->first))) { 	
	  continue;
	} else {
	  auto ins = oldCx.equal_range(im->first);
	  Cx.insert(ins.first, ins.second);
	  M.insert(im->first, im->second);
	}
      }
      M.erase(Infty);

      for(auto im = M.cbegin(); im != M.cend(); ++im) {
	const std::string& y = im->first;
	const Mcolor& Q = im->second; // color of central edge

	if (Q.size() == graph.size_graph()) { 
	  continue;
	} 

	Mularcs<Mcolor> oldCy = graph.get_adjacent_multiedges(y, split_bad_colors);
	oldCy.erase(x);
	Mularcs<Mcolor> Cy;
	std::unordered_set<vertex_t> processed;
	for(auto iy = oldCy.cbegin(); iy != oldCy.cend(); ++iy) { 
	  if (processed.find(iy->first) != processed.cend()) {
		continue;
	  }
	  if (iy->first != Infty && (graph.is_duplication_vertex(iy->first) || graph.is_indel_vertex(iy->first))) { 	
		processed.insert(iy->first);
	  } else {
		processed.insert(iy->first);
		auto ins = oldCy.equal_range(iy->first);
		Cy.insert(ins.first, ins.second);
	  }
	}
	  
	//std::cerr << "Testing mobility of edge " << x << "-" << y << " " << genome_match::mcolor_to_name(Q) << " ";

	// test "mobility" of central edge
	// can it be ever find neighboring edge of the same multicolor
	bool mobilQ = false;

	//here 
	for(auto jc = Cx.cbegin(); jc != Cx.cend(); ++jc) {
	  if (jc->first != y) { 
		continue; 
	  } // not a cental sub-edge

	  const Mcolor& QQ = jc->second; // color of central sub-edge (QQ is sub-multicolor of Q)
	  if (!graph.is_vec_T_color(QQ)) { 
		continue;
	  } 


	  for(auto ix = Cx.cbegin(); ix != Cx.cend(); ++ix) { 
	    if (ix->first == y) { 
		continue; 
	    } 

	    if (canformQ(ix->first, QQ)) {
	      //std::cerr << "MOBIL: " << x << "-" << ix->first << " canForm: " << genome_match::mcolor_to_name(QQ) << std::endl;
	      mobilQ = true;
	      break;
	    }
	  }
	
	  if (mobilQ) { 
	    break;
	  } 
    
	  for(auto iy = Cy.cbegin(); iy != Cy.cend(); ++iy) { 
	    if (canformQ(iy->first, QQ)) {
	      //std::cerr << "MOBIL: " << y << "-" << iy->first << " canForm: " << genome_match::mcolor_to_name(QQ) << std::endl;
	      mobilQ = true;
	      break;
	    }
	  }

	  if (mobilQ) { 
	    break;
	  } 
	}

	if (mobilQ) continue;

	//std::cerr << "NOT MOBIL" << std::endl;

	bool found = false;

	for (auto ix = Cx.cbegin(); ix != Cx.cend(); ++ix) { 
	  if (ix->first == y) { 
		continue;
  	  }

	  const Mcolor& QQ = ix->second;

	  //std::cerr << " Sub-multiedge " << genome_match::mcolor_to_name(ix->second) << std::endl;

	  vertex_t temp = "";   
	  for(auto iy = Cy.cbegin(); iy != Cy.cend(); ++iy) { 
	    if (iy->second == ix->second) { 	
	      temp = iy->first;
	      break; 
	    } 
	  } 

	  if (!graph.is_vec_T_color(QQ) || temp.empty()) { 
		continue; 
	  }

          graph.apply_two_break(TwoBreak<Mcolor>(x, ix->first, y, temp, QQ));
	  found = true;
	  ++nf;
	}

	if (found) { break; } 
      }
    } 

    if (nr != 0 || nf != 0) { 
      symplified = true;
    } 
  } while (nr > 0 || nf > 0); 
   
  return symplified;
} 
 
#endif
