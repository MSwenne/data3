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
		for(std::map<unsigned,bool>::const_iterator i= label.begin(); i!=label.end(); i++)
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
	std::list<BitVector>::const_iterator it;
	currentStates = initialStates;
	for (it = input.begin(); it != input.end(); ++it){
		next(*it);
	}
	if(!inFinalState()){
		std::cout << "not in final!" << std::endl;
	}
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
  std::set<BitVector> bitSet;
	std::pair<State, State> pairs;
  std::pair<State, State> nextpair;
 	std::map<BitVector, std::set<State> > transf1, transf2; 
	std::set<State> nextf1, nextf2;
	std::map<std::pair<State, State>, unsigned> stateMap;
	std::queue<std::pair<State, State> > remain;

	makeBitSet(fa1.alphabet.size(), bitSet);
	unsigned i = setInitial(fa1, fa2, stateMap, remain);

  while(!remain.empty()){
		pairs = remain.front();
		auto state = stateMap.find(pairs)->second;
 		addState(state);
 		remain.pop();
 		if(fa1.finalStates.find(pairs.first)!=fa1.finalStates.end() && 
			 fa2.finalStates.find(pairs.second)!=fa2.finalStates.end()){
 			markFinal(stateMap[pairs]);
 		}
		std::set<BitVector>::iterator it;
		for(it = bitSet.begin(); it != bitSet.end(); ++it){
 			transf1 = fa1.transitions.find(pairs.first)->second;
 			transf2 = fa2.transitions.find(pairs.second)->second;
 			nextf1 = transf1.find(*it)->second;
			nextf2 = transf2.find(*it)->second;
			std::set<State>::iterator itf1, itf2;
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
		    	addTransition(state, *it, next->second);
  			}
			}
  	}
  } 
}

unsigned Automaton::setInitial(Automaton& fa1, Automaton& fa2, std::map<std::pair<State, State>, unsigned> &stateMap, std::queue<std::pair<State, State> > &remain) {
	std::pair<State, State> pairs;
  std::set<State> states1 = fa1.initialStates;
  std::set<State> states2 = fa2.initialStates;
	unsigned i = 0;

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

void Automaton::makeBitSet(int length, std::set<BitVector> &result){
	BitVector bv1;
	BitVector bv2;
	bv1[0] = 0;
	bv2[0] = 1;
	makeBitSet_p(length, 1, bv1, result);
	makeBitSet_p(length, 1, bv2, result);
}

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

void Automaton::addToAlphabet(unsigned varnr){

}

void Automaton::complement(Automaton& fa){

}

void Automaton::project(const unsigned variable){

}

void Automaton::makeDeterministic(Automaton& fa){
/*	std::set<State> Q, d, F, remain;
  std::set<State> S = fa.initialStates;
  std::set<State> remain = S;

  while(!remain.empty()){

  }
  std::map<BitVector, std::set<State> > toState;

	remain = initialStates; 
*/
}

void Automaton::printStates(std::ostream &str, const std::set<State> s) {
    str << "{";
    for(auto& st : s) {
        str << st << ", ";
    }
    str << "}";
    
}


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


