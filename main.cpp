#include <iostream>
#include <cmath>
#include "exprtree.h"
#include "Automaton.h"

/**
 * Implementation of the class main
 * Yenebeb Schwarze (s1908928) & Martijn Swenne (s1923889)
 * Datum: 5 Jan 2018
 * Datastructuren Assignment 3
**/



Automaton createAutomaton_A(ExprTree * exprtree, Automaton & theAuto){
	std::queue<State> states;
	std::set<State>::iterator statesit;
	std::set<unsigned> newAlphabet;
	std::map<unsigned,int> pres;
	std::map<unsigned,int>::iterator presit;
	std::set<BitVector> bits;
	std::set<BitVector>::iterator bititSet;
	BitVector bitcurr;
	std::map<unsigned, bool>::iterator bitit;
	State statecurr;
	int b = 0, checkb = 0, state = 0;

	if(exprtree->getPresburgerMap(pres, b)){
		theAuto.addState(b);
		theAuto.markInitial(b);
		states.push(b);
		std::cout << "b BABLABLA = " << b << std::endl;
		
		for(presit = pres.begin(); presit != pres.end(); ++presit){
			std::cout << "addToAlphabet = " << presit->first << std::endl;
			newAlphabet.insert(presit->first);
		}
		theAuto.setAlphabet(newAlphabet);


		theAuto.makeBitSet(bits);
		std::cout << "b BABLABLA = " << b << std::endl;

		bititSet = bits.begin();
		bitcurr = *bititSet;
		while(!states.empty()){
			statecurr = states.front();
			std::cout << statecurr << std::endl;

			b = statecurr;
			states.pop();
			for(bititSet = bits.begin(); bititSet != bits.end(); ++bititSet){
				checkb = 0;
				bitcurr = *bititSet;
				for(bitit = bitcurr.begin(); bitit != bitcurr.end(); ++bitit){
					presit = pres.find(bitit->first);
					std::cout << " wa " << bitit->first << std::endl;
					if(presit != pres.end()){
						std::cout << " a = " << presit->second << " x = " << bitit->second << " wa " << bitit->first << std::endl;

						checkb += presit->second * bitit->second;
						std::cout << "checkb: " << checkb << std::endl;
					}
					else{
						std::cout << "Wrong bitmap." << std::endl;
					}
				}
				//std::cout << "end bitit || b = " << b << " checkb " << checkb << std::endl;
				//std::cout << " checkb mod 2 " << checkb%2 << std::endl;
				if(std::abs(checkb%2) == b%2){
					state = (b-checkb)/2;
					if(!theAuto.hasState(state)){
						std::cout << "ADDING " << state << std::endl;
						theAuto.addState(state);
						states.push(state);
					}
					//std::cout << "Trans " << statecurr << " To " << state << std::endl; 
					theAuto.addTransition(statecurr, bitcurr, state);
				}
			}
		}
		theAuto.markFinal(0);

	}
	else{
		std::cout << "Wrong expression tree." << std::endl;
	}
	return theAuto;
}

void checkAlphabet(Automaton & theAuto1, Automaton & theAuto2){
	std::set<unsigned> alphabet1 = theAuto1.getAlphabet();
	std::set<unsigned> alphabet2 = theAuto2.getAlphabet();

	if(alphabet1 != alphabet2){
		std::set<unsigned>::iterator it1;
		std::set<unsigned>::iterator it2;
		for(it1 = alphabet1.begin(); it1 != alphabet1.end(); ++it1){
			it2 = alphabet2.find(*it1);
			if(it2 == alphabet2.end()){
				std::cout << " ADDING " << *it1 << std::endl;
				theAuto2.addToAlphabet(*it1);
			}
		}
		for(it2 = alphabet2.begin(); it2 != alphabet2.end(); ++it2){
			it1 = alphabet1.find(*it2);
			if(it1 == alphabet1.end()){
				std::cout << " ADDING " << *it2 << std::endl;

				theAuto1.addToAlphabet(*it2);
			}
		}
	}
}

