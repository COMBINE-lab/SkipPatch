#!/bin/bash

#run updates benchmark for human sampled and size of search str = 30

#generate jelly file
time echo ">FF" | cat - /mnt/scratch3/nirm/data/ecoli.fa | jellyfish count -m 30 -s 100M -o /mnt/scratch3/nirm/data/jellyfish_data/ecoli.jf /dev/stdin &

#create query file
time /home/nitish/SkipPatch/scripts/genquery.py --genome /mnt/scratch3/nirm/data/ecoli.fa --jellyfile /mnt/scratch3/nirm/data/jellyfish_data/ecoli_30.jf --totedits 46410 --meanindel 5 --insprob 0.5 --delprob 0.5 --queryfreq 4641 --querycount 5000 --output /mnt/scratch3/nirm/data/search_queries/ecoli_q_30 -k 30

python /home/nitish/SkipPatch/scripts/run.py -SABinary "/home/nitish/SkipPatch/dyn_sa/src/test" -o "/mnt/scratch2/nirm/results/benchmarks_nitish/" -l "/mnt/scratch2/nirm/results/benchmarks_nitish/" -rc '-g /mnt/scratch2/nirm/data/human.fa -e /mnt/scratch2/nirm/data/update_number/human_5' -c "human_updates_sampling_16" -mem



#run all bemchmarks
#python verifyCorrectnessWithCLI.py -SPBinary "/home/nitish/SkipPatch/src/main" -SABinary "/home/nitish/SkipPatch/dyn_sa/src/test" -o "/mnt/scratch2/nirm/output_genomes/" -l "/mnt/scratch2/nirm/output_genomes/" -rc '-g /mnt/scratch2/nirm/data/ecoli.fa -e /home/komal/data/update_number/ecoli_0.01_10 -n 10 -s /home/komal/data/substring/ecoli_substr_10000 -q /home/komal/data/search_queries/ecoli_q_10 -qf 10 -qc 10 -it 1'

python /home/nitish/SkipPatch/scripts/run.py -SPBinary "/home/nitish/SkipPatch/src/main" -SABinary "/home/nitish/SkipPatch/dyn_sa/src/test" -o "/mnt/scratch2/nirm/results/nitish/" -l "/mnt/scratch2/nirm/results/nitish" -rc '-g /mnt/scratch2/nirm/data/ecoli.fa -e /mnt/scratch2/nirm/data/update_number/ecoli_0.01 -n 464 -lh /mnt/scratch2/nirm/hash -sh /mnt/scratch2/nirm/hash'

#debugging skip patch
python /home/nitish/SkipPatch/scripts/run.py -SPBinary "/home/nitish/SkipPatch/src/main" -l "/mnt/scratch2/nirm/results/nitish" -rc '-g /mnt/scratch2/nirm/data/ecoli.fa -e /mnt/scratch2/nirm/data/update_number/ecoli_0.01 -n 1 -t'

#memory analysis
#python verifyCorrectnessWithCLI.py -SPBinary "/home/nitish/SkipPatch/src/main" -SABinary "/home/nitish/SkipPatch/dyn_sa/src/test" -l "/mnt/scratch2/nirm/output_genomes/" -rc '-g /mnt/scratch2/nirm/data/ecoli.fa -e /home/komal/data/update_number/ecoli_0.01 -n 464' -mem

#python verifyCorrectnessWithCLI.py -SABinary "/home/nitish/SkipPatch/dyn_sa/src/test" -o "/mnt/scratch2/nirm/output_genomes/" -l "/mnt/scratch2/nirm/output_genomes/" -rc '-g /mnt/scratch2/nirm/data/ecoli.fa -e /home/komal/data/update_number/ecoli_0.01_10 -n 10 -s /home/komal/data/substring/ecoli_substr_10000 -q /home/komal/data/search_queries/ecoli_q_10 -qf 10 -qc 10 -it 1'
