#!/usr/bin/env sh
ls -ARl | grep -E '^[d-][rwxSsTt-]{9}[[:space:]]+.+' | sort -g -r -k 5 | awk 'BEGIN {fcnt = 0; dcnt = 0; bigcnt = 0; totalsize = 0} $1~/^d.+/ {dcnt += 1} $1~/^-.+/ {fcnt += 1; totalsize += $5; if (bigcnt < 5) {bigcnt += 1; print bigcnt ": " $5 " " $9}} END {print "Directory count: " dcnt; print "File count: " fcnt; print "Total size: " totalsize}'
