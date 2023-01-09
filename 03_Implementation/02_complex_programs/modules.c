// Copyright (C) 2020 Michael Kirsten, Michael Schrempp, Alexander Koch

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program; if not, <http://www.gnu.org/licenses/>.

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "findTwoCardProtocolADDERwithModules.c"

unsigned int nondet_uint();
void __CPROVER_assume(int x);
void __CPROVER_assert(int x, char y[]);

#define assert2(x, y) __CPROVER_assert(x, y)
#define assume(x) __CPROVER_assume(x)


/**
* MODULES:
* The maximum number of possible result states a protocol can have.
* This is defined for all possibly used protocols because protocolStates needs to have a fixed size
* it is currently defined as 2, because the protocol with the most endstates has 2 endstates
*/
#ifndef MAX_PROTOCOL_ENDSTATES
#define MAX_PROTOCOL_ENDSTATES 2
#endif

/**
* whether the protcol
* AND by Takaaki Mizuki and Hideaki Sone (2009) -> Finite Runtime, 6 cards, 2 steps
* (https://doi.org/10.1007/978-3-642-02270-8_36)
* is used (0: not used, 1: used)
*/
#ifndef USE_FR_AND
#define USE_FR_AND 1
#endif 

/**
* AND by Takaaki Mizuki and Hideaki Sone (2009) -> Finite Runtime, 6 cards, 2 steps
* (https://doi.org/10.1007/978-3-642-02270-8_36)
*/
#ifndef FR_AND
#define FR_AND 0
#endif 


/**
* whether the protcol
* XOR by Takaaki Mizuki and Hideaki Sone(2009) -> Finite Runtime, 4 cards, 2 steps
* (https://doi.org/10.1007/978-3-642-02270-8_36)
* is used (0: not used, 1: used)
*/
#ifndef USE_FR_XOR
#define USE_FR_XOR 1
#endif 

/**
* XOR by Takaaki Mizuki and Hideaki Sone(2009) -> Finite Runtime, 4 cards, 2 steps
* (https://doi.org/10.1007/978-3-642-02270-8_36)
*/
#ifndef FR_XOR
#define FR_XOR 1
#endif 

/**
* whether the protcol
* AND by Alexander Koch, Michael Schrempp and Michael Kirsten (2021) -> Las Vegas, 5 cards, 5 steps
* (https://doi.org/10.1007/s00354-020-00120-0)
* is used (0: not used, 1: used)
*/
#ifndef USE_LV_AND
#define USE_LV_AND 0
#endif 

/**
* AND by Alexander Koch, Michael Schrempp and Michael Kirsten (2021) -> Las Vegas, 5 cards, 5 steps
* (https://doi.org/10.1007/s00354-020-00120-0)
*/
#ifndef LV_AND
#define LV_AND 2
#endif 


/**
* whether the protcol
* OR by Anne Hoff -> Las Vegas, 4 cards, 6 steps
* (https://github.com/a-nne-h/automatedApproachToGeneratingCardProtocols)
* is used (0: not used, 1: used)
*/
#ifndef USE_LV_OR
#define USE_LV_OR 0
#endif 

/**
* OR by Anne Hoff -> Las Vegas, 4 cards, 6 steps
* (https://github.com/a-nne-h/automatedApproachToGeneratingCardProtocols)
*/
#ifndef LV_OR
#define LV_OR 3
#endif 

/**
* whether the protcol
* COPY by Takaaki Mizuki and Hideaki Sone (2009) with fixed amount of copies = 1 -> Finite Runtime, 6 cards, 2 steps
* (https://doi.org/10.1007/978-3-642-02270-8_36)
* is used(0: not used, 1 : used)
*/
#ifndef USE_FR_COPY
#define USE_FR_COPY 0
#endif 

/**
* COPY by Takaaki Mizuki and Hideaki Sone (2009) with fixed amount of copies = 1 -> Finite Runtime, 6 cards, 2 steps
* (https://doi.org/10.1007/978-3-642-02270-8_36)
*/
#ifndef FR_COPY
#define FR_COPY 4
#endif 



