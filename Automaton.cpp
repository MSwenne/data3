#include "Automaton.h"
/**
 * Implementation of the class Automaton
 * Yenebeb Schwarze (s1908928) & Martijn Swenne (s1923889)
 * Datum: 5 Jan 2018
 * Datastructuren Assignment 3
**/


// adds a new state to the automata
void Automaton::addState(const State state){
	states.insert(state);
}

// adds a new transition from a given state to a given state
void Automaton::addTransition(const State from, const BitVector label, const State to){
	std::map<BitVector, std::set<State> > toMap;
	std::set<State> toState;
	std::map<State, std::map<BitVector, std::set<State> > >::iterator it;
	std::map<BitVector, std::set<State> >::iterator setIt;

	if(alphabet.empty()){
		for(std::map<unsigned,bool>::const_iterator i= label.begin(); i!=label.end(); i++)
			alphabet.insert(i->first);
	}
	it = transitions.find(from);
	if(it != transitions.end()){
		toMap = it->second;
		setIt = toMap.find(label);
		if(setIt != toMap.end()){
			toState = setIt->second;
		}
	}
	toState.insert(to);
	toMap[label] = toState;
	transitions[from] = toMap;
}

// marks a state as an initial state
void Automaton::markInitial(const State state){
	initialStates.insert(state);
}

// marks a state as a final state
void Automaton::markFinal(const State state){
	finalStates.insert(state);
}

// checks if an input qualifies for the automata
void Automaton::parseInput(const std::list<BitVector> input){
	std::list<BitVector>::const_iterator it;
	currentStates = initialStates;
	for (it = input.begin(); it != input.end(); ++it){
		next(*it);
	}
	if(!inFinalState()){
		std::cout << "not in final!" << std::endl;
	}
}

// checks if one of your current states is a final state
bool Automaton::inFinalState() const{
	std::set<State>::iterator itcur;
	std::set<State>::iterator itfin;

	for(itcur = currentStates.begin(); itcur != currentStates.end(); itcur++){
		for(itfin = finalStates.begin(); itfin != finalStates.end(); itfin++){
			if(*itcur == *itfin){
				return true;
			}
		}
	}
	return false;
}

// makes an intersection automata of the two given automata
void Automaton::intersect(Automaton& fa1, Automaton& fa2){
	std::set<BitVector> bitSet;
	std::pair<State, State> pairs;
	std::pair<State, State> nextpair;
	std::map<BitVector, std::set<State> > transf1, transf2; 
	std::set<State> nextf1, nextf2;
	std::map<std::pair<State, State>, int> stateMap;
	std::queue<std::pair<State, State> > remain;
	std::set<State>::iterator itf1, itf2;
	std::set<BitVector>::iterator it;
	
	makeBitSet(fa1.alphabet.size(), bitSet);
	int i = setInitial(fa1, fa2, stateMap, remain);
	while(!remain.empty()){
		pairs = remain.front();
		auto state = stateMap.find(pairs)->second;
		addState(state);
		remain.pop();
		if(fa1.finalStates.find(pairs.first)!=fa1.finalStates.end() || 
			fa2.finalStates.find(pairs.second)!=fa2.finalStates.end()){
			markFinal(stateMap[pairs]);
	}
	for(it = bitSet.begin(); it != bitSet.end(); ++it){
		transf1 = fa1.transitions.find(pairs.first)->second;
		transf2 = fa2.transitions.find(pairs.second)->second;
		nextf1 = transf1.find(*it)->second;
		nextf2 = transf2.find(*it)->second;
		for(itf1 = nextf1.begin(); itf1 != nextf1.end(); ++itf1){
			for(itf2 = nextf2.begin(); itf2 != nextf2.end(); ++itf2){
				nextpair.first = *itf1;
				nextpair.second = *itf2;
				auto next = stateMap.find(nextpair);
				if(next == stateMap.end()){
					stateMap.insert(std::make_pair(nextpair, i));
					i++;
					remain.push(nextpair);
				}	
				next = stateMap.find(nextpair);
				addTransition(state, *it, next->second);
			}
		}
	}
}
}

// makes a map of initial state pairs and their new state value
int Automaton::setInitial(Automaton& fa1, Automaton& fa2, std::map<std::pair<State, State>, int> &stateMap, std::queue<std::pair<State, State> > &remain) {
	std::pair<State, State> pairs;
	std::set<State> states1 = fa1.initialStates;
	std::set<State> states2 = fa2.initialStates;
	int i = 0;

	for(std::set<State>::iterator S1 = states1.begin(); S1!=states1.end(); S1++){
		for(std::set<State>::iterator S2 = states2.begin(); S2!=states2.end(); S2++){
			pairs.first = *S1;
			pairs.second = *S2;
			stateMap.insert(std::make_pair(pairs, i));
			remain.push(pairs);
			markInitial(i);
			i++;
		}
	} 
	return i;
}

