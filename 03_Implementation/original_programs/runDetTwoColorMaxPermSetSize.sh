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
FILE="determineMaxPermSetSize.c"
HOST=`echo -e $(hostname)`
OUTFILE="detTwoColPermedStates_"$HOST"_"$START_PRINT".out"
TRACE_OPTS='--compact-trace --trace-hex'
TIMEOUT="5d"
N=$1
MINPERMS=$2
LENGTH='1'
OPT=$3
NUM_SYM='2' # This is the setting where all cards carry only two distinct symbols

OPTS=''
while [ -n "$3" ]
do
    OPTS=$OPTS" ${3}" && shift;
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
FOO=$?
BOUND=$[$NUM_SYM - 1]

for i in $(eval echo "{1..$BOUND}")
do
    NOM=$[$NOM + $VAL]
    DENOM=$[$DENOM * $FOO]
done

REST=$[$N - $NOM]
fact $REST
FOO=$?
DENOM=$[$DENOM * $FOO]

fact $N
FOO=$?

POS_SEQ=$[$FOO / $DENOM]
POS_SEQ_STRING="NUMBER_POSSIBLE_SEQUENCES"

POS_PERM=$FOO
POS_PERM_STRING="NUMBER_POSSIBLE_PERMUTATIONS"

NUMBER_CLOSED_SHUFFLES=(0 1 2 6 30 156 1455 11300 151221)
PERM_SET_SIZE="${NUMBER_CLOSED_SHUFFLES[$N]}"

echo -e '\n'"############################################################" 2>&1 | tee $OUTFILE
echo -e $TIMESTAMP'\n'$VERSION$OPTIONS 2>&1 | tee -a $OUTFILE
echo -e "# N = "$N", NUM_SYM = "$NUM_SYM", L = "$LENGTH", MIN_PERM_SET_SIZE="$MINPERMS", TIMEOUT = "$TIMEOUT 2>&1 | tee -a $OUTFILE
echo -e "############################################################" 2>&1 | tee -a $OUTFILE
echo -e '\n'"############################################################"'\n' 2>&1 | tee -a $OUTFILE
timeout $TIMEOUT $CBMC $TRACE_OPTS -D L=$LENGTH -D N=$N -D NUM_SYM=$NUM_SYM -D $POS_SEQ_STRING=$POS_SEQ -D $POS_PERM_STRING=$POS_PERM -D PERM_SET_SIZE=$PERM_SET_SIZE -D MIN_PERM_SET_SIZE=$MINPERMS $FILE $OPTS 2>&1 | tee -a $OUTFILE
END=$(date +'%Y-%m-%d %H:%M:%S %Z')
END_SEC=$(date +%s)
FINAL_TIMESTAMP="# Final Time: "$END
DIFF=$(( $END_SEC - $START_SEC ))
echo -e '\n'"############################################################" 2>&1 | tee -a $OUTFILE
echo -e $FINAL_TIMESTAMP 2>&1 | tee -a $OUTFILE
echo -e "# It took $DIFF seconds." 2>&1 | tee -a $OUTFILE
echo -e "############################################################" 2>&1 | tee -a $OUTFILE
