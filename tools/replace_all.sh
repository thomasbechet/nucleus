#!/bin/bash

function usage {
  echo "usage: $0 regex action replace"
  exit 1
}

if [ $# -lt 2 ]; 
then
  usage 
  exit
fi

if  [ "$2" == "r" ]; then
    if [ $# -ne 3 ]; then
        echo 'missing replace string'
    else
        find nucleus/ -regex '.*\(h\|c\)' -not -path 'nucleus/external/*' -exec sed -i "s/$1/$3/g" {} \;
    fi
elif [ "$2" == "c" ]; then
    find nucleus/ -regex '.*\(h\|c\)' -not -path 'nucleus/external/*' -exec grep -H $1 {} \;
else
    echo 'unknown action'
fi
