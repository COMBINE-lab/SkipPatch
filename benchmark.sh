 python testmedian2.py --genome "/mnt/scratch3/nirm/data/fruitfly.fa" --totedits 172573 --meanindel 5 --insprob 0.5 --delprob 0.5 --output "./update_size/fruit_5.txt" &
./main /mnt/scratch3/nirm/data/fruitfly.fa ../scripts/update_size/fruit_5.txt /mnt/scratch3/nirm/results/update_size/fruit_5.fa 142573 >/mnt/scratch3/nirm/results/update_size/fruit_5 &

python testmedian2.py --genome "/mnt/scratch3/nirm/data/fruitfly.fa" --totedits 172573 --meanindel 10 --insprob 0.5 --delprob 0.5 --output "./update_size/fruit_10.txt" &
./main /mnt/scratch3/nirm/data/fruitfly.fa ../scripts/update_size/fruit_10.txt /mnt/scratch3/nirm/results/update_size/fruit_10.fa 142573 >/mnt/scratch3/nirm/results/update_size/fruit_10 &

python testmedian2.py --genome "/mnt/scratch3/nirm/data/fruitfly.fa" --totedits 172573 --meanindel 25 --insprob 0.5 --delprob 0.5 --output "./update_size/fruit_25.txt" &
./main /mnt/scratch3/nirm/data/fruitfly.fa ../scripts/update_size/fruit_25.txt /mnt/scratch3/nirm/results/update_size/fruit_25.fa 142573 >/mnt/scratch3/nirm/results/update_size/fruit_25 &



 python testmedian2.py --genome "/mnt/scratch3/nirm/data/ecoli.fa" --totedits 4841 --meanindel 5 --insprob 0.5 --delprob 0.5 --output "./update_size/ecoli_5.txt" &
./main /mnt/scratch3/nirm/data/ecoli.fa ../scripts/update_size/ecoli_5.txt /mnt/scratch3/nirm/results/update_size/ecoli_5.fa 4641 >/mnt/scratch3/nirm/results/update_size/ecoli_5 &

python testmedian2.py --genome "/mnt/scratch3/nirm/data/ecoli.fa" --totedits 4841 --meanindel 10 --insprob 0.5 --delprob 0.5 --output "./update_size/ecoli_10.txt" &
./main /mnt/scratch3/nirm/data/ecoli.fa ../scripts/update_size/ecoli_10.txt /mnt/scratch3/nirm/results/update_size/ecoli_10.fa 4641 >/mnt/scratch3/nirm/results/update_size/ecoli_10 &

python testmedian2.py --genome "/mnt/scratch3/nirm/data/ecoli.fa" --totedits 4841 --meanindel 25 --insprob 0.5 --delprob 0.5 --output "./update_size/ecoli_25.txt" &
./main /mnt/scratch3/nirm/data/ecoli.fa ../scripts/update_size/ecoli_25.txt /mnt/scratch3/nirm/results/update_size/ecoli_25.fa 4641 >/mnt/scratch3/nirm/results/update_size/ecoli_25 &
  

./main /mnt/scratch3/nirm/data/ecoli.fa ../scripts/ecoli_0.01.txt /mnt/scratch3/nirm/results/ecoli_0.01.fa 464 >/mnt/scratch3/nirm/results/ecoli_0.01 &
./main /mnt/scratch3/nirm/data/ecoli.fa ../scripts/ecoli_0.1.txt /mnt/scratch3/nirm/results/ecoli_0.1.fa 4641 >/mnt/scratch3/nirm/results/ecoli_0.1 &
./main /mnt/scratch3/nirm/data/ecoli.fa ../scripts/ecoli_1.txt /mnt/scratch3/nirm/results/ecoli_1.fa 46410 >/mnt/scratch3/nirm/results/ecoli_1 &
./main /mnt/scratch3/nirm/data/ecoli.fa ../scripts/ecoli_0.5.txt /mnt/scratch3/nirm/results/ecoli_0.5.fa 23207 >/mnt/scratch3/nirm/results/ecoli_0.5 &
./main /mnt/scratch3/nirm/data/ecoli.fa ../scripts/ecoli_5.txt /mnt/scratch3/nirm/results/ecoli_5.fa 232070 >/mnt/scratch3/nirm/results/ecoli_5 &


./main /mnt/scratch3/nirm/data/fruitfly.fa ../scripts/fruit_0.01.txt /mnt/scratch3/nirm/results/fruit_0.01.fa 14257 >/mnt/scratch3/nirm/results/fruit_0.01 &
./main /mnt/scratch3/nirm/data/fruitfly.fa ../scripts/fruit_0.1.txt /mnt/scratch3/nirm/results/fruit_0.1.fa 142570 >/mnt/scratch3/nirm/results/fruit_0.1 &
./main /mnt/scratch3/nirm/data/fruitfly.fa ../scripts/fruit_1.txt /mnt/scratch3/nirm/results/fruit_1.fa 1425700 >/mnt/scratch3/nirm/results/fruit_1 &
./main /mnt/scratch3/nirm/data/fruitfly.fa ../scripts/fruit_0.5.txt /mnt/scratch3/nirm/results/fruit_0.5.fa 712850 >/mnt/scratch3/nirm/results/fruit_0.5 &


./main /mnt/scratch3/nirm/data/fruit.fa /home/komal/dynamic-updateable-index/scripts/fruit_q /mnt/scratch3/nirm/results/search/fruit.fa 23207 >/mnt/scratch3/nirm/results/search/fruit &
