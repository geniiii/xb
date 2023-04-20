@echo off

set wildcard=*.h *.c *.inc

@echo TODOs found:
findstr -s -n -i -l "TODO(" %wildcard%
