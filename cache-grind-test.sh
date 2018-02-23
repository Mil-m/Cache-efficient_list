#!/bin/bash

function grind_test
{
    valgrind --log-fd=1 --tool=cachegrind --LL=65536,8,64 --D1=4096,8,64 ./test |tee -a log.txt |awk '/LL misses/{ gsub( /\,|\(|\)/, "", $0 ); print $5 "; " $8; }'
}

function run_test
{
    CC="gcc -o test -O3 -g -static -DNMIN=$1 -DNMAX=$2"
    echo -e "\n\n$CC -DSKIP_TIME -DSKIP_SEARCH $3 -lrt\n" >>log.txt
    $CC -DSKIP_TIME -DSKIP_SEARCH "$3" -lrt
    S1="$1"
    S2=$(grind_test)
    echo -e "\n\n$CC -DSKIP_TIME $3 -lrt\n" >>log.txt
    $CC -DSKIP_TIME "$3" -lrt
    S3=$(grind_test)
    rm -f cachegrind.out*
    rm -f test
    echo "$S1; $S2; $S3"
}

[ -f log.txt ] && rm -f log.txt
[ -f "$1" ] && SrcFile="$1" || SrcFile="First-Functions.c"

echo "Count; Read Misses; Write Misses; Read Misses; Write Misses"
echo ";Insert+Delete; Insert+Delete;Insert+Search+Delete; Insert+Search+Delete"
run_test 0 1 "$SrcFile"
for ((i=100;i<20000;i+=i/5)) do
    ((j=$i+1))
    run_test $i $j "$SrcFile"
done
