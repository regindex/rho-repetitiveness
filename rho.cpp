// Copyright (c) 2023, Nicola Prezza.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

/*
 *
 *  Created on: Dec 11, 2023
 *      Author: Nicola Prezza
 */


#include <iostream>
#include <unistd.h>
#include "internal/dna_bwt_n.hpp"
#include <stack>
#include <algorithm>

using namespace std;

string input_bwt;
dna_bwt_n_t bwt;
vector<bool> suffixient_bwt; //marks set of nexessary+suffixient BWT positions

bool containsN = false;
uint64_t nodes=0; // number of visited nodes

int last_perc = -1;
int perc = 0;
uint64_t n=0;

uint64_t rec_depth = 0;
uint64_t max_rec_depth = 0;

char TERM = '#';

void help(){

	cout << "rho [options]" << endl <<
	"Input: BWT of a DNA dataset (alphabet: A,C,G,T,N,#). Output: value of the rho repetitiveness measure and related statistics." << endl <<
	"Options:" << endl <<
	"-i <arg>    Input BWT (REQUIRED)" << endl <<
	"-t          ASCII code of the terminator. Default:" << int('#') << " (#). Cannot be the code for A,C,G,T,N." << endl;
	exit(0);
}

//recursively processes node and return cost of its subtree, i.e. total number of 
//right-extensions that we pay
uint64_t process_node(	typename dna_bwt_n_t::sa_node_t& x, 
						//function process_node will overwrite this with right ext covered 
						//by weiner descendants of x
						flags& covered_from_wchildren, 
						//pass in to_be_covered the right-extensions that must be covered in x (or 
						//in its Weiner descendants) due to ancestors of x
						flags& to_be_covered
						){ 

	rec_depth++;

	max_rec_depth = std::max(max_rec_depth,rec_depth);

	uint64_t rho = 0;

	//we recurse on all but the last child of x. On the last child, we cycle in this while loop, 
	//replacing x with its last child. Since we process recursively children in decreasing order of
	//BWT interval length, this guarantees that the recursion depth is logarithmic.
	while(true){

		nodes++;
		perc = (100*nodes)/n;

		if(perc > last_perc){

			cout << perc << "%." << endl;
			last_perc = perc;

		}

		//get (right-maximal) children of x in the Weiner tree
		int t = 0;
		auto children = vector<typename dna_bwt_n_t::sa_node_t>(5); 
		bwt.get_weiner_children(x, children, t);

		//right-extensions that need to be covered on x.
		//these are the right-extensions of x, plus those that needed to 
		//be covered in some ancestor of x
		to_be_covered.TM |= has_child_TERM(x);
		to_be_covered.A  |= has_child_A(x);
		to_be_covered.C  |= has_child_C(x);
		to_be_covered.G  |= has_child_G(x);
		to_be_covered.N  |= has_child_N(x);
		to_be_covered.T  |= has_child_T(x);

		if(t==0){ 

			// no children in the Weiner tree: pay the right extensions that need to be covered

			rho += to_be_covered.TM;
			rho += to_be_covered.A;
			rho += to_be_covered.C;
			rho += to_be_covered.G;
			rho += to_be_covered.N;
			rho += to_be_covered.T;

			covered_from_wchildren = flags{ to_be_covered.TM,
											to_be_covered.A,
											to_be_covered.C,
											to_be_covered.G,
											to_be_covered.N,
											to_be_covered.T };

			break;

		}else{

			covered_from_wchildren = flags{false,false,false,false,false,false};

			//scan all children but the last
			for(int i=0;i<t-1;++i){

				flags tmp_covered_children {false,false,false,false,false,false};
				flags tmp_to_be_covered {false,false,false,false,false,false};

				rho += process_node(children[i],tmp_covered_children,tmp_to_be_covered);

				covered_from_wchildren.TM |= tmp_covered_children.TM;
				covered_from_wchildren.A  |= tmp_covered_children.A;
				covered_from_wchildren.C  |= tmp_covered_children.C;
				covered_from_wchildren.G  |= tmp_covered_children.G;
				covered_from_wchildren.N  |= tmp_covered_children.N;
				covered_from_wchildren.T  |= tmp_covered_children.T;
				
			}

			to_be_covered.TM = to_be_covered.TM and not covered_from_wchildren.TM;
			to_be_covered.A  = to_be_covered.A  and not covered_from_wchildren.A;
			to_be_covered.C  = to_be_covered.C  and not covered_from_wchildren.C;
			to_be_covered.G  = to_be_covered.G  and not covered_from_wchildren.G;
			to_be_covered.N  = to_be_covered.N  and not covered_from_wchildren.N;
			to_be_covered.T  = to_be_covered.T  and not covered_from_wchildren.T;

			x = children[t-1];

		}
	
	}

	rec_depth--;
	return rho;

}

int main(int argc, char** argv){

	if(argc < 3) help();

	int opt;
	while ((opt = getopt(argc, argv, "hi:o:l:t:")) != -1){
		switch (opt){
			case 'h':
				help();
			break;
			case 'i':
				input_bwt = string(optarg);
			break;
			case 't':
				TERM = atoi(optarg);
			break;
			default:
				help();
			return -1;
		}
	}

	if(TERM == 'A' or TERM == 'C' or TERM == 'G' or TERM == 'T' or TERM == 'N'){

		cout << "Error: invalid terminator '" << TERM << "'" << endl;
		help();

	}

	if(input_bwt.size()==0) help();

	cout << "Input BWT file: " << input_bwt << endl;

	cout << "Loading and indexing BWT ... " << endl;

	bwt = dna_bwt_n_t(input_bwt, TERM);

	n = bwt.size();

	cout << "Done. Size of BWT: " << n << endl;

	//navigate suffix link tree

	cout << "Starting DFS navigation of the suffix link tree." << endl;

	auto x = bwt.root();

	flags tmp_covered_children {false,false,false,false,false,false};
	flags tmp_to_be_covered {false,false,false,false,false,false};

	uint64_t rho = process_node(x, tmp_covered_children,tmp_to_be_covered);

	cout << "Processed " << nodes << " suffix tree nodes." << endl;

	cout << "rho = " << rho << endl;
	cout << "Number of suffix link tree leaves: " << bwt.get_number_sl_leaves() << endl;
	cout << "Number of right-extensions of suffix link tree leaves: " << bwt.get_number_sl_leaves_ext() << endl;
	cout << "Maximum recursion depth = " << max_rec_depth << endl;

}

