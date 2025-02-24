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

unsigned int nondet_uint();
void __CPROVER_assume(int x);
void __CPROVER_assert(int x, char y[]);

#define assert2(x, y) __CPROVER_assert(x, y)
#define assume(x) __CPROVER_assume(x)

/**
 * Amount of distinguishable card symbols.
 */
#ifndef NUM_SYM
#define NUM_SYM 2
#endif

/**
 * Size of input sequence (number of cards including both commitments plus additional cards).
 */
#ifndef N
#define N 4
#endif

 /**
  * Number of all cards used for commitments
  */
#ifndef COMMIT
#define COMMIT 4
#endif

 /**
  * Maximum number of sequences (usually N!).
  * This value can be lowered if there are multiple indistinguishable symbols in the deck.
  * This variable is used for over-approximating loops such that
  * their unrolling bound can be statically determined.
  */
#ifndef NUMBER_POSSIBLE_SEQUENCES
#define NUMBER_POSSIBLE_SEQUENCES 6
#endif

  /**
   * For two players inserting yes or no to a protocol,
   * there are four different possibilities how the protocol could start.
   * For more players or other scenarios this value has to be adapted.
   */
#ifndef NUMBER_START_SEQS
#define NUMBER_START_SEQS 4
#endif


 /**
  * Maximum number of permutations fpr the given number of cards (N!).
  * This value has to be computed by our script, or adjusted manually.
  */
#ifndef NUMBER_POSSIBLE_PERMUTATIONS
#define NUMBER_POSSIBLE_PERMUTATIONS 24
#endif


  /**
   * Regarding possibilities for a sequence, we (only) consider
   * - 0: probabilistic security
   *      (exact possibilities for a sequence)
   * - 1: input possibilistic security (yes or no)
   *      (whether the sequence can belong to the specific input)
   * - 2: output possibilistic security (yes or no)
   *      (to which output the sequence can belong)
   */
#ifndef WEAK_SECURITY
#define WEAK_SECURITY 2
#endif


  /**
   * We always had four input possibilities,
   * this is changed if we only consider output possibilistic security.
   * This variable is used for over-approximating loops such that
   * their unrolling bound can be statically determined.
   */
#if WEAK_SECURITY == 2
#define NUMBER_PROBABILITIES 2
#else
#define NUMBER_PROBABILITIES 4
#endif

   /**
    * This variable is used to limit the permutation set in any shuffle.
    * This can reduce the running time of this program.
    * When reducing this Variable, keep in mind that it could exclude some valid protocols,
    * as some valid permutation sets are not longer considered.
    */
#ifndef MAX_PERM_SET_SIZE
#define MAX_PERM_SET_SIZE NUMBER_POSSIBLE_PERMUTATIONS
#endif

struct fraction {
    unsigned int num; // The numerator.
    unsigned int den; // The denominator.
};

struct fractions {
    struct fraction frac[NUMBER_PROBABILITIES];
};

/**
 * This is one sequence, as seen from left to right.
 *
 * If the sequence can belong to a specific input sequence,
 * then the probabilities entry is set to the probability for this input sequence.
 * Indices:
 * - 0: X_00
 * - 1: X_01
 * - 2: X_10
 * - 3: X_11
 *
 * If the sequence is not contained in the state, all probabilities are set to zero.
 *
 * We save the probabilities as numerator/denominator (of type fraction),
 * so we can avoid floating point operations and divisions.
 *
 * One line looks like this:
 *   val:           [card#1][card#2]..[card#N]
 *   probs:         [num#1]..[num#4]
 *   (num./denom.)  [den#1]..[den#4]
 *
 * For input-possibilistic protocols,
 * we only need to determine whether a sequence can belong to a specific input:
 *    [card#1][card#2]..[card#N]
 *    [bool#1]..[bool#4]
 *
 * For output-possibilistic protocols,
 * we only need to determine whether a sequence can belong to a specific output:
 *    [card#1][card#2]..[card#N]
 *    [bool#1][bool#2]
 * Note that in this scenario, we have bool#1 == X_0 and bool#2 == X_1.
 */
struct sequence {
    unsigned int val[N];
    struct fractions probs;
};


/**
 * All sequences are remembered here, as seen from left to right, sorted alphabetically.
 * The states in this program are equal to the states in KWH trees.
 */
struct state {
    struct sequence seq[NUMBER_POSSIBLE_SEQUENCES];
};

   /**
    * All permutations are remembered here, as seen from left to right, sorted alphabetically.
    */
struct permutationState {
    struct sequence seq[NUMBER_POSSIBLE_PERMUTATIONS];
};

