#!/bin/bash
git add README.md Makefile
git add *.cpp *.hpp
cd cascades
git add *
cd ..
git commit
git push -f origin master