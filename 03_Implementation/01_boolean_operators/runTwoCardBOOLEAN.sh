#!/bin/bash 
 
# Copyright (C) 2020 Michael Kirsten, Michael Schrempp, Alexander Koch 
 
#    This program is free software; you can redistribute it and/or modify 
#    it under the terms of the GNU General Public License as published by 
#    the Free Software Foundation; either version 3 of the License, or 
#    (at your option) any later version. 
 
#    This program is distributed in the hope that it will be useful, 
#    but WITHOUT ANY WARRANTY; without even the implied warranty of 
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
#    GNU General Public License for more details. 
 
#    You should have received a copy of the GNU General Public License 
#    along with this program; if not, <http://www.gnu.org/licenses/>. 
 
START=$(date +'%Y-%m-%d %H:%M:%S %Z') 
START_PRINT=`echo -e "$START" | sed -e 's/\s/\_/g' | sed -e 's/\-/\_/g' | sed -e 's/:/\_/g'` 
START_SEC=$(date +%s) 
TIMESTAMP="# Timestamp: "$START 
CBMC='./cbmc' 
HOST=`echo -e $(hostname)` 
TRACE_OPTS='--compact-trace --trace-hex' 
TIMEOUT="5d" 
OPERATOR=$1 
N=$2 
LENGTH=$3 
OPT=$4 
NUM_SYM='2' # This is the setting where all cards carry only two distinct symbols 
OUTFILE="twoCardProtocol"$OPERATOR"_n"$N"_l"$LENGTH"__"$HOST"_"$START_PRINT".out" 
SHORTFILE="shortTwoCardProtocol"$OPERATOR"_n"$N"_l"$LENGTH"__"$START_PRINT".out" 
 
OPTS='' 
while [ -n "$4" ] 
do 
        OPTS=$OPTS" ${4}" && shift; 
done 
PRINT_OPTS='' 
if [ -n "$OPTS" ] 
then 
    printOpts=`echo -e "?$OPTS" | sed -e 's/?\s--//g' | sed -e 's/?\s-D\s//g' | sed -re 's/([a-zA-Z0-9]+)=([0-9]+)/\1 = \2/g' | sed -e 's/\s-D\s/, /g'` 
    printOpts=`echo -e "$printOpts" | sed 's/\s\(.*\)--/\1 and /' | sed 's/--/, /g'` 
    PRINT_OPTS=`echo -e "$printOpts" | sed -e 's/and/,/g' | sed 's/[[:space:]]*,/,/g'` 
fi 
if [ -n "$OPTS" ] 
then 
    OPTIONS='\n'"# Further Options: "$PRINT_OPTS 
else 
    OPTIONS='' 
fi 
 
