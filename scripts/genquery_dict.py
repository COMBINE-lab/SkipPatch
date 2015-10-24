#!/usr/bin/env python
import random
import numpy
import sys
import argparse
import jellyfish

LETTERS = "ACGT"
K = 20
EDIT_QUERY_PROB = 0.1

def random_deletion(numbases, medindel):
    # pick a random edit length
    delsize = max([int(round(random.gauss(medindel,medindel/2))),1])
    # pick a random location
    delpos = random.randrange(K*2, numbases - K*2 - delsize)
    return delpos, delsize

def random_sequence(size):
    return ''.join([random.choice(LETTERS) for k in xrange(size)])

def random_insertion(numbases, medindel):
    inssize = max([int(round(random.gauss(medindel,medindel/2))),1])
    inspos = random.randrange(K*2, numbases - K*2)
    seq = random_sequence(inssize)
    return inspos, inssize, seq

def random_snp(numbases):
    snppos = random.randrange(K*2, numbases - K*2)
    return snppos, random_sequence(1)

def add_kmers_in_seq(S, seq):
    for i in xrange(len(seq)-K):
	S[seq[i:i+K].upper()] = seq[i:i+K].upper()
        #S.add(seq[i:i+K].upper())

def genquery(genomeFile, jellyFile, totedits, medindel, insprob, delprob, queryfreq, querycount,  outputFile):
    #genome - path to genome
    #totedits - total number of edits to make
    #medindel - median (mean) size of indel edits. actual edit length determined from gaussian with mean medindel and std medindel/2
    #insprob - probability of insertion
    #delprob - probability of deletion
    #outputs all edits into a text file called "sampleedits.txt"

    if delprob + insprob > 1.0:
        raise "Error, delprob = {} and insprob = {}. "\
              "The sum is {} > 1.0".format(
                delprob, insprob, delprob + insprob)

    genome = genomeFile.readline()
    genomeFile.close()
    #mf = jellyfish.ReadMerFile(jellyFile)
    qf = jellyfish.QueryMerFile(jellyFile)
    numbases = len(genome)-1
    genome = genome[0:numbases]
    letters = ['A','C','G','T']
    randr = []
    allinds = []
    snpProb = 1.0 - (insprob + delprob)
    SNPrange = int(snpProb * totedits)
    insrange = int(insprob * totedits)
    delrange = int(delprob * totedits)

    editTypes = (['S'] * SNPrange) +\
                (['D'] * delrange) +\
                (['I'] * insrange)

    random.shuffle(editTypes)
    qcount = 0
    effectedkmers = dict()
    for val in editTypes:
        qcount += 1
        if val == 'I':
            p, s, seq = random_insertion(numbases, medindel)
            numbases += s
            outputFile.write('I %d %s\n' % (p, seq))
            add_kmers_in_seq(effectedkmers, seq)
            add_kmers_in_seq(effectedkmers, genome[p-K+1:p+K])

        elif val == 'D':
            p, s = random_deletion(numbases, medindel)
            numbases -= s
            outputFile.write('D %d %d\n' % (p, p+s-1))
            #add_kmers_in_seq(effectedkmers, genome[p-K+1:p+s-1+K])

        else:
            p, seq = random_snp(numbases)
            outputFile.write('S %d %s\n' % (p, seq))
            add_kmers_in_seq(effectedkmers, genome[p-K+1:p+K-1])

        # if it's time to output some queries
        if qcount == queryfreq:
            qcount = 0
            for qlist in xrange(querycount):
                dart = random.random()
                if dart <= EDIT_QUERY_PROB:
                    kmer = random.choice(effectedkmers.keys())
		    #kmer = random.sample(effectedkmers, 1)[0]
                    editflag = 'I'
                else:
                    p = random.randrange(K*2, numbases - K*2)
                    kmer = genome[p:p+K].upper()
                    editflag = 'N'
                
		kcount = 0
                #kcount = int(qf[jellyfish.MerDNA(kmer)])
                outputFile.write('Q %s %s %d\n' % (kmer, editflag, kcount))
    
    outputFile.close()

def main():
    global EDIT_QUERY_PROB
    global K
    parser = argparse.ArgumentParser(description="Generate random edits following a gaussian distribution.")
    parser.add_argument('--genome', type=argparse.FileType('r'), help='path to the genome')
    parser.add_argument('--jellyfile', type=str, help='path to the Jellyfish output file')
    parser.add_argument('--totedits', type=int, help='total number of edits to make')
    parser.add_argument('--meanindel', type=float, help='mean size of indel edits')
    parser.add_argument('--insprob', type=float, help='probability of insertion')
    parser.add_argument('--delprob', type=float, help='probability of deletion')
    parser.add_argument('--queryfreq', type=int, help='frequency of queries')
    parser.add_argument('--querycount', type=int, help='number of queries per query section')
    parser.add_argument('--output', type=argparse.FileType('w'), help='file where edits should be written')
    parser.add_argument('--editqueries', type=float, default=EDIT_QUERY_PROB, help='fraction of queries that hit an edit')
    parser.add_argument('--kmer_size','-k', type=int, default=20, help='kmer size')
    args = parser.parse_args()

    EDIT_QUERY_PROB=args.editqueries
    K=args.kmer_size
    
    genquery(args.genome, args.jellyfile, args.totedits, args.meanindel, args.insprob, args.delprob, args.queryfreq, args.querycount, args.output)

if __name__ == '__main__':
    main()
