#!/bin/sh
set -eu
stem=man-1.6g
dir=$stem/man2html
shasum -c $stem.shasum
for file in abbrev.c cgibase.c defs.h man2html.1 man2html.c strdefs.c
do
    tar -xf $stem.tar.gz $dir/$file
    mv $dir/$file $file
done
rmdir $dir
rmdir $stem
patch <version.patch
