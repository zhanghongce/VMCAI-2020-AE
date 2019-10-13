#!/usr/bin/python
import os
import argparse

def AddPrefix(fileName, prefix):
    print 'processing', fileName

    def GetUpdatedInclude(line):
        if '#include "' in line:
            terms = line.split('"')[1].split('/')
            projDirs = ['ila', 'util', 'backend', 'verilog-in', 'verilog-out', 'json-inout', 'python-api', 'cpp_api.h']

            if terms[0] in projDirs:
                newLine = '#include <' + 'ilang'
                for term in terms:
                    newLine = newLine + '/' + term
                newLine = newLine + '>\n'
                return newLine
            
        return line

    fileBuff = []

    with open(fileName, 'r') as rFile:
        for line in rFile:
            newLine = GetUpdatedInclude(line)
            fileBuff.append(newLine)

    with open(fileName, 'w') as wFile:
        for line in fileBuff:
            print >> wFile, line,

def AddPrefixRecursive(path, prefix):
    rootDir = path
    for dirName, subdirList, fileList in os.walk(rootDir):
        for fname in fileList:
            filePath = os.path.join(dirName, fname)
            AddPrefix(filePath, prefix)

def ChangeNameSpace(src, dst, fileName):
    print 'processing', fileName

    def GetNameSpace(line):
        if 'namespace' in line and src in line:
            return line.replace(src, dst)
        return line

    fileBuff = []

    with open(fileName, 'r') as rFile:
        for line in rFile:
            newLine = GetNameSpace(line)
            fileBuff.append(newLine)

    with open(fileName, 'w') as wFile:
        for line in fileBuff:
            print >> wFile, line,

def ChangeNameSpaceRecursive(src, dst, path):
    rootDir = path
    for dirName, subdirList, fileList in os.walk(rootDir):
        for fname in fileList:
            filePath = os.path.join(dirName, fname)
            ChangeNameSpace(src, dst, filePath)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Rename include path with prefix')
    parser.add_argument('-pref', type = str, help = 'Prefix to preppend', default = '')
    parser.add_argument('-file', type = str, help = 'File name for renaming', default = '')
    parser.add_argument('-path', type = str, help = 'Path for recursive process', default = '')
    parser.add_argument('-space_src', type = str, help = 'Namespace source', default = '')
    parser.add_argument('-space_dst', type = str, help = 'Namespace destination', default = '')
    args = parser.parse_args()

    if args.pref and args.path and os.path.exists(args.path):
        AddPrefixRecursive(args.path, args.pref)

    if args.pref and args.file and os.path.exists(args.file):
        AddPrefix(args.file, args.pref)

    if args.space_src and args.space_dst and os.path.exists(args.path):
        ChangeNameSpaceRecursive(args.space_src, args.space_dst, args.path)

