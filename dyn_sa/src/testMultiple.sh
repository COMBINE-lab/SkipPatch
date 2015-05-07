#!/bin/bash

if [ $# -ne 5 -a $# -ne 6 ]
then
  echo "Usage: $0 [lcp] <text> <nb_ins> <nb_del> <min length pattern> <max length pattern>" >&2
	exit 1
fi

LCP=
if [ $# -eq 6 ] 
then
  if [ "$1" == "lcp" ]
	then
	  LCP=$1
	fi
	shift
fi

TEXT=$1
NB_PATTERNS=$2
NB_DEL=$3
MIN_LENGTH_PATTERN=$4
MAX_LENGTH_PATTERN=$5
FILESIZE=`wc -c $TEXT | awk '{print $1}'`

DIFF_LENGTH=$((MAX_LENGTH_PATTERN-MIN_LENGTH_PATTERN+1))
COMMAND_LINE="./test $TEXT $LCP insert"
NEW_LENGTH=$FILESIZE
for (( i=0; $i < $NB_PATTERNS; i++ ))
do
  RANDOM_LENGTH=$((RANDOM*RANDOM%DIFF_LENGTH+MIN_LENGTH_PATTERN))
	LENGTH=$((RANDOM*RANDOM%FILESIZE))
	if [ $LENGTH -le $RANDOM_LENGTH ]
	then
	  LENGTH=$RANDOM_LENGTH
	fi
	PATTERNS[$i]=`tempfile`
	NEW_LENGTH=$((NEW_LENGTH+RANDOM_LENGTH))
	head -c $LENGTH $TEXT | tail -c $RANDOM_LENGTH > ${PATTERNS[$i]}
	COMMAND_LINE="$COMMAND_LINE "${PATTERNS[$i]}" "$((RANDOM*RANDOM%FILESIZE))" "
done

COMMAND_LINE="$COMMAND_LINE delete"
for ((i=0; $i<$NB_DEL; i++))
do
  RANDOM_LENGTH=$((RANDOM*RANDOM%DIFF_LENGTH+MIN_LENGTH_PATTERN))
	RANDOM_POS=$((RANDOM*RANDOM%(NEW_LENGTH-RANDOM_LENGTH-1)))
	COMMAND_LINE="$COMMAND_LINE $RANDOM_POS $RANDOM_LENGTH"
	NEW_LENGTH=$((NEW_LENGTH-RANDOM_LENGTH))
done

echo "Executing $COMMAND_LINE" >&2
RES="`$COMMAND_LINE 2>&1`"
EXITCODE=$?
#EXITCODE=1
if [ $EXITCODE -ne 0 ]
then
  echo "Code sortie : $EXITCODE" >&2
else
  echo -e "$RES"
fi

for (( i=0; $i < $NB_PATTERNS; i++ ))
do
  if [ $EXITCODE -eq 0 ]
	then
    rm ${PATTERNS[$i]}
	fi
done
