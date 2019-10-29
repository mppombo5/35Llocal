#!/usr/local/bin/bash

echo "$# arguments were passed into script $0"

if [ $1 == $2 ]
then
    echo "the two arguments are the same"
elif [ $1 -gt $2 ]
then
    echo "arg 1 is greater than arg 2"
else
    echo "arg 2 is greater than arg 1"
fi
