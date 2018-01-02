#!/bin/sh
# Tee to file $1 and exit with code of given command

PIPE=$(mktemp -u)
rm -f $PIPE
mkfifo $PIPE
tee $1 < $PIPE &
shift 1
$@ 1>$PIPE 2>$PIPE
exit $?
