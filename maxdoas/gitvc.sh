#!/bin/sh
echo "#ifndef vc_h" > vc.h
echo "#define vc_h" >> vc.h
git log -1 --format="format:#define GITHASH \"0x%h\"" >> vc.h
echo "\n" >> vc.h
git log -1 --date=short --format="format:#define GITDATE ""\"%ad\"""" >> vc.h
echo "\n" >> vc.h
echo "#endif" >> vc.h