/**
 * We store all possible permutations into a seperate state to save resources.
 */
struct permutationState stateWithAllPermutations;

/**
 * We store one empty state at beginning of the program to save ressources.
 */
struct state emptyState;

/**
 * An integer array with length N.
 */
struct narray {
    unsigned int arr[N];
};

/**
 * An integer array with length NUM_SYM.
 */
struct numsymarray {
    unsigned int arr[NUM_SYM];
};


struct permutationState getStateWithAllPermutations() {
    struct permutationState s;
    for (unsigned int i = 0; i < NUMBER_POSSIBLE_PERMUTATIONS; i++) {
        struct narray taken;
        for (unsigned int j = 0; j < N; j++) {
            taken.arr[j] = 0;
        }
        for (unsigned int j = 0; j < N; j++) {
            s.seq[i].val[j] = nondet_uint();
            unsigned int val = s.seq[i].val[j];
            assume(0 < val && val <= N);
            unsigned int idx = val - 1;
            assume(!taken.arr[idx]);
            taken.arr[idx]++;
        }
    }

    // Not needed, but to avoid state space explosion
    for (unsigned int i = 0; i < NUMBER_POSSIBLE_PERMUTATIONS; i++) {
        for (unsigned int j = 0; j < NUMBER_PROBABILITIES; j++) {
            s.seq[i].probs.frac[j].num = 0;
            s.seq[i].probs.frac[j].den = 1;
        }
    }

    for (unsigned int i = 1; i < NUMBER_POSSIBLE_PERMUTATIONS; i++) {
        unsigned int checked = 0;
        unsigned int last = i - 1;
        for (unsigned int j = 0; j < N; j++) {
            // Check lexicographic order
            unsigned int a = s.seq[last].val[j];
            unsigned int f = s.seq[i].val[j];
            checked |= (a < f);
            assume(checked || a == f);
        }
        assume(checked);
    }
    return s;
}

/**
 * Given an array containing a sequence, we return the index of the given sequence in a state.
 */
unsigned int getSequenceIndexFromArray(struct narray compare, struct state compareState) {
    unsigned int seqIdx = nondet_uint();
    assume(seqIdx < NUMBER_POSSIBLE_SEQUENCES);
    struct sequence seq = compareState.seq[seqIdx];

    for (unsigned int i = 0; i < N; i++) {
        assume(compare.arr[i] == seq.val[i]);
    }
    return seqIdx;
}

/**
 * Update the possibilities of a sequence after a shuffle.
 */
struct fractions recalculatePossibilities(struct fractions probs,
    struct fractions resProbs,
    unsigned int permSetSize) {
    for (unsigned int k = 0; k < NUMBER_PROBABILITIES; k++) {
        struct fraction prob = probs.frac[k];
        unsigned int num = prob.num;
        unsigned int denom = prob.den;

        if (num && WEAK_SECURITY) {
            resProbs.frac[k].num |= num;
        }
        else if (num) {
            /**
             * Only update fractions in case we are in the
             * strong security setup.
             */
             // Update denominator.
            resProbs.frac[k].den = denom * permSetSize;
            // Update numerator.
            resProbs.frac[k].num = (num * permSetSize) + denom;
        }
    }
    return resProbs;
}

/**
 * Calculate the state after a shuffle operation starting from s with the given permutation set.
 * 
 * Deleted isStillPossible
 */
struct state doShuffle(struct state s,
    unsigned int permutationSet[MAX_PERM_SET_SIZE][N],
    unsigned int permSetSize) {
    struct state res = emptyState;
    // For every sequence in the input state.
    for (unsigned int i = 0; i < NUMBER_POSSIBLE_SEQUENCES; i++) {
            // For every permutation in the permutation set.
            for (unsigned int j = 0; j < MAX_PERM_SET_SIZE; j++) {
                if (j < permSetSize) {
                    struct narray resultingSeq = { .arr = { 0 } };
                    for (unsigned int k = 0; k < N; k++) {
                        // Apply permutation j to sequence i.
                        resultingSeq.arr[permutationSet[j][k]] = s.seq[i].val[k];
                    }
                    unsigned int resultSeqIndex = // Get the index of the resulting sequence.
                        getSequenceIndexFromArray(resultingSeq, res);
                    // Recalculate possibilities.
                    res.seq[resultSeqIndex].probs =
                        recalculatePossibilities(s.seq[i].probs,
                            res.seq[resultSeqIndex].probs,
                            permSetSize);
                }
            }
    }
    return res;
}