// makes a set of every possible BitSet you can make with an alphabet of given length
void Automaton::makeBitSet(int length, std::set<BitVector> &result){
	BitVector bv1;
	BitVector bv2;
	bv1[0] = 0;
	bv2[0] = 1;
	makeBitSet_p(length, 1, bv1, result);
	makeBitSet_p(length, 1, bv2, result);
}

// helper function for makeBitSet
void Automaton::makeBitSet_p(int length, int i, BitVector bv, std::set<BitVector> &result){
	BitVector bv1 = bv;
	BitVector bv2 = bv;
	bv1[i] = 0;
	bv2[i] = 1;
	i++;
	if(length == i) {
		result.insert(bv1);
		result.insert(bv2);
	}
	else {
		makeBitSet_p(length, i, bv1, result);
		makeBitSet_p(length, i, bv2, result);
	}
}

// adds a new variable varnr to the automata
void Automaton::addToAlphabet(unsigned varnr){
	std::map<State, std::map<BitVector, std::set<State> > > newtransitions;
	std::map<State, std::map<BitVector, std::set<State> > >::iterator it;
	std::map<BitVector, std::set<State> > newtrans;
	std::map<BitVector, std::set<State> > trans;
	std::map<BitVector, std::set<State> >::iterator it2;
	std::set<State> nextstates;

	for(it = transitions.begin(); it != transitions.end(); ++it){
		State origstate = it->first;
		trans = it->second;
		for(it2 = trans.begin(); it2 != trans.end(); ++it2){
			nextstates = it2->second;
			BitVector origVec1 = it2->first;
			BitVector origVec2 = it2->first;
			origVec1[varnr] = 0;
			origVec2[varnr] = 1;
			newtrans[origVec1] = nextstates;
			newtrans[origVec2] = nextstates;
		}
		newtransitions.insert(std::make_pair(origstate, newtrans));
	}
	transitions = newtransitions;
}

// makes the complement of the given FA
void Automaton::complement(Automaton& fa){
	fa.makeDeterministic;
	std::set<State> copyInitial = initialStates;
	initialStates = finalStates;
	finalStates = copyInitial;
}

// deletes the given variable from the automata
void Automaton::project(const unsigned variable){
	std::map<State, std::map<BitVector, std::set<State> > >::iterator it;
	for(it = transitions.begin(); it != transitions.end(); ++it){
		std::map<BitVector, std::set<State> > bitMap = it->second;
		std::map<BitVector, std::set<State> >::iterator bitIt;
		for(bitIt = bitMap.begin(); bitIt != bitMap.end(); ++bitIt){
			BitVector vec = bitIt->first;
			if(vec.find(variable) != vec.end()){
				std::map<BitVector, std::set<State> > bit = it->second;
				std::set<State> nextS = bitIt->second;
				bit.erase(vec);
				vec.erase(variable);
				bit[vec] = nextS;
				transitions[it->first] = bit;
			}
		}
	}
}

// makes the given FA deterministic
void Automaton::makeDeterministic(Automaton& fa){
	std::queue<std::set<State> > remain;
	std::map<std::set<State>, int> stateMap;
	std::map<BitVector, std::set<State> > transf; 
	std::set<BitVector> bitSet;
	std::set<BitVector>::iterator it;
	std::set<State> Q, nextf;
	std::set<State>::iterator Qit, itf;
	int i = 0;

	makeBitSet(fa.alphabet.size(), bitSet);
	stateMap.insert(std::make_pair(fa.initialStates, i));
	markInitial(i);
	remain.push(fa.initialStates);
	std::cout << " A " << std::endl;
	while(!remain.empty()){
		Q = remain.front();
		remain.pop();
		auto state = stateMap.find(Q)->second;
		addState(state);
		if(fa.check_intersect(Q)){
			markFinal(i);
		}
		for(it = bitSet.begin(); it != bitSet.end(); ++it){
			std::set<State> nextQ;
			for(Qit = Q.begin(); Qit != Q.end(); ++Qit){
				auto trans = fa.transitions.find(*Qit);
				if(trans != fa.transitions.end()){
					transf = trans->second;
					auto nexta = transf.find(*it);
					if(nexta != transf.end()){

						nextf = nexta->second;
						for(itf = nextf.begin(); itf != nextf.end(); ++itf){
							nextQ.insert(*itf);
						}
					}
				}
			}
			if(!nextQ.empty()){
				auto next = stateMap.find(nextQ);
				if(next == stateMap.end()){
					i++;
					stateMap.insert(std::make_pair(nextQ, i));
					remain.push(nextQ);
				}
				printStates(std::cout, Q);
				next = stateMap.find(nextQ);
				addTransition(state, *it, next->second);
			}
		}
	}
	std::cout << " D " << std::endl;
}

