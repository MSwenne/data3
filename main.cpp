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


Automaton createAutomaton(ExprTree * exprtree){
    Automaton theAuto;
    std::queue<State> states;
    std::set<State>::iterator statesit;
    std::map<unsigned,int> pres;
    std::map<unsigned,int>::iterator presit;
    std::set<BitVector> bits;
    std::set<BitVector>::iterator bititSet;
    BitVector bitcurr;
    std::map<unsigned, bool>::iterator bitit;
    State statecurr;
    int b = 0, nextb = 0, checkb = 0, state = 0;
    
   
   
    if(exprtree->getPresburgerMap(pres, b)){
    	theAuto.addState(b);
    	theAuto.markInitial(b);
    	theAuto.markFinal(b);
    	states.push(b);
    	nextb = b;
        std::cout << "b BABLABLA = " << b << std::endl;
        theAuto.makeBitSet(pres.size(), bits);
        bititSet = bits.begin();
        bitcurr = *bititSet;
        for(presit = pres.begin(); presit != pres.end(); ++presit){
            std::cout << "aaa" << presit->first << "bbb" << presit->second << endl;
        }
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
                std::cout << "end bitit || b = " << b << " checkb " << checkb << std::endl;
            	std::cout << " checkb mod 2 " << checkb%2 << std::endl;
                if(std::abs(checkb%2) == b%2){
                    state = (b-checkb)/2;
                   if(!theAuto.hasState(state)){
                        std::cout << "ADDING " << state << std::endl;
                        theAuto.addState(state);
                        states.push(state);
                    }
                    std::cout << "Trans " << statecurr << " To " << state << std::endl; 
                    theAuto.addTransition(statecurr, bitcurr, state);
                }
            }
        }
    }
    else{
        std::cout << "Wrong expression tree." << std::endl;
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

