#!/bin/bash

# * 'dna' - unmasked genomic DNA sequences.
#  * 'dna_rm' - masked genomic DNA.  Interspersed repeats and low
#     complexity regions are detected with the RepeatMasker tool and masked
#     by replacing repeats with 'N's.
#  * 'dna_sm' - soft-masked genomic DNA. All repeats and low complexity regions
#    have been replaced with lowercased versions of their nucleic base

wget ftp://ftp.ensembl.org/pub/release-79/fasta/drosophila_melanogaster/dna/Drosophila_melanogaster.BDGP6.dna.toplevel.fa.gz
wget ftp://ftp.ensembl.org/pub/release-79/fasta/drosophila_melanogaster/dna/Drosophila_melanogaster.BDGP6.dna_sm.toplevel.fa.gz
wget ftp://ftp.ensembl.org/pub/release-79/fasta/drosophila_melanogaster/dna/Drosophila_melanogaster.BDGP6.dna_rm.toplevel.fa.gz
wget ftp://ftp.ensembl.org/pub/release-79/fasta/danio_rerio/dna/Danio_rerio.Zv9.dna.toplevel.fa.gz
wget ftp://ftp.ensembl.org/pub/release-79/fasta/danio_rerio/dna/Danio_rerio.Zv9.dna_sm.toplevel.fa.gz
wget ftp://ftp.ensembl.org/pub/release-79/fasta/danio_rerio/dna/Danio_rerio.Zv9.dna_rm.toplevel.fa.gz

gzip -d Drosophila_melanogaster.BDGP6.dna.toplevel.fa.gz
gzip -d Drosophila_melanogaster.BDGP6.dna_sm.toplevel.fa.gz
gzip -d Drosophila_melanogaster.BDGP6.dna_rm.toplevel.fa.gz
gzip -d Danio_rerio.Zv9.dna.toplevel.fa.gz 
gzip -d Danio_rerio.Zv9.dna_sm.toplevel.fa.gz
gzip -d Danio_rerio.Zv9.dna_rm.toplevel.fa.gz

mv Drosophila_melanogaster.BDGP6.dna.toplevel.fa fruitfly.fa
mv Drosophila_melanogaster.BDGP6.dna_sm.toplevel.fa fruitfly_sm.fa
mv Drosophila_melanogaster.BDGP6.dna_rm.toplevel.fa fruitfly_rm.fa
mv Danio_rerio.Zv9.dna.toplevel.fa zebrafish.fa
mv Danio_rerio.Zv9.dna_sm.toplevel.fa zebrafish_sm.fa
mv Danio_rerio.Zv9.dna_rm.toplevel.fa zebrafish_rm.fa

./clean_input fruitfly.fa
./clean_input fruitfly_sm.fa
./clean_input fruitfly_rm.fa
./clean_input zebrafish.fa
./clean_input zebrafish_sm.fa
./clean_input zebrafish_rm.fa

