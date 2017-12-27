#include "Automaton.h"



// TODO (voor studenten - deel 1): VOEG HIER DE IMPLEMENTATIES VAN DE OPERATIES IN Automaton.h TOE


void Automaton::addState(const State state){
	states.insert(state);
}

void Automaton::addTransition(const State from, const BitVector label, const State to){
	std::map<BitVector, std::set<State> > toMap;
	std::set<State> toState;
	std::map<State, std::map<BitVector, std::set<State> > >::iterator it;
	std::map<BitVector, std::set<State> >::iterator setIt;

	if(alphabet.empty()){
		for(std::map<int,bool>::iterator i=label.begin(); i!=label.end(); i++)
			alphabet.insert(i->first);
	}

	it = transitions.find(from);
	if(it == transitions.end()){
		toState.insert(to);
		toMap[label] = toState;
		transitions[from] = toMap;
		return;
	}
	toMap = it->second;
	setIt = toMap.find(label);
	if(setIt != toMap.end()){
		setIt->second.insert(to);
		return;
	}
	toState.insert(to);
	toMap[label] = toState;

}

void Automaton::markInitial(const State state){
	initialStates.insert(state);
}

void Automaton::markFinal(const State state){
	finalStates.insert(state);
}

void Automaton::parseInput(const std::list<BitVector> input){

}

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

void Automaton::intersect(Automaton& fa1, Automaton& fa2){
	std::set<State> Q, d, F, remain;
	std::set<State> states1 = fa1->initialStates;	
	std::set<State> states2 = fa2->initialStates;	
	std::map<BitVector, std::set<State> > toState;
	
	for(std::set<State>::iterator S1 = states1.begin(); S1!=states1.end(); S1++){
		for(std::set<State>::iterator S2 = states2.begin(); S2!=states2.end(); S2++){
			if(*S1 == *S2){
				initialStates.insert(S1);
			}
		}
	}
	remain = initialStates;

	while(!remain.isEmpty()){
		toState = remain
	}
	return false;
}

void Automaton::addToAlphabet(unsigned varnr){

}

void Automaton::complement(Automaton& fa){

}

void Automaton::project(const unsigned variable){

}

void Automaton::makeDeterministic(Automaton& fa){

}

void Automaton::printStates(std::ostream &str, const std::set<State> s) {
    str << "{";
    for(auto& st : s) {
        str << st << ", ";
    }
    str << "}";
    
}

void Automaton::printTransitionLabel(std::ostream &str, const BitVector t) {
    str << "{";
    for(auto& bv : t) {
        str << bv.first << "->" << bv.second << ", ";
    }
    str << "}";
}

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
            
            // print label of source state
            str << trans.first;
            str << ", ";
            
            // print label of transition
            printTransitionLabel(str, m.first);
            str << ", ";
            
            // print target states
            printStates(str, m.second);
            str << ")\n";
        }
    }
}


