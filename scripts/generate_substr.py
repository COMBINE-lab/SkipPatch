#!/usr/bin/env python
import random
import numpy
import sys
import argparse
import bisect

LETTERS = "ACGT"

def random_deletion(numbases, medindel):
    # pick a random edit length
    delsize = max([int(round(random.gauss(medindel,medindel/2))),1])
    # pick a random location
    delpos = random.randrange(20, numbases - 20 - delsize)
    return delpos, delsize

def random_sequence(size):
    return ''.join([random.choice(LETTERS) for k in xrange(size)])

def random_insertion(numbases, medindel):
    inssize = max([int(round(random.gauss(medindel,medindel/2))),1])
    inspos = random.randrange(20, numbases - 20)
    seq = random_sequence(inssize)
    return inspos, inssize, seq

def random_snp(numbases):
    snppos = random.randrange(20, numbases - 20)
    return snppos, random_sequence(1)

def random_substr(numbases,meansubstr):
    # pick a random substring length
    substr_len = max([int(round(random.gauss(meansubstr,meansubstr/2))),1])
    # pick a random location
    substr_pos = random.randrange(0, int(numbases-meansubstr*1.5))
    return substr_pos, substr_len


def testmedian(genomeFile, totedits, medindel, insprob, delprob, outputFile, totsubstr, meansubstr, OutFileSubstr):
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
    for val in editTypes:

        if val == 'I':
            p, s, seq = random_insertion(numbases, medindel)
            numbases += s
            outputFile.write('I %d %s\n' % (p, seq))

        elif val == 'D':
            p, s = random_deletion(numbases, medindel)
            numbases -= s
            outputFile.write('D %d %d\n' % (p, p+s-1))

        else:
            p, seq = random_snp(numbases)
            outputFile.write('S %d %s\n' % (p, seq))

    outputFile.close()

    #generate random substring start positions, lengths 
    for i in range(0,totsubstr):
        p, l = random_substr(numbases, meansubstr)
        OutFileSubstr.write('%d,%d\n' % (p,l))
    OutFileSubstr.close()

def main(args):
    genomeFile = args.genome
    totedits = args.totedits
    meanindel = args.meanindel
    insprob = args.insprob
    delprob = args.delprob
    outFile = args.output
    totsubstr = args.totsubstr
    meansubstr = args.meansubstr
    OutFileSubstr = args.outputsubstr

    testmedian(genomeFile, totedits, meanindel, insprob, delprob, outFile, totsubstr, meansubstr, OutFileSubstr)
    #testmedian(argv[1],int(argv[2]),int(argv[3]),float(argv[4]),float(argv[5]))

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Generate random edits following a gaussian distribution.")
    parser.add_argument('--genome', type=argparse.FileType('r'), help='path to the genome')
    parser.add_argument('--totedits', type=int, help='total number of edits to make')
    parser.add_argument('--meanindel', type=float, help='mean size of indel edits')
    parser.add_argument('--insprob', type=float, help='probability of insertion')
    parser.add_argument('--delprob', type=float, help='probability of deletion')
    parser.add_argument('--output', type=argparse.FileType('w'), help='file where edits should be written')
    parser.add_argument('--totsubstr', type=int, help='total number of substrings to generate')
    parser.add_argument('--meansubstr', type=float, help='mean size of substring')
    parser.add_argument('--outputsubstr', type=argparse.FileType('w'), help='file where substrings should be written')
    args = parser.parse_args()
    main(args)
