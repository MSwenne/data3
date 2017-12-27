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
   std::set<State> states1 = fa1->initialStates;       
   std::set<State> states2 = fa2->initialStates;  
   
   std::pair<State, State> > pairs;
   std::pair<std::pair<State, State>, int > intpair;
   std::map<std::pair<State, State>, int> stateMap;
   std::queue<std::pair<std::pair<State, State>, int > > remain;

   int i = 0;

   for(std::set<State>::iterator S1 = states1.begin(); S1!=states1.end(); S1++){
     for(std::set<State>::iterator S2 = states2.begin(); S2!=states2.end(); S2++){
       pairs.first = S1*;
       pairs.second = S2*;

       intpair.first = pairs;
       intpair.second = i;

       remain.push(intpair);
       initialStates.insert(i);
       i++;
     }
   } 
   
   while(!remain.isEmpty()){
   		intpairs = remain.peek();
   		pairs = intpairs.first;
   		int state = intpairs.second;

   		states.insert(state);
   		remain.pop();

   		if(fa1.finalStates.contains(pairs.first) && fa2.finalStates.contains(pairs.second)){
   			markFinal(i);
   		}

   		std::set<unsigned>::iterator it;
   		int laststate = i;
   		for(it = fa1->alphabet.begin(); it != fa1->alphabet.end(); ++it){
   			std::map<BitVector, std::set<State> transf1 = fa1->transitions.find(pairs.first);
   			std::map<BitVector, std::set<State> transf2 = fa2->transitions.find(pairs.second);

   			std::set<State> nextf1 = transf1.find(*it);
   			std::set<State> nextf2 = transf2.find(*it);

   			std::set<State>::iterator itf1;
   			std::set<State>::iterator itf2;

   			for(itf1 = nextf1.begin(); itf1 != nextf1.end(); ++itf1){
   				for(itf2 = nextf2.begin(); itf2 != nextf2.end(); ++itf2){
   					   std::pair<State, State> > nextstate;
   					   nextstate.first = itf1*;
   					   nextstate.second = itf2*;
   					   if(!states.contains(nextstate)){
   					   		remain.push(nextstate);
   					   		i++;
   					   }

   					   addTransition(laststate, *it, i);
   					}
   				}
   			}
   		} 
}


void Automaton::addToAlphabet(unsigned varnr){

}

void Automaton::complement(Automaton& fa){

}

void Automaton::project(const unsigned variable){

}

void Automaton::makeDeterministic(Automaton& fa){
  std::set<State> Q, d, F, remain;
  std::set<State> S = fa1->initialStates;
  std::set<State> remain = S;

  while(!remain.isEmpty()){

  }
  std::map<BitVector, std::set<State> > toState;

	remain = initialStates; 
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


