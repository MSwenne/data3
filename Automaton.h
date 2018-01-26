#ifndef AUTOMATON_H
#define AUTOMATON_H

/**
 * Prototype of the class Automaton
 * Yenebeb Schwarze (s1908928) & Martijn Swenne (s1923889)
 * Datum: 5 Jan 2018
 * Datastructuren Assignment 3
**/

#include <algorithm>
#include <map>
#include <set>
#include <list>
#include <queue>
#include <iostream>
//#include "exprtree.h"

typedef std::map<unsigned, bool> BitVector;
typedef int State;


class Automaton {
	public:
		/**
		* sets alphabet to newAlphabet
		*/
		void setAlphabet(std::set<unsigned> newAlphabet);

		/**
		* checks if automaton has the state
		*/
		bool hasState(State state);


		/**
		* resets current automaton
		*/
		void reset();

		/**
		* Add the given state (an integer) to the automaton
		*/
		void addState(const State state);

		/**
		* Add transition with the given label (a bitvector) between State from and State to.
		* If this is the first transition added to the automaton: initialize the field alphabet with all variables
		* in BitVector label (i.e., the domain of the BitVector map)
		*/
		void addTransition(const State from, const BitVector label, const State to);

		/**
		* Mark the given state as an initial state
		*/
		void markInitial(const State state);

		/**
		* Mark the given state as a final state
		*/
		void markFinal(const State state);

		/**
		* Read the list of bitvectors and move to the next states according to the transitions,
		* starting from the initial states
		*/
		void parseInput(const std::list<BitVector> input);

		/**
		* Return whether the current states contain a final state
		*/
		bool inFinalState() const;

		/**
		* Compute intersection of automata fa1 and fa2 and store the result in the current automaton
		* (implementation calculates cartesian product of automata)
		*/
		void intersect(Automaton& fa1, Automaton& fa2);

		/**
		* Makes a map with a pair combination of all the initial states of fa1 and fa2 and adds a
		* corresponding new state value to every state
		*/
		int setInitial(Automaton& fa1, Automaton& fa2, std::map<std::pair<State, State>, int>& stateMap, std::queue<std::pair<State, State> >& remain);

		/**
		* Makes a set of BitVectors which contains every possible BitVector you can make with the 
		* given alphabet (so you can loop over every BitVector)
		*/
		void makeBitSet(std::set<BitVector>& result);

		/**
		* a recursive helper function for makeBitSet
		*/
		void makeBitSet_p(std::set<unsigned>::iterator it, BitVector bv, std::set<BitVector>& result);
	
		/**
		* Replace every bitvector in the transitions with two new bitvectors that additionally contain the new variable varnr:
		* 1. a transition where variable varnr has value 0
		* 2. a transition where variable varnr has value 1
		* The values for the other variables are unchanged from the original bitvector.
		* Finally, add the given variable to the alphabet.
		* If the variable was already part of the alphabet: do nothing.
		*/
		void addToAlphabet(unsigned varnr);

		/**
		* OPTIONEEL (voor bonus)!!!
		* Compute the complement of the current automaton
		* (for non-deterministic automata: uses makeDeterministic() )
		*/
		void complement(Automaton& fa);

		/**
		* OPTIONEEL (voor bonus)!!!
		* Remove the variable with the given index from all transitions of this automaton
		*/
		void project(const unsigned variable);
                
		/**
		* OPTIONEEL (voor bonus)!!!
		* Make automata fa deterministic, eliminate epsilon transitions, and store the result in the current automaton
		*/
		void makeDeterministic(Automaton& fa);

		/**
		* checks of checkStates intersects with finalStates
		*/
		bool check_intersect(std::set<State> checkStates);

		/**
		* Prints the current automaton to the specified stream
		*/
		void print(std::ostream &str) const;

	private:
		/**
		* Read a single BitVector as input and move from the set of current states to the set of next states
		*/
		void next(const BitVector input);

		/**
		* Eliminate epsilon transitions, and store the result in the current automaton
		*/
		void eliminateLambda(Automaton& fa);

		/**
		* Prints the set of states s to the specified stream
		*/
		static void printStates(std::ostream &str, const std::set<State> s);

		/**
		* Prints the BitVector t to the specified stream
		*/
		static void printTransitionLabel(std::ostream &str, const BitVector t);

		// the set of all states
		std::set<State> states;

		// the set of initial states
		std::set<State> initialStates;
                
		// the set of final states
		std::set<State> finalStates;
                
		// the map containing all transitions of the automton
		std::map<State, std::map<BitVector, std::set<State> > > transitions;

		// the set of current states
		std::set<State> currentStates;

		// the set of variables used in transitions
		std::set<unsigned> alphabet;
};

#endif