CBMC_BINARY=${CBMC#"./"} 
if ! [ -x "$(command -v $CBMC_BINARY)" ] && [ ! -f $CBMC ] 
then 
        echo -e $CBMC_BINARY" is not a valid cbmc binary. Now terminating." 
        exit 
fi 
if [ ! -f $CBMC ] 
then 
    CBMC=$CBMC_BINARY 
fi 
VERSION="# CBMC Version: "$($CBMC -version) 
 
if [[ $N == "" ]] || (( "$N" <= "0" )) 
then 
    echo -e "No valid card number specified. Now terminating." 
    exit 
fi 
 
FOUR='4' # To be changed when we support more card numbers 
if [ "$N" -lt $FOUR ] 
then 
    echo -e "Program only supports a minimum of "$FOUR" cards, you entered "$N". Now terminating." 
    exit 
fi 
 
EIGHT='8' # To be changed when we support more card numbers 
if [ "$N" -gt $EIGHT ] 
then 
    echo -e "Program only supports a maximum of "$EIGHT" cards, as we do not know the number of subgroups for greater numbers. You entered the value "$N". Now terminating." 
    exit 
fi 
 
TWO='2' # Decks with only one distinguishable card are kind of senseless 
if [ "$NUM_SYM" -lt $TWO ] 
then 
    echo -e "Program only supports a minimum number of two distinct card symbols. You entered the value "$NUM_SYM" for distinct symbols. Now terminating." 
    exit 
fi 
 
# Decks with more distinguishable cards than total cards can probably be represented in some other way using less distinguishable cards. 
if [ "$NUM_SYM" -gt $N ] 
then 
    echo -e "Program only supports a number of possible distinct cards which equals at most the total number of cards. You entered the value "$NUM_SYM" for distinct symbols, where there are only "$N" cards in total. Now terminating." 
    exit 
fi 
 
if [[ $LENGTH == "" ]] || (( "$LENGTH" <= "0" )) 
then 
    echo -e "No valid protocol length specified. Now terminating." 
    exit 
fi 
 
FILE="" 
if [[ $OPERATOR == "AND" ]] 
then 
    FILE="findTwoCardProtocolAND.c" 
elif [[ $OPERATOR == "OR" ]] 
then 
    FILE="findTwoCardProtocolOR.c" 
elif [[ $OPERATOR == "XOR" ]] 
then 
    FILE="findTwoCardProtocolXOR.c" 
elif [[ $OPERATOR=="COPY" ]] 
then 
    FILE="findTwoCardProtocolCOPY.c" 
fi 
 
 
if [ ! -f $FILE ] 
then 
    echo -e $FILE" is not a valid file. Now terminating." 
    exit 
fi 
 
fact () 
{ 
  local number=$1 
  #  Variable "number" must be declared as local, 
  #+ otherwise this doesn't work. 
  if [ "$number" -eq 0 ] 
  then 
    factorial=1    # Factorial of 0 = 1. 
  else 
    let "decrnum = number - 1" 
    fact $decrnum  # Recursive function call (the function calls itself). 
    let "factorial = $number * $?" 
  fi 
 
  return $factorial 
} 
 
 
NOM='0' 
DENOM='1' 
VAL=$[$N / $NUM_SYM] 
fact $VAL 
FOO=$factorial 
BOUND=$[$NUM_SYM - 1] 
 
for i in $(eval echo "{1..$BOUND}") 
do 
    NOM=$[$NOM + $VAL] 
    DENOM=$[$DENOM * $FOO] 
done 
 
REST=$[$N - $NOM] 
fact $REST 
FOO=$factorial 
DENOM=$[$DENOM * $FOO] 
 
fact $N 
FOO=$factorial 
 
POS_SEQ=$[$FOO / $DENOM] 
POS_SEQ_STRING="NUMBER_POSSIBLE_SEQUENCES" 
POS_SEQ_MINUS_ONE=$[$POS_SEQ -1] 
N_TIMES_TWO=$[$N*2] 
POS_PERM=$FOO 
POS_PERM_STRING="NUMBER_POSSIBLE_PERMUTATIONS" 
 
NUMBER_CLOSED_SHUFFLES=(0 1 2 6 30 156 1455 11300 151221) 
PERM_SET_SIZE="${NUMBER_CLOSED_SHUFFLES[$N]}" 
 
FIVE='5' # To be changed when we support more card numbers 
THREE='3' 
SUBGROUP_SIZES="" 
 
# if we had something like this: 
# Hard-coded values, look up at https://groupprops.subwiki.org/wiki/Subgroup_structure_of_symmetric_group:S5 
# * (We could even omit the largest number here, even) 
# * S5_subgroup_sizes = {1, 2, 3, 4, 5, 6, 8, 10, 12, 20, 24, 60, 120} // leave out 1, 120 
# * S4_subgroup_sizes = {1, 2, 3, 4, 6, 8, 12, 24} // leave out 1, 24 
# * S3_subgroup_sizes = {1, 2, 3, 6} // leave out 1, 6 
# * we could check for permSetSize being equal to one of the numbers in the list 
if [ "$N" -gt $FIVE ] 
then 
    NUMBER_SUBGROUP_SIZES='0' 
elif [ "$N" -eq $FIVE ] 
then 
    NUMBER_SUBGROUP_SIZES='11' # We can leave out 1 and 120 
    SUBGROUP_SIZES=$SUBGROUP_SIZES" -D SUBGROUP_SIZE_1=2 -D SUBGROUP_SIZE_2=3 -D SUBGROUP_SIZE_3=4 -D SUBGROUP_SIZE_4=5" 
    SUBGROUP_SIZES=$SUBGROUP_SIZES" -D SUBGROUP_SIZE_5=6 -D SUBGROUP_SIZE_6=8 -D SUBGROUP_SIZE_7=10 -D SUBGROUP_SIZE_8=12" 
    SUBGROUP_SIZES=$SUBGROUP_SIZES" -D SUBGROUP_SIZE_9=20 -D SUBGROUP_SIZE_10=24 -D SUBGROUP_SIZE_11=60 " 
elif [ "$N" -eq $FOUR ] 
then 
    NUMBER_SUBGROUP_SIZES='6' # We can leave out 1 and 24 
    SUBGROUP_SIZES=$SUBGROUP_SIZES" -D SUBGROUP_SIZE_1=2 -D SUBGROUP_SIZE_2=3 -D SUBGROUP_SIZE_3=4 " 
    SUBGROUP_SIZES=$SUBGROUP_SIZES" -D SUBGROUP_SIZE_4=6 -D SUBGROUP_SIZE_5=8 -D SUBGROUP_SIZE_6=12 " 
elif [ "$N" -eq $THREE ] 
then 
    NUMBER_SUBGROUP_SIZES='2' # We can leave out 1 and 6 
    SUBGROUP_SIZES=$SUBGROUP_SIZES" -D SUBGROUP_SIZE_1=2 -D SUBGROUP_SIZE_2=3 " 
else 
    NUMBER_SUBGROUP_SIZES='0' 
fi 
 
echo -e '\n'"############################################################" 2>&1 | tee $OUTFILE | tee $SHORTFILE 
echo -e '# HOST: '$HOST'\n'$TIMESTAMP'\n'$VERSION$OPTIONS 2>&1 | tee -a $OUTFILE | tee $SHORTFILE 
echo -e "# N = "$N", NUM_SYM = "$NUM_SYM", L = "$LENGTH", NUMBER_POSSIBLE_PERMUTATIONS = "$POS_PERM", NUMBER_POSSIBLE_SEQUENCES = "$POS_SEQ" TIMEOUT = "$TIMEOUT 2>&1 | tee -a $OUTFILE | tee -a $SHORTFILE 
echo -e "############################################################" 2>&1 | tee -a $OUTFILE | tee -a $SHORTFILE 
echo -e '\n'"############################################################"'\n' 2>&1 | tee -a $OUTFILE | tee -a $SHORTFILE 
timeout $TIMEOUT $CBMC $TRACE_OPTS -D L=$LENGTH -D N=$N -D NUM_SYM=$NUM_SYM -D $POS_SEQ_STRING=$POS_SEQ -D $POS_PERM_STRING=$POS_PERM -D PERM_SET_SIZE=$PERM_SET_SIZE -D NUMBER_SUBGROUP_SIZES=$NUMBER_SUBGROUP_SIZES $SUBGROUP_SIZES $FILE $OPTS 2>&1 | tee -a $OUTFILE 
END=$(date +'%Y-%m-%d %H:%M:%S %Z') 
END_SEC=$(date +%s) 
FINAL_TIMESTAMP="# Final Time: "$END 
DIFF=$(( $END_SEC - $START_SEC )) 
 
 
#SHUFFLE 
grep -n -A $POS_SEQ_MINUS_ONE "applyShuffle(" $OUTFILE | tee -a $SHORTFILE 
grep -n -E 'permSetSize' $OUTFILE | tee -a $SHORTFILE 
grep -n -A $N_TIMES_TWO "lastChosenPermutationIndex" $OUTFILE | tee -a $SHORTFILE 
 
#TURN 
grep -n -A $POS_SEQ_MINUS_ONE "Turn(" $OUTFILE | tee -a $SHORTFILE 
grep -n -E 'turnPosition' $OUTFILE | tee -a $SHORTFILE 
grep -n -A $POS_SEQ "stateIdx" $OUTFILE | tee -a $SHORTFILE 
 
#RESULT (only for two bit result) 
grep -n -B 1 ' a=' $OUTFILE | tail -1 | tee -a $SHORTFILE 
grep -n -B 1 ' b=' $OUTFILE | tail -1 | tee -a $SHORTFILE 
grep -n -A 1 ' c=' $OUTFILE | tail -1 | tee -a $SHORTFILE 
grep -n -A 1 ' c=' $OUTFILE | tail -1 | tee -a $SHORTFILE 
 
python manipulateShortInput.py "shortTwoCardProtocol"$OPERATOR"_n"$N"_l"$LENGTH"__"$START_PRINT".out" | tee -a $OUTFILE 
 
 
 
echo -e '\n'"############################################################" 2>&1 | tee -a $OUTFILE | tee -a $SHORTFILE 
echo -e $FINAL_TIMESTAMP 2>&1 | tee -a $OUTFILE | tee -a $SHORTFILE 
echo -e "# It took $DIFF seconds." 2>&1 | tee -a $OUTFILE | tee -a $SHORTFILE 
echo -e "############################################################" 2>&1 | tee -a $OUTFILE | tee -a $SHORTFILE