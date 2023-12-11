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

char TERM = '#';

void help(){

	cout << "size-from-bwt [options]" << endl <<
	"Input: BWT of a DNA dataset (alphabet: A,C,G,T,N,#). Output: size of the SLT factorization." << endl <<
	"Options:" << endl <<
	"-i <arg>    Input BWT (REQUIRED)" << endl <<
	"-t          ASCII code of the terminator. Default:" << int('#') << " (#). Cannot be the code for A,C,G,T,N." << endl;
	exit(0);
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

	uint64_t n = bwt.size();

	cout << "Done. Size of BWT: " << n << endl;



	//navigate suffix tree


	uint64_t m = 0;//portion of text covered by visited leaves
	uint64_t leaves = 0;//number of visited leaves
	uint64_t max_stack = 0;

	cout << "Starting navigation of the suffix tree." << endl;

	auto TMP_NODES = vector<typename dna_bwt_n_t::sa_node_t>(5);

	uint64_t nodes = 0;//visited ST nodes
	max_stack = 0;

	stack<typename dna_bwt_n_t::sa_node_t> S;

	typename dna_bwt_n_t::sa_node_t root = bwt.root();

	S.push(root);

	int last_perc = -1;
	int perc = 0;

	while(not S.empty()){

		max_stack = S.size() > max_stack ? S.size() : max_stack;

		typename dna_bwt_n_t::sa_node_t N = S.top();
		S.pop();
		nodes++;

		int t = 0;

		bwt.next_nodes(N, TMP_NODES, t);

		for(int i=t-1;i>=0;--i){

			S.push(TMP_NODES[i]);

		}

		perc = (100*nodes)/n;

		if(perc > last_perc){

			cout << perc << "%." << endl;
			last_perc = perc;

		}

	}

	cout << "Max stack size = " << max_stack << endl;
	cout << "Processed " << nodes << " suffix tree nodes." << endl;

	cout << "Number of suffix link tree leaves: " << bwt.get_number_sl_leaves() << endl;
	cout << "size of SLT factorization (number of right-extensions of suffix link tree leaves): " << bwt.get_number_sl_leaves_ext() << endl;


}