struct state applyShuffle(struct state s) {
    // Generate permutation set (shuffles are assumed to be uniformly distributed).
    unsigned int permSetSize = nondet_uint();
    assume(0 < permSetSize && permSetSize <= MAX_PERM_SET_SIZE);

    unsigned int permutationSet[MAX_PERM_SET_SIZE][N] = { 0 };
    unsigned int takenPermutations[NUMBER_POSSIBLE_PERMUTATIONS] = { 0 };
    /**
     * Choose permSetSize permutations nondeterministically. To achieve this,
     * generate a nondeterministic permutation index and get the permutation from this index.
     * No permutation can be chosen multiple times.
     */
    unsigned int lastChosenPermutationIndex = 0;
    for (unsigned int i = 0; i < MAX_PERM_SET_SIZE; i++) {
        if (i < permSetSize) { // Only generate permutations up to permSetSize.
            unsigned int permIndex = nondet_uint();
            // This ensures that the permutation sets are sorted lexicographically.
            assume(lastChosenPermutationIndex <= permIndex);
            assume(permIndex < NUMBER_POSSIBLE_PERMUTATIONS);
            assume(!takenPermutations[permIndex]);

            takenPermutations[permIndex] = 1;
            lastChosenPermutationIndex = permIndex;

            for (unsigned int j = 0; j < N; j++) {
                permutationSet[i][j] = stateWithAllPermutations.seq[permIndex].val[j] - 1;
                /**
                 * The '-1' is important. Later, we convert to array indices such as
                 * array[permutationSet[x][y]]. Without the '-1', we would get out-
                 * of-bound errors there.
                 */
            }
        }
    }
    struct state res = doShuffle(s, permutationSet, permSetSize);
    return res;
}



/**
 * Constructor for states. Only use this to create new states.
 */
struct state getEmptyState() {
    struct state s;
    struct numsymarray symbolCount;
    for (unsigned int i = 0; i < NUM_SYM; i++) {
        symbolCount.arr[i] = 0;
    }

    for (unsigned int i = 0; i < NUMBER_POSSIBLE_SEQUENCES; i++) {
        struct numsymarray taken;
        for (unsigned int j = 0; j < NUM_SYM; j++) {
            taken.arr[j] = 0;
        }
        for (unsigned int j = 0; j < N; j++) {
            s.seq[i].val[j] = nondet_uint();
            unsigned int val = s.seq[i].val[j];
            assume(0 < val && val <= NUM_SYM);
            unsigned int idx = val - 1;
            taken.arr[idx]++;
            assume(taken.arr[idx] <= N - 2); // At least two symbols have to be different. Players cannot commit otherwise.
        }
        for (unsigned int j = 0; j < NUM_SYM; j++) {
            if (i == 0) {
                symbolCount.arr[j] = taken.arr[j];
            }
            else { // We ensure that every sequence consists of the same symbols
                assume(taken.arr[j] == symbolCount.arr[j]);
            }
        }

        // Here we store the numerators and denominators
        for (unsigned int j = 0; j < NUMBER_PROBABILITIES; j++) {
            s.seq[i].probs.frac[j].num = 0;
            s.seq[i].probs.frac[j].den = 1;
        }
    }

    for (unsigned int i = 1; i < NUMBER_POSSIBLE_SEQUENCES; i++) {
        unsigned int checked = 0;
        unsigned int last = i - 1;
        for (unsigned int j = 0; j < N; j++) {
            // Check lexicographic order
            unsigned int a = s.seq[last].val[j];
            unsigned int f = s.seq[i].val[j];
            checked |= (a < f);
            assume(checked || a == f);
        }
        assume(checked);
    }
    return s;
}


/**
 * One bit is represented by two cards, a and b.
 * If the first card is lower than the second card, the bit represents the value "0"
 * If the first card is higher than the second card, the bit represents the value "1"
 * Note that if both cards are equal, the bit is "undefined".
 * This must not happen in our implementation, but must be considered for multiple
 * indistinguishable cards.
 */
unsigned int isZero(unsigned int a, unsigned int b) {
    return a < b;
}

/**
 * See description of isZero(uint, uint) above.
 */
unsigned int isOne(unsigned int a, unsigned int b) {
    return a > b;
}


/**
 * Determine if a sequence in the start state belongs to the input possibility (0 0).
 */
unsigned int isZeroZero(unsigned int arr[N]) {
    return isZero(arr[0], arr[1]) && isZero(arr[2], arr[3]);
}

/**
 * Determine if a sequence in the start state belongs to the input possibility (0 1).
 */