Automaton createAutomaton(ExprTree *exprtree){
	Automaton theAuto;
	Automaton theAuto2;
	Automaton theAuto3;

	if(exprtree->getRoot()->getData().type == expr::EQUALS){
		return createAutomaton_A(exprtree, theAuto);
	}
	else if(exprtree->getRoot()->getData().type == expr::EXISTS){
		std::cout << "Exist " << std::endl;

		unsigned proj;
		if(exprtree->getRoot()->getLeft()->getData().type == expr::VARIABLE){
			proj = exprtree->getRoot()->getLeft()->getData().variable;
			exprtree->setRoot(exprtree->getRoot()->getRight());
		}
		else if (exprtree->getRoot()->getRight()->getData().type == expr::VARIABLE){
			proj = exprtree->getRoot()->getRight()->getData().variable;
			exprtree->setRoot(exprtree->getRoot()->getLeft());
		}
		else{
			std::cout << "WRONG " << std::endl;

		}
		theAuto3 = createAutomaton_A(exprtree, theAuto2);
		theAuto3.project(proj);
		theAuto.makeDeterministic(theAuto3);

	}
	else if(exprtree->getRoot()->getData().type == expr::AND){
		node<expr>* right = exprtree->getRoot()->getRight();
		node<expr>* left = exprtree->getRoot()->getLeft();
		exprtree->setRoot(right);
		createAutomaton_A(exprtree, theAuto2);

		exprtree->setRoot(left);
		createAutomaton_A(exprtree, theAuto3);
		

		checkAlphabet(theAuto2, theAuto3);
		theAuto2.print(std::cout);
		theAuto3.print(std::cout);
		theAuto.intersect(theAuto2, theAuto3);

	}
	else if(exprtree->getRoot()->getData().type == expr::NOT){
		exprtree->setRoot(exprtree->getRoot()->getLeft());
		createAutomaton_A(exprtree, theAuto2);
		theAuto2.print(std::cout);

		theAuto3.makeDeterministic(theAuto2);

		theAuto.complement(theAuto3);
		theAuto3.print(std::cout);

	}
	
	return theAuto;
}



void addVarToBitVectors(std::list<BitVector> &l, const unsigned index, int val) {
	for(std::list<BitVector>::iterator it = l.begin(); it != l.end(); ++it) {
		bool bit = (val&1);
		(*it)[index] = bit;
		val>>=1;
	}

    // create BitVector in which all variables have a bit value of 0 
	BitVector zeroVector;
	if(l.size() > 0)
		zeroVector = *(l.begin());
	for(BitVector::iterator it = zeroVector.begin(); it != zeroVector.end(); ++it) {
		it->second = 0;
	}

    // val requires more bits than l.size(): add new vectors at the end of l
	while(val || l.size() == 0) {
		BitVector b = zeroVector;
		bool bit = (val&1);
		b[index] = bit;
		l.push_back(b);
		val>>=1;
	}
}

/** Utility function for BitVectors. Prints the content all BitVectors in the list to the given output stream
*/
void printBitVectors(std::ostream &out, std::list<BitVector> l) {
	for(BitVector b: l) {
		out << "[";
		for(std::pair<unsigned, bool> elem : b) {
			out << elem.first << ": " << elem.second << ", ";
		}
		out << "]\n";
	}
}

std::list<BitVector> generateBitVectors(std::map<unsigned,unsigned> valueMap){
	std::list<BitVector> l;
	for(auto &var : valueMap){
		addVarToBitVectors(l, var.first, var.second);
	}
	return l;
}

bool verifyAutomaton(Automaton& theAuto, string formula, bool debug, std::ostream &out){
	std::stringstream ss(formula);
	string data;
	int variable;
	int value;
	std::map<unsigned,unsigned> valueMap;
	while(ss >> data){
		if(data[0] == 'x'){
			string substr = data.substr(1,data.find('=')-1);
			variable = std::atoi(substr.c_str());
			if(variable == 0)
				return false;
			string substr2 = data.substr(data.find('=')+1,string::npos);
			value =  std::atoi(substr2.c_str());
			valueMap[variable] = value;
		}
		else
			return false;
	}
	std::list<BitVector> l = generateBitVectors(valueMap);
	theAuto.parseInput(l);
	if(debug) {
		printBitVectors(out,l);
	}
	return theAuto.inFinalState();
}


void menu(bool debug, std::istream& inStr, std::ostream &out){
	Automaton theAuto;
	ExprTree * t = new ExprTree();
	while(true){
		string in;
		getline(inStr,in, '\n');
		if(in.substr(0,5) == "read "){
			if(t->create(in.substr(4,string::npos))) {
				theAuto = createAutomaton(t);
				if(debug) {
					theAuto.print(out);
				}
			} else if(!debug) {
				out << "Error: invalid formula" <<std::endl;
			}
		}
		else if(in.substr(0,6) == "check "){
			if(verifyAutomaton(theAuto,in.substr(5,string::npos), debug, out))
				out << "valid" <<std::endl;
			else
				out << "invalid" <<std::endl;
		}
		else if(in.substr(0,3) == "end")
			return;
		else if(!debug)
			out << "Error: invalid input" <<std::endl;
	}
	delete t;
}

int main(int argc, char** argv)
{
	bool debug = false;
	if(argc >= 2 && string(argv[1]).find("d") != string::npos){
		debug = true;
	}
	menu(debug, std::cin, std::cout);
	return 0;
}

