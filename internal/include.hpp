// Copyright (c) 2018, Nicola Prezza.  All rights reserved.
// Use of this source code is governed
// by a MIT license that can be found in the LICENSE file.

#ifndef INCLUDE_HPP_
#define INCLUDE_HPP_

#include <fstream>
#include <vector>
#include <cassert>
#include <algorithm>

using namespace std;

typedef pair<uint64_t,uint64_t> range_t;

//const char TERM = '#';

std::ifstream::pos_type filesize(string filename){
    std::ifstream in(filename.c_str(), std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

/*
 * representation of a right-maximal substring (SA node) as a list of BWT intervals
 */
struct sa_node{

	//right-maximal substring: string W such that Wa_1, ..., Wa_k occur in the text for
	//at least k>=2 characters a_1, ..., a_k

	uint64_t first_TERM;
	uint64_t first_A;
	uint64_t first_C;
	uint64_t first_G;
	uint64_t first_T;
	uint64_t last;

	//depth = |W|
	uint64_t depth;

	uint64_t key(){
		return first_TERM;
	}

};

struct sa_node_n{

	//right-maximal substring: string W such that Wa_1, ..., Wa_k occur in the text for
	//at least k>=2 characters a_1, ..., a_k

	uint64_t first_TERM;
	uint64_t first_A;
	uint64_t first_C;
	uint64_t first_G;
	uint64_t first_N;
	uint64_t first_T;
	uint64_t last;

	//depth = |W|
	uint64_t depth;

	uint64_t key(){
		return first_TERM;
	}

};

/*
 * file contains 'N' characters
 */
bool hasN(string filename){

	std:ifstream i(filename);

	char c;

	while (i.get(c)){

		if(c=='N') return true;

	}

	return false;

}

uint64_t node_size(sa_node s){
	return s.last - s.first_TERM;
}

uint64_t node_size(pair<sa_node, sa_node> p){
	return node_size(p.first) + node_size(p.second);
}

uint64_t node_size(sa_node_n s){
	return s.last - s.first_TERM;
}

uint64_t node_size(pair<sa_node_n, sa_node_n> p){
	return node_size(p.first) + node_size(p.second);
}

void print_node(sa_node n){

	cout << "[" << 	n.first_TERM << ", " <<
					n.first_A << ", " <<
					n.first_C << ", " <<
					n.first_G << ", " <<
					n.first_T << ", " <<
					n.last << "]" << endl;

}

void print_node(sa_node_n n){

	cout << "[" << 	n.first_TERM << ", " <<
					n.first_A << ", " <<
					n.first_C << ", " <<
					n.first_G << ", " <<
					n.first_N << ", " <<
					n.first_T << ", " <<
					n.last << "]" << endl;

}

sa_node merge_nodes(sa_node a, sa_node b){

	assert(a.depth == b.depth);

	return {
		a.first_TERM + b.first_TERM,
		a.first_A + b.first_A,
		a.first_C + b.first_C,
		a.first_G + b.first_G,
		a.first_T + b.first_T,
		a.last + b.last,
		a.depth
	};

}

sa_node_n merge_nodes(sa_node_n a, sa_node_n b){

	assert(a.depth == b.depth);

	return {
		a.first_TERM + b.first_TERM,
		a.first_A + b.first_A,
		a.first_C + b.first_C,
		a.first_G + b.first_G,
		a.first_N + b.first_N,
		a.first_T + b.first_T,
		a.last + b.last,
		a.depth
	};

}

/*
 * suffix array leaf = BWT range (inclusive) of W.TERM, for some string W.
 *
 */
struct sa_leaf{

	//rn.first = first position of range. Equivalently, number of suffixes smaller than W.TERM (valid also if W.TERM does not occur)
	//rn.second = last position (excluded) of interval.  Equivalently, number of suffixes smaller than W.TERM + number of occurrences of W.TERM
	//if last == first, then W.TERM does not occur (however, 'first' is in any case number of suffixes smaller than W.TERM)
	range_t rn;

	//depth = |W.TERM|
	uint64_t depth;

	uint64_t key(){
		return rn.first;
	}

};

inline uint64_t range_length(range_t r){
	assert(r.second >= r.first);
	return r.second - r.first;
}

inline uint64_t leaf_size(sa_leaf L){
	return range_length(L.rn);
}

inline uint64_t leaf_size(pair<sa_leaf, sa_leaf> P){
	return leaf_size(P.first) + leaf_size(P.second);
}


struct p_range{

	range_t A;
	range_t C;
	range_t G;
	range_t T;

};

struct p_node{

	sa_node A;
	sa_node C;
	sa_node G;
	sa_node T;

};

struct p_range_n{

	range_t A;
	range_t C;
	range_t G;
	range_t N;
	range_t T;

};

struct p_node_n{

	sa_node_n A;
	sa_node_n C;
	sa_node_n G;
	sa_node_n N;
	sa_node_n T;

};

struct flags{
	bool TM,A,C,G,N,T;
};

void print_nodes(p_node p){

	print_node(p.A);
	print_node(p.C);
	print_node(p.G);
	print_node(p.T);

}

struct p_rank{

public:

	uint64_t A;
	uint64_t C;
	uint64_t G;
	uint64_t T;

	p_rank operator+(const p_rank& a) const{

		return {
			a.A + A,
			a.C + C,
			a.G + G,
			a.T + T
		};

	}

	bool operator==(const p_rank& a) const{

		return a.A == A and a.C == C and a.G == G and a.T == T;

	}

	bool operator!=(const p_rank& a) const{

		return a.A != A or a.C != C or a.G != G or a.T != T;

	}

	bool operator<=(const p_rank& a) const{

		return A <= a.A and C <= a.C and G <= a.G and T <= a.T;

	}

};

struct p_rank_n{

public:

	uint64_t A;
	uint64_t C;
	uint64_t G;
	uint64_t N;
	uint64_t T;

	p_rank_n operator+(const p_rank_n& a) const{

		return {
			a.A + A,
			a.C + C,
			a.G + G,
			a.N + N,
			a.T + T
		};

	}

	bool operator==(const p_rank_n& a) const{

		return a.A == A and a.C == C and a.G == G and a.N == N and a.T == T;

	}

	bool operator!=(const p_rank_n& a) const{

		return a.A != A or a.C != C or a.G != G or a.N != N or a.T != T;

	}

	bool operator<=(const p_rank_n& a) const{

		return A <= a.A and C <= a.C and G <= a.G and N <= a.N and T <= a.T;

	}

};

inline p_range fold_ranks(p_rank &a, p_rank &b){

	return {{a.A, b.A},{a.C, b.C},{a.G, b.G},{a.T, b.T}};

}

inline p_range_n fold_ranks(p_rank_n &a, p_rank_n &b){

	return {{a.A, b.A},{a.C, b.C},{a.G, b.G},{a.N, b.N},{a.T, b.T}};

}

inline uint64_t popcount128(__uint128_t x){

	return __builtin_popcountll(uint64_t(x>>64)) + __builtin_popcountll( x & 0xFFFFFFFFFFFFFFFF );

}

inline bool has_right_ext_TERM(sa_node N){
	return N.first_A > N.first_TERM;
}
inline bool has_right_ext_A(sa_node N){
	return N.first_C > N.first_A;
}
inline bool has_right_ext_C(sa_node N){
	return N.first_G > N.first_C;
}
inline bool has_right_ext_G(sa_node N){
	return N.first_T > N.first_G;
}
inline bool has_right_ext_T(sa_node N){
	return N.last > N.first_T;
}

inline bool has_right_ext_TERM(sa_node_n N){
	return N.first_A > N.first_TERM;
}
inline bool has_right_ext_A(sa_node_n N){
	return N.first_C > N.first_A;
}
inline bool has_right_ext_C(sa_node_n N){
	return N.first_G > N.first_C;
}
inline bool has_right_ext_G(sa_node_n N){
	return N.first_N > N.first_G;
}
inline bool has_right_ext_N(sa_node_n N){
	return N.first_T > N.first_N;
}
inline bool has_right_ext_T(sa_node_n N){
	return N.last > N.first_T;
}

inline bool empty_node(sa_node_n N){
	return N.last == N.first_TERM;
}

uint8_t number_of_right_ext(sa_node N){

	return 	uint8_t(N.last>N.first_T) +
			uint8_t(N.first_T>N.first_G) +
			uint8_t(N.first_G>N.first_C) +
			uint8_t(N.first_C>N.first_A) +
			uint8_t(N.first_A>N.first_TERM);

}

uint8_t number_of_right_ext(sa_node_n N){

	return 	uint8_t(N.last>N.first_T) +
			uint8_t(N.first_T>N.first_N) +
			uint8_t(N.first_N>N.first_G) +
			uint8_t(N.first_G>N.first_C) +
			uint8_t(N.first_C>N.first_A) +
			uint8_t(N.first_A>N.first_TERM);

}

#endif /* INCLUDE_HPP_ */

