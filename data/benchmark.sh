#DYNAMIC HASH
#DIR: /home/komal/dynamic-updateable-index/src
./main < ../../data/ecoli.fa > ../../results/benchmark_ecoli.fa &

./main < ../../data/fruitfly.fa > ../../results/benchmark_fruitfly.fa &
./main < ../../data/fruitfly_sm.fa > ../../results/benchmark_fruitfly_sm.fa &
./main < ../../data/fruitfly_rm.fa > ../../results/benchmark_fruitfly_rm.fa &

./main < ../../data/zebrafish.fa > ../../results/benchmark_zebrafish.fa &
./main < ../../data/zebrafish_sm.fa > ../../results/benchmark_zebrafish_sm.fa &
./main < ../../data/zebrafish_rm.fa > ../../results/benchmark_zebrafish_rm.fa &


#DYNAMIC SUFFIX ARRAY
#DIR: /home/komal/nirm/dynamicSA/src
./testMultiple.sh ../../../data/fruitfly.fa 100 100 1 1 > ../results/benchmark_fruitfly.fa &
./testMultiple.sh ../../../data/fruitfly_sm.fa 100 100 1 1 > ../results/benchmark_fruitfly_sm.fa &
./testMultiple.sh ../../../data/fruitfly_rm.fa 100 100 1 1 > ../results/benchmark_fruitfly_rm.fa &

./testMultiple.sh ../../../data/zebrafish.fa 100 100 1 1 > ../results/benchmark_zebrafish.fa &
./testMultiple.sh ../../../data/zebrafish_sm.fa 100 100 1 1 > ../results/benchmark_zebrafish_sm.fa &
./testMultiple.sh ../../../data/zebrafish_rm.fa 100 100 1 1 > ../results/benchmark_zebrafish_rm.fa &
