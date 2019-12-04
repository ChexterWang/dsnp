#!/bin/bash
printf '' &> out/out.csv
for i in {1..10}
do
   ./cirTest -f tests/do1 &> out/out1
   ./ref/cirTest-linux18 -f tests/do1 &> out/out1.ref
   cat -n out/out1 | sed -n '4p' | awk '{print $6}' | tr -d '\n' &>> out/out.csv
   printf ',' &>> out/out.csv
   cat -n out/out1.ref | sed -n '4p' | awk '{print $6}' | tr -d '\n' &>> out/out.csv
   printf '\n' &>> out/out.csv
done
