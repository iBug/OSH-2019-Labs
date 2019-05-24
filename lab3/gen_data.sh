#!/bin/sh

if [ $# -eq 0 ]; then
  echo "Arguments?" >&2
  exit 1
fi

DATA_ROOT=data
EXIT=0

while [ $# -ne 0 ]; do
  file="$DATA_ROOT/$1"
  if ! [ -r "$file" ]; then
    echo "Bad file $1" >&2
    EXIT=1
    continue
  fi

  xxd -i "$file" > "$file".c
  awk '/^unsigned/{print "extern", $1, $2, $3 ";"}' "$file".c > "$file".h
  shift
done

exit $EXIT
