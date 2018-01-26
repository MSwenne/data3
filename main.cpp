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

// checks on mod for createAutomaton
bool modTest(Automaton & theAuto, int checkb, int b, int & state, std::queue<State> & states){
	if(std::abs(checkb%2) == b%2){
		state = (b-checkb)/2;
		if(!theAuto.hasState(state)){
			theAuto.addState(state);
			states.push(state);
		}
		return true;
	}
	return false;
}

// adds alphabet of pres to automaton
void addAlphabet(Automaton & theAuto, std::map<unsigned,int> pres){
	std::set<unsigned> newAlphabet;

	std::map<unsigned,int>::iterator presit;
	for(presit = pres.begin(); presit != pres.end(); ++presit){
		newAlphabet.insert(presit->first);
	}
	theAuto.setAlphabet(newAlphabet);
}

// creates automaton when root of exprtree is EQUAL
Automaton createAutomaton_A(ExprTree * exprtree, Automaton & theAuto){
	std::queue<State> states;
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
		
		addAlphabet(theAuto, pres);
		theAuto.makeBitSet(bits);

		bititSet = bits.begin();
		bitcurr = *bititSet;
		while(!states.empty()){
			statecurr = states.front();

			b = statecurr;
			states.pop();
			for(bititSet = bits.begin(); bititSet != bits.end(); ++bititSet){
				checkb = 0;
				bitcurr = *bititSet;
				for(bitit = bitcurr.begin(); bitit != bitcurr.end(); ++bitit){
					presit = pres.find(bitit->first);
					if(presit != pres.end()){
						checkb += presit->second * bitit->second;
					}
				}
				if(modTest(theAuto, checkb, b, state, states)){
					theAuto.addTransition(statecurr, bitcurr, state);
				}

			}
		}
		theAuto.markFinal(0);
	}
	return theAuto;
}

// Checks if automaton 1 and 2 have same alphabet, and if not add new var to alphabet to fix
void checkAlphabet(Automaton & theAuto1, Automaton & theAuto2){
	std::set<unsigned> alphabet1 = theAuto1.getAlphabet();
	std::set<unsigned> alphabet2 = theAuto2.getAlphabet();

	if(alphabet1 != alphabet2){
		std::set<unsigned>::iterator it1;
		std::set<unsigned>::iterator it2;
		for(it1 = alphabet1.begin(); it1 != alphabet1.end(); ++it1){
			it2 = alphabet2.find(*it1);
			if(it2 == alphabet2.end()){
				theAuto2.addToAlphabet(*it1);
			}
		}
		for(it2 = alphabet2.begin(); it2 != alphabet2.end(); ++it2){
			it1 = alphabet1.find(*it2);
			if(it1 == alphabet1.end()){
				theAuto1.addToAlphabet(*it2);
			}
		}
	}
}

// makes automaton for quantification
Automaton createExists(ExprTree *exprtree){
	Automaton theAuto;
	Automaton theAuto2;
	Automaton theAuto3;		
	unsigned proj;
	if(exprtree->getRoot()->getLeft()->getData().type == expr::VARIABLE){
		proj = exprtree->getRoot()->getLeft()->getData().variable;
		exprtree->setRoot(exprtree->getRoot()->getRight());
	}
	else if (exprtree->getRoot()->getRight()->getData().type == expr::VARIABLE){
		proj = exprtree->getRoot()->getRight()->getData().variable;
		exprtree->setRoot(exprtree->getRoot()->getLeft());
	}
	
	theAuto3 = createAutomaton_A(exprtree, theAuto2);
	theAuto3.project(proj);
	theAuto.makeDeterministic(theAuto3);
	return theAuto;
}

// makes automaton for Logic conjunction
Automaton createAnd(ExprTree *exprtree){
	Automaton theAuto;
	Automaton theAuto2;
	Automaton theAuto3;
	node<expr>* right = exprtree->getRoot()->getRight();
	node<expr>* left = exprtree->getRoot()->getLeft();
	exprtree->setRoot(right);
	createAutomaton_A(exprtree, theAuto2);

	exprtree->setRoot(left);
	createAutomaton_A(exprtree, theAuto3);

	checkAlphabet(theAuto2, theAuto3);
	theAuto.intersect(theAuto2, theAuto3);
	return theAuto;
}

// makes automaton for negation
Automaton createNot(ExprTree *exprtree){
	Automaton theAuto;
	Automaton theAuto2;
	Automaton theAuto3;
	exprtree->setRoot(exprtree->getRoot()->getLeft());
	createAutomaton_A(exprtree, theAuto2);
	theAuto3.makeDeterministic(theAuto2);
	theAuto.complement(theAuto3);
	
	return theAuto;
}

// main function createAutomaton
// checks on root
Automaton createAutomaton(ExprTree *exprtree){
	Automaton theAuto;
	Automaton theAuto2;
	Automaton theAuto3;

	// depending on root we return the right Automaton
	if(exprtree->getRoot()->getData().type == expr::EXISTS){
		return createExists(exprtree);
	}
	else if(exprtree->getRoot()->getData().type == expr::AND){
		return createAnd(exprtree);
	}
	else if(exprtree->getRoot()->getData().type == expr::NOT){
		return createNot(exprtree);
	}
	
	return createAutomaton_A(exprtree, theAuto);

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

