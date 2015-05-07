#!/bin/bash

##### E.Coli

for len in {25,35,70,100}
do 
echo "Performing 10 searches in ecoli of length "$len >> ../data/benchmarking_ecoli 
./locate ../data/reference_ecoli ../data/ecoli$len 10 $len >> ../data/benchmarking_ecoli
done

echo "Performing 1000 updates in ecoli" >> benchmarking_ecoli 
./testMultiple.sh ../data/reference_ecoli 1000 1000 1 1 >> ../data/benchmarking_ecoli


##### Fruit Fly

for len in {25,35,70,100}
do 
echo "Performing 10 searches in fruitfly of length "$len >> ../data/benchmarking_fruitfly 
./locate ../data/reference_fruitfly ../data/fruitfly$len 10 $len >> ../data/benchmarking_fruitfly
done

echo "Performing 1000 updates in fruitfly" >> benchmarking_fruitfly 
./testMultiple.sh ../data/reference_fruitfly 1000 1000 1 1 >> ../data/benchmarking_fruitfly


