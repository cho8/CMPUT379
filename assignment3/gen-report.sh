#!/bin/bash
program=$1
numints=$2
reportname="average_$program_$numints.csv"

psizes=(64 128 256 512)
wsizes=(128 256 512 1024 2048 4096 8192 16384)

rm -f $reportname

if [ $# -ne 2 ]; then
    echo "Usage: $0 <program-name> <number-ints>"
    exit 1
fi

for p in "${psizes[@]}"; do
    for w in "${wsizes[@]}"; do
        result=$(echo $numints | ./sim $p $w $program | grep "execution: " | sed 's/^.*: //')
        printf "$p,$w,$result\r\n" >> $reportname
    done
done
