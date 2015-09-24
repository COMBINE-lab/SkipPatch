#!/bin/bash

#run all bemchmarks
#python verifyCorrectnessWithCLI.py -SPBinary "/home/nitish/SkipPatch/src/main" -SABinary "/home/nitish/SkipPatch/dyn_sa/src/test" -o "/mnt/scratch2/nirm/output_genomes/" -l "/mnt/scratch2/nirm/output_genomes/" -rc '-g /mnt/scratch2/nirm/data/ecoli.fa -e /home/komal/data/update_number/ecoli_0.01_10 -n 10 -s /home/komal/data/substring/ecoli_substr_10000 -q /home/komal/data/search_queries/ecoli_q_10 -qf 10 -qc 10 -it 1'


#python verifyCorrectnessWithCLI.py -SPBinary "/home/nitish/SkipPatch/src/main" -SABinary "/home/nitish/SkipPatch/dyn_sa/src/test" -o "/mnt/scratch2/nirm/output_genomes/" -l "/mnt/scratch2/nirm/output_genomes/" -rc '-g /mnt/scratch2/nirm/data/ecoli.fa -e /home/komal/data/update_number/ecoli_0.01 -n 464' 

#memory analysis
python verifyCorrectnessWithCLI.py -SPBinary "/home/nitish/SkipPatch/src/main" -SABinary "/home/nitish/SkipPatch/dyn_sa/src/test" -l "/mnt/scratch2/nirm/output_genomes/" -rc '-g /mnt/scratch2/nirm/data/ecoli.fa -e /home/komal/data/update_number/ecoli_0.01 -n 464' -mem

#python verifyCorrectnessWithCLI.py -SABinary "/home/nitish/SkipPatch/dyn_sa/src/test" -o "/mnt/scratch2/nirm/output_genomes/" -l "/mnt/scratch2/nirm/output_genomes/" -rc '-g /mnt/scratch2/nirm/data/ecoli.fa -e /home/komal/data/update_number/ecoli_0.01_10 -n 10 -s /home/komal/data/substring/ecoli_substr_10000 -q /home/komal/data/search_queries/ecoli_q_10 -qf 10 -qc 10 -it 1'
