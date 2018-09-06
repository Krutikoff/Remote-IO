#!/usr/bin/env python
# -*- coding: utf-8 -*-

import subprocess
import re
import sys
import os.path
import os

module = 'Remote_IO'
filename = 'version'

out_path = sys.argv[1]

file_create = os.path.normpath(os.path.join(out_path, filename))
print('\nVersion file: {}.h'.format(file_create))

p = subprocess.Popen('git describe --always --dirty=+ --abbrev=5 &&'
                     'git symbolic-ref --short HEAD',
                     stdout=subprocess.PIPE, shell=True)

git_resp = p.communicate()[0].decode().split('\n')
commit_hash = git_resp[0].split('-')[-1]
git_version = git_resp[1].split('/')[-1]

version_str = git_version + "__" + commit_hash

# If not release then get a tag instead of branch name
if git_resp[1].find("release/") == -1:
    version_str = git_resp[0].split('-')[0] + "__" + commit_hash

print("Current version {}".format(version_str))

Write_files = [
    [file_create+'.h',
     '#ifndef ' + module + '_VERSION_H\r\n' +
     '#define ' + module + '_VERSION_H\r\n' +
     'namespace version {\r\n' +
     'static constexpr const char* ' + 'STRING ="' + version_str + '";\r\n' +
     '}\r\n' +
     '#endif\r\n']
    ]

for file in Write_files:
    try:
        f = open(file[0], 'r')
        f.seek(0)
        fileString = f.read()
        f.close()
    except IOError as e:
        fileString = ""
    # Check for differences and update if required
    if(fileString != file[1]):
        print('Version Generator: updating ' + file[0] + ' file')
        f = open(file[0], 'w+')
        f.seek(0)
        f.write(file[1])
        f.truncate()
        f.close()
