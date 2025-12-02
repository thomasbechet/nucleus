#!/bin/bash

function usage {
  echo "usage: $0 compile_commands_file"
  exit 1
}

if [ $# -ne 1 ] || [ -z $1 ];
then
  usage 
  exit
fi

# replace -I to -isystem (ensure import using brackets)
sed -i 's/\-I/\-isystem/g' $1
# no need to add NU_IMPL, it is in .clangd
