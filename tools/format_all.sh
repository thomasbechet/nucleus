#!/bin/bash

find nucleus/ -regex '.*\(h\|c\)' -not -path 'nucleus/external/*' -exec clang-format -i {} \;
