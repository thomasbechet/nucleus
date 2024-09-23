import os
import re

included_files = set()
line_count = 0
output = open('nucleus.h', 'w')

def process_file(path):
    global line_count
    if path in included_files:
        return
    included_files.add(path)
    with open(path) as file:
        for line in file.readlines():
            m = re.match("\\#include\\s*\\<(.*)\\>", line)
            if m:
                child_file = m.group(1)
                if child_file.startswith('nucleus'):
                    process_file(child_file)
                    continue
            line.strip()
            output.write(line)
            line_count += 1

process_file('nucleus/nucleus.h')
print('line count:', line_count)