unsigned int isZeroOne(unsigned int arr[N]) {
    return isZero(arr[0], arr[1]) && isOne(arr[2], arr[3]);
}

/**
 * Determine if a sequence in the start state belongs to the input possibility (1 0).
 */
unsigned int isOneZero(unsigned int arr[N]) {
    return isOne(arr[0], arr[1]) && isZero(arr[2], arr[3]);
}

/**
 * Determine if a sequence in the start state belongs to the input possibility (1 1).
 */
unsigned int isOneOne(unsigned int arr[N]) {
    return isOne(arr[0], arr[1]) && isOne(arr[2], arr[3]);
}

/**
 * This method constructs the start sequence for a given commitment length COMMIT
 * using nodeterministic assignments. We only consider the case where Alice uses
 * the cards "1" and "2", and Bob uses the cards "3" and "4".
 */
struct narray getStartSequence() {
    assume(N >= COMMIT); // We assume at least as many cards as needed for the commitments.
    struct numsymarray taken;
    for (unsigned int i = 0; i < NUM_SYM; i++) {
        taken.arr[i] = 0;
    }
    struct narray res;
    for (unsigned int i = 0; i < COMMIT; i++) {
        res.arr[i] = nondet_uint();
        unsigned int val = res.arr[i];
        assume(0 < val && val <= COMMIT && val <= NUM_SYM);
        unsigned int idx = val - 1;
        assume(taken.arr[idx] < COMMIT / NUM_SYM);
        taken.arr[idx]++;
    }
    // Here we assume that each player only uses fully distinguishable cards
    assume(res.arr[1] != res.arr[0]);
    assume(res.arr[3] != res.arr[2]);
    for (unsigned int i = COMMIT; i < N; i++) {
        res.arr[i] = nondet_uint();
        assume(0 < res.arr[i]);
        assume(res.arr[i] <= NUM_SYM);
    }
    return res;
}

/**
* This function performs a shuffle and afterwards checks for a specific property of the probabilities
* in this test it is, whether all probabilities in all possible sequences have a value that is not equal to 0
* a correct result needs at least 6 permutations
* therefore the problem is complicated enough to ensure some level of complexity while keeping the code simple
* For other tests, this function can be easiy altered
*/
struct state tryPermutation(struct state s) {
    struct state res = applyShuffle(s);

    // check if every possibility is 1 after shuffle
    for (int i = 0; i < NUMBER_POSSIBLE_SEQUENCES; i++) {
        for (int j = 0; j < NUMBER_PROBABILITIES; j++) {
            assume(res.seq[i].probs.frac[j].num != 0);
        }
    }
    return s;
}

int main() {

    emptyState = getEmptyState();
    struct state startState = emptyState;
    struct narray start[NUMBER_START_SEQS];
    for (unsigned int i = 0; i < NUMBER_START_SEQS; i++) {
        start[i] = getStartSequence();
    }

    assume(isZeroZero(start[0].arr));
    assume(NUMBER_START_SEQS == 4);
    assume(start[0].arr[0] == start[1].arr[0]);
    assume(start[1].arr[0] != start[2].arr[0]);
    assume(start[2].arr[0] == start[3].arr[0]);

    assume(start[0].arr[2] == start[2].arr[2]);
    assume(start[0].arr[2] != start[1].arr[2]);
    assume(start[1].arr[2] == start[3].arr[2]);

    unsigned int arrSeqIdx[NUMBER_START_SEQS];
    for (unsigned int i = 0; i < NUMBER_START_SEQS; i++) {
        arrSeqIdx[i] = getSequenceIndexFromArray(start[i], startState);
    }

    if (WEAK_SECURITY == 2) {
        for (unsigned int i = 0; i < (NUMBER_START_SEQS - 1); i++) {
        startState.seq[arrSeqIdx[i]].probs.frac[0].num = 1;
        }
    
        unsigned int lastStartSeq = NUMBER_START_SEQS - 1;
        unsigned int arrIdx = arrSeqIdx[lastStartSeq];
        unsigned int lastProbIdx = NUMBER_PROBABILITIES - 1;
        startState.seq[arrIdx].probs.frac[lastProbIdx].num = isOneOne(start[lastStartSeq].arr);
    }
    else {
        for (unsigned int i = 0; i < (NUMBER_START_SEQS); i++) {
            startState.seq[arrSeqIdx[i]].probs.frac[i].num = 1;
        }
    }


    stateWithAllPermutations = getStateWithAllPermutations();

    tryPermutation(startState);
    assert(0);
    return 0;
}