/**
* TODO: we need all possible endstates here, not just one
* therefore the struct PROTOCOL_TABLE should have PROTOCOL_TABLE[5][MAX_PROTOCOL_ENDSTATES][4][6]
* and the FR_AND_TABLE & co schould be FR_AND_TABLE[MAX_PROTOCOL_ENDSTATES][4]
*/
 const unsigned int VAL_111222[6] = {1,1,1,2,2,2};
 const unsigned int VAL_112122[6] = { 1,1,2,1,2,2 };
 const unsigned int VAL_112212[6] = { 1,1,2,2,1,2 };
 const unsigned int VAL_112221[6] = { 1,1,2,2,2,1 };
 const unsigned int VAL_121122[6] = { 1,2,1,1,2,2 };
 const unsigned int VAL_121212[6] = { 1,2,1,2,1,2 };
 const unsigned int VAL_121221[6] = { 1,2,1,2,2,1 };
 const unsigned int VAL_122112[6] = { 1,2,2,1,1,2 };
 const unsigned int VAL_122121[6] = { 1,2,2,1,2,1 };
 const unsigned int VAL_122211[6] = { 1,2,2,2,1,1 };
 const unsigned int VAL_211122[6] = { 2,1,1,1,2,2 };
 const unsigned int VAL_211212[6] = { 2,1,1,2,1,2 };
 const unsigned int VAL_211221[6] = { 2,1,1,2,2,1 };
 const unsigned int VAL_212112[6] = { 2,1,2,1,1,2 };
 const unsigned int VAL_212121[6] = { 2,1,2,1,2,1 };
 const unsigned int VAL_212211[6] = { 2,1,2,2,1,1 };
 const unsigned int VAL_221112[6] = { 2,2,1,1,1,2 };
 const unsigned int VAL_221121[6] = { 2,2,1,1,2,1 };
 const unsigned int VAL_221211[6] = { 2,2,1,2,1,1 };
 const unsigned int VAL_222111[6] = { 2,2,2,1,1,1 };

 const unsigned int VAL_12122[6] = { 1,2,1,2,2,0 };
 const unsigned int VAL_12212[6] = { 1,2,2,1,2,0 };
 const unsigned int VAL_21212[6] = { 2,1,2,1,2,0 };
 const unsigned int VAL_21122[6] = { 2,1,1,2,2,0 };

 const unsigned int VAL_1212[6] = { 1,2,1,2,0,0 };
 const unsigned int VAL_2112[6] = { 2,1,1,2,0,0 };
 const unsigned int VAL_2121[6] = { 2,1,2,1,0,0 };
 const unsigned int VAL_1221[6] = { 1,2,2,1,0,0 };
 const unsigned int VAL_2211[6] = { 2,2,1,1,0,0 };

 const unsigned int VAL_PLACEHOLDER[6] = { 0,0,0,0,0,0 };

 const unsigned int FR_AND_TABLE[2][4][6] = { { VAL_121212, VAL_122112,VAL_121212, VAL_121221 }, {VAL_211212,VAL_211221, VAL_211212, VAL_212112} };
 const unsigned int FR_XOR_TABLE[2][4][6] = { { VAL_1212, VAL_2112, VAL_2112, VAL_1212 }, {VAL_2121, VAL_1221, VAL_1221, VAL_2121} };
 const unsigned int LV_AND_TABLE[2][4][6] = { { VAL_12212, VAL_12212, VAL_12212, VAL_12122 }, {VAL_21122, VAL_21122, VAL_21122, VAL_21212} };
 const unsigned int LV_OR_TABLE[2][4][6] = { { VAL_2211, VAL_1212, VAL_1212, VAL_1212 }, {VAL_2121, VAL_2112, VAL_2112, VAL_2112 } };
 const unsigned int FR_COPY_TABLE[2][4][6] = { { VAL_121212, VAL_PLACEHOLDER, VAL_122121, VAL_PLACEHOLDER }, {VAL_212121, VAL_PLACEHOLDER, VAL_211212, VAL_PLACEHOLDER} };
 const unsigned int PROTOCOL_TABLE[5][2][4][6] = { FR_AND_TABLE, FR_XOR_TABLE, LV_AND_TABLE, LV_OR_TABLE, FR_COPY_TABLE };

/**
* NOT does not have to be a protocol, becaue it is nothing else than a perm operation which is already included
* Whether NOT is used -> Finite Runtime, 2 cards, 1 steps
*/



/**
* MODULES:
* Analog to turn states, this struct is used to retun arrays of states after a protocol operation.
* There is one state for each possible end state (resut state) of the protocol
* In each usage of a protocol, for each sequence the resulting sequences in each end state are calculated and stored in states.
* isUsed[i] contains if the corresponding state[i] holds a end state or isn't used
*
*/
struct protocolStates {
    struct state states[2];
    unsigned int isUsed[MAX_PROTOCOL_ENDSTATES];
};


/**
* MODULES:
* finds the index of a given sequence (as an array) within a state.
*/
unsigned  int findIndex(struct sequence seq) {
    unsigned int index = nondet_uint();
    for (int j = 0; j < N; j++) {
        assume(seq.val[j] != emptyState.seq[index].val[j]);
    }
    return index;
}

/**
* MODULES:
* searches for the endSequence in result.states[resultIdx]
* if found, copy the probabilities/possibilities from seq to result.states[resultIdx] and return new result
*/
struct protocolStates copyResults(struct sequence seq, struct protocolStates result, unsigned int resultIdx) {

    //find index of sequence within state that matches endSequence
    unsigned int index = findIndex(seq);

    // copy the probabilities/possibilities from seq to result.states[resultIdx] (! add the values -> cr shuffle)
    for (unsigned int j = 0; j < NUMBER_PROBABILITIES; j++) {
        struct fraction prob = seq.probs.frac[j];
        // Copy numerator.
        if (prob.num != 0) {
            result.states[resultIdx].seq[index].probs.frac[j].num = prob.num;
        }
        if (!WEAK_SECURITY) { // Probabilistic security
            // Copy denominator.
            result.states[resultIdx].seq[index].probs.frac[j].den = prob.den;
        }
    }
    return result;
}


struct protocolStates doProtocols(unsigned int protocolChosen, struct state s, unsigned int com1A, unsigned int com1B, unsigned int com2A, unsigned int com2B, unsigned int help1, unsigned int help2) {
    struct protocolStates result;
    // Initialise N empty states.
    for (unsigned int i = 0; i < MAX_PROTOCOL_ENDSTATES; i++) {
        result.states[i] = emptyState;
        result.isUsed[i] = 0;
    }