// checks if checkstates intersects with finalstates
bool Automaton::check_intersect(std::set<State> checkStates){
	std::set<State>::iterator checkit;
	for(checkit = checkStates.begin(); checkit != checkStates.end(); ++checkit){
		std::set<State>::iterator finalit;
		for(finalit = finalStates.begin(); finalit != finalStates.end(); ++finalit){
			if(*checkit == *finalit){
				return true;
			}
		}
	}
	return false;
}

// prints all the states
void Automaton::printStates(std::ostream &str, const std::set<State> s) {
	str << "{";
	for(auto& st : s) {
		str << st << ", ";
	}
	str << "}";

}

// puts the states you can reach from your current states with the given BitSet to your current states
void Automaton::next(const BitVector input){
	std::set<State>::iterator itcur;
	std::set<State> nextStates;
	for(itcur = currentStates.begin(); itcur != currentStates.end(); itcur++){
		std::set<State> next = transitions[*itcur][input];
		std::set<State>::iterator itnext;
		for(itnext = next.begin(); itnext != next.end(); itnext++){
			nextStates.insert(*itnext);
		}
	}
	currentStates = nextStates;
}

// prints all the transitions of the automata
void Automaton::printTransitionLabel(std::ostream &str, const BitVector t) {
	str << "{";
	for(auto& bv : t) {
		str << bv.first << "->" << bv.second << ", ";
	}
	str << "}";
}

// prints the automata
void Automaton::print(std::ostream &str) const {
	str << "Initial States: ";
	printStates(str, initialStates);
	str << "Final States: ";
	printStates(str, finalStates);
	str << "Current States: ";
	printStates(str, currentStates);
	str <<"\nTransitions: \n";
	for (auto& trans : transitions) {
		for(auto& m : trans.second) {
			str << "(";
			str << trans.first;
			str << ", ";
			printTransitionLabel(str, m.first);
			str << ", ";
			printStates(str, m.second);
			str << ")\n";
		}
	}
}

// eliminates lambda transitions in fa
void Automaton::eliminateLambda(Automaton& fa){
	std::map<State, std::map<BitVector, std::set<State> > >::iterator it;
	for(it = transitions.begin(); it != transitions.end(); ++it){
		std::map<BitVector, std::set<State> > bitMap = it->second;
		std::map<BitVector, std::set<State> >::iterator bitIt;
		for(bitIt = bitMap.begin(); bitIt != bitMap.end(); ++bitIt){
			BitVector vec = bitIt->first;
			if(vec.){
				std::map<BitVector, std::set<State> > bit = it->second;
				std::set<State> nextS = bitIt->second;
				bit.erase(vec);
				vec.erase(variable);
				bit[vec] = nextS;
				transitions[it->first] = bit;
			}
		}
	}

}

int main() {
    Automaton fa1;
    Automaton fa2;
    Automaton fa3;
    BitVector bv, bv2;
    bv[0] = 0;
    bv[1] = 1;
    bv2[0] = 1;
    bv2[1] = 0;

    fa1.markInitial(0);
    fa1.addState(0);
    fa1.addState(1);
    fa1.addState(2);
    fa1.addTransition(0, bv, 1);
  	fa1.addTransition(1, bv, 2);
	fa1.addTransition(2, bv, 2);
	fa1.addTransition(0, bv2, 0);
    fa1.addTransition(1, bv2, 0);
	fa1.addTransition(2, bv2, 2);
	fa1.markFinal(0);
	fa1.markFinal(1);
    
	fa2.markInitial(0);
  	fa2.addState(0);
    fa2.addState(1);
    fa2.addState(2);
    fa2.addTransition(0, bv, 1);
    fa2.addTransition(1, bv, 1);
    fa2.addTransition(2, bv, 1);
    fa2.addTransition(0, bv2, 0);
    fa2.addTransition(1, bv2, 2);
    fa2.addTransition(2, bv2, 0);
    fa2.markFinal(2);
    
    fa1.print(std::cout);
    fa1.project(0);
    fa1.print(std::cout);
    fa1.project(1);
    fa1.print(std::cout);


/*
    std::cout << std::endl << std::endl << "intersect:" << std::endl;
    fa3.intersect(fa1, fa2);
    fa3.print(std::cout);*/
}





