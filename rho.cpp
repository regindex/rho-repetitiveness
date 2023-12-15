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

using namespace std;

string input_bwt;

bool containsN = false;
uint64_t nodes=0; // number of visited nodes

int last_perc = -1;
int perc = 0;
uint64_t n=0;

char TERM = '#';

void help(){

	cout << "rho [options]" << endl <<
	"Input: BWT of a DNA dataset (alphabet: A,C,G,T,N,#). Output: value of the rho repetitiveness measure and related statistics." << endl <<
	"Options:" << endl <<
	"-i <arg>    Input BWT (REQUIRED)" << endl <<
	"-t          ASCII code of the terminator. Default:" << int('#') << " (#). Cannot be the code for A,C,G,T,N." << endl;
	exit(0);
}


//recursively processes node and return cost of its subtree, i.e. total number of right-extensions that we pay
uint64_t process_node(dna_bwt_n_t& bwt, typename dna_bwt_n_t::sa_node_t& x){

	nodes++;

	perc = (100*nodes)/n;

	if(perc > last_perc){

		cout << perc << "%." << endl;
		last_perc = perc;

	}

	int t = 0;

	auto children = vector<typename dna_bwt_n_t::sa_node_t>(5); 

	//get (right-maximal) children of current_node in the suffix link tree
	bwt.next_nodes(x, children, t);

	for(int i=t-1;i>=0;--i){

		process_node(bwt, children[i]);
		
	}

	return 0;

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

	dna_bwt_n_t bwt = dna_bwt_n_t(input_bwt, TERM);

	n = bwt.size();

	cout << "Done. Size of BWT: " << n << endl;

	//navigate suffix link tree

	cout << "Starting DFS navigation of the suffix link tree." << endl;

	auto x = bwt.root();
	process_node(bwt, x);

	cout << "Processed " << nodes << " suffix tree nodes." << endl;

	cout << "Number of suffix link tree leaves: " << bwt.get_number_sl_leaves() << endl;
	cout << "Number of right-extensions of suffix link tree leaves: " << bwt.get_number_sl_leaves_ext() << endl;


}

