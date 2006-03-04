#!/bin/sh

dst=$PWD/scum-`date +%y%m%d%H%M`
rm -rf $dst $dst.iso $dst.dmg
mkdirhier $dst/bin $dst/SCUM
cp ./src/scum $dst/bin/scum
fltk-config --post $dst/bin/scum
svn export ./lib $dst/SCUM/lib
svn export ./help $dst/SCUM/help
hdiutil makehybrid -o $dst.iso $dst
hdiutil convert -format UDRO -o $dst.dmg $dst.iso
rm -rf $dst $dst.iso
