#!/bin/bash

function usage {
  echo "usage: $0 shader_directory"
  exit 1
}

if [ $# -ne 1 ] || [ -z $1 ];
then
  usage 
  exit
fi

printf "#ifndef NUGL_SHADERS_H\n"
printf "#define NUGL_SHADERS_H\n"

printf "#include <nucleus/core/config.h>\n"

for file in $1/*; do
    name="nugl__shader_$(basename $file | tr '.' '_')" 
    printf "static const nu_char_t *$name = \n"
    while IFS= read -r line; do
        # if [[ -z $line ]]; then
        #     continue 
        # fi
        printf "\"$line\\\n\"\n"
    done < $file
    printf ";\n"
done

printf "#endif\n"