    for (unsigned int i = 0; i < MAX_PROTOCOL_ENDSTATES; i++) {
        for (unsigned int j = 0; j < NUMBER_POSSIBLE_SEQUENCES; j++) {
            struct sequence seq = s.seq[j];
            unsigned int idx = 0;
            if (isZero(seq.val[com1A], seq.val[com1B])) {
                if (isZero(seq.val[com2A], seq.val[com2B])) {
                    // 0101  
                    idx = 0;
                }
                else if (isOne(seq.val[com2A], seq.val[com2B])) {
                    // 0110
                    idx = 1;
                }
            }
            else if (isOne(seq.val[com1A], seq.val[com1B])) {
                if (isZero(seq.val[com2A], seq.val[com2B])) {
                    // 1001
                    idx = 2;
                }
                else if (isOne(seq.val[com2A], seq.val[com2B])) {
                    // 1010
                    idex = 3;
                }
            }

            seq.val[com1A] = PROTOCOL_TABLE[protocolChosen][i][idx][0];
            seq.val[com1B] = PROTOCOL_TABLE[protocolChosen][i][idx][1];
            seq.val[com2A] = PROTOCOL_TABLE[protocolChosen][i][idx][2];
            seq.val[com2B] = PROTOCOL_TABLE[protocolChosen][i][idx][3];
            // if we have one (or more) helper card
            if (protocolChosen == FR_AND || protocolChosen == FR_COPY
                || protocolChosen == LV_AND || protocolChosen == LV_OR) {
                seq.val[help1] = PROTOCOL_TABLE[protocolChosen][i][idx][4];
                // if we have two helper cards
                if (protocolChosen == FR_AND || protocolChosen == FR_COPY) {
                    seq.val[help2] = PROTOCOL_TABLE[protocolChosen][i][idx][5];
                }
            }
            result = copyResults(seq, result, i);
            result.isUsed[0] = 1;
        }
    }
        for (unsigned int l = 0; l < MAX_PROTOCOL_ENDSTATES; l++) {
            assume(isBottomFree(result.states[l]));
        }
        return result;
    }

    

    struct protocolStates applyProtocols(struct state s) {
        // check that the chosen protocol is actually 'allowed'
        unsigned int protocolChosen = nondet_uint();
        assume(protocolChosen >= 0 && protocolChosen < 5);
        if (USE_FR_AND == 0) {
            assume(protocolChosen != FR_AND);
        }
        if (USE_FR_XOR == 0) {
            assume(protocolChosen != FR_XOR);
        }
        if (USE_LV_AND == 0) {
            assume(protocolChosen != LV_AND);
        }
        if (USE_LV_OR == 0) {
            assume(protocolChosen != LV_OR);
        }
        if (USE_FR_COPY == 0) {
            assume(protocolChosen != FR_COPY);
        }

        // create resulting states
        struct protocolStates resultingStates;

        for (unsigned int i = 0; i < MAX_PROTOCOL_ENDSTATES; i++) {
            resultingStates.states[i] = emptyState;
            resultingStates.isUsed[i] = 0;
        }


        // pick 4 cards that represent the two commitments
        unsigned int com1A = nondet_uint();
        unsigned int com1B = nondet_uint();
        unsigned int com2A = nondet_uint();
        unsigned int com2B = nondet_uint();
        assume(com1A < N&& com1B < N&& com2A < N&& com2B < N);
        assume(com1A != com1B && com1A != com2A && com1A != com2B);
        assume(com1B != com2A && com1B != com2B);
        assume(com2A != com2B);

        unsigned int help1 = 0;
        unsigned int help2 = 0;

        for (unsigned int i = 0; i < NUMBER_POSSIBLE_SEQUENCES; i++) {
            // if the probability/possibility of this state is not 0 
            if (isStillPossible(s.seq[i].probs)) {
                // check that througout every possible sequence in the state we have chosen two different cards for our commitments
                assume(s.seq[i].val[com1A] != s.seq[i].val[com1B]);
                assume(s.seq[i].val[com2A] != s.seq[i].val[com2B]);
            }
        }

        // for copy we only have two commitments and four help cards
        if (protocolChosen == FR_COPY) {
            for (unsigned int i = 0; i < NUMBER_POSSIBLE_SEQUENCES; i++) {
                // if the probability/possibility of this state is not 0 
                if (isStillPossible(s.seq[i].probs)) {
                    // check that helper cards are the same all throughout every possible sequence in the state
                    assume(isZero((s.seq[i].val[com2A]), s.seq[i].val[com2B]));
                }
            }
        }
        //protocols with five cards
        if (protocolChosen == LV_AND || protocolChosen == LV_OR) {
            help1 = nondet_uint();
            assume(help1 < N);
            assume(help1 != com1A && help1 != com1B && help1 != com2A && help1 != com2B);
            for (unsigned int i = 0; i < NUMBER_POSSIBLE_SEQUENCES; i++) {
                // if the probability/possibility of this state is not 0 
                if (isStillPossible(s.seq[i].probs)) {
                    // check that helper cards are the same all throughout every possible sequence in the state
                    if (protocolChosen == LV_AND) {
                        // for LV_AND the helper card is 2
                        assume(s.seq[i].val[help1] == 2);
                    }
                    else if (protocolChosen == LV_OR) {
                        // for LV_OR the helper card is 1
                        assume(s.seq[i].val[help1] == 1);
                    }
                }
            }
        }
        if (protocolChosen == FR_AND || protocolChosen == FR_COPY) {
            help1 = nondet_uint();
            help2 = nondet_uint();
            assume(help1 < N&& help2 < N);
            assume(help1 != com1A && help1 != com1B && help1 != com2A && help1 != com2B);
            assume(help2 != com1A && help2 != com1B && help2 != com2A && help2 != com2B);
            for (unsigned int i = 0; i < NUMBER_POSSIBLE_SEQUENCES; i++) {
                // if the probability/possibility of this state is not 0 
                if (isStillPossible(s.seq[i].probs)) {
                    // check that helper cards are the same all throughout every possible sequence in the state
                    assume(isZero((s.seq[i].val[help1]), s.seq[i].val[help2]));
                }
            }
        }

        resultingStates = doProtocols(protocolChosen, s, com1A, com1B, com2A, com2B, help1, help2);

        //as with TURN, choose one output nondeterministically to look at further
        unsigned int stateIdx = nondet_uint();
        assume(stateIdx < MAX_PROTOCOL_ENDSTATES);
        assume(resultingStates.isUsed[stateIdx]);
        return resultingStates.states[stateIdx];
    }