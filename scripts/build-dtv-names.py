#!/usr/bin/env python
#-----------------------------------------------------------------------------
#
#  TSDuck - The MPEG Transport Stream Toolkit
#  Copyright (c) 2005-2023, Thierry Lelegard
#  BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/#license
#
#  Build the file tsduck.dtv.names, containing names for Digital TV.
#  Syntax: build-dtv-names-names.py out-file in-file-or-dir ...
#
#-----------------------------------------------------------------------------

import tsbuild, sys, os, fnmatch

# Process a .names file or a directory.
def process_file_or_dir(path, output):
    if os.path.isfile(path):
        print('#---------- ' + os.path.basename(path), file=output)
        print('', file=output)
        with open(path, 'r', encoding='utf-8') as input:
            output.write(input.read())
        print('', file=output)
    elif os.path.isdir(path):
        for name in os.listdir(path):
            subpath = path + os.sep + name
            if os.path.isdir(subpath) or fnmatch.fnmatch(name, '*.names'):
                process_file_or_dir(subpath, output)

# Main code.
if __name__ == '__main__':
    if len(sys.argv) < 3:
        print('Usage: %s out-file in-file-or-dir ...' % sys.argv[0], file=sys.stderr)
        exit(1)
    with open(sys.argv[1], 'w', encoding='utf-8') as output:
        tsbuild.write_source_header('#', 'Registered names for Digital TV', file=output)
        for i in range(2, len(sys.argv)):
            process_file_or_dir(sys.argv[i], output)
