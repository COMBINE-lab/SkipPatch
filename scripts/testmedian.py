import random
import numpy
import sys
import argparse
import bisect

def testmedian(genomeFile, totedits, medindel, insprob, delprob, outputFile):
    #genome - path to genome
    #totedits - total number of edits to make
    #medindel - median (mean) size of indel edits. actual edit length determined from gaussian with mean medindel and std medindel/2
    #insprob - probability of insertion
    #delprob - probability of deletion
    #outputs all edits into a text file called "sampleedits.txt"

    if delprob + insprob >= 1.0:
        raise "Error, delprob = {} and insprob = {}. "\
              "The sum is {} >= 1.0".format(
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
    start = True
    numEdits = 0

    for val in editTypes:
        numEdits += 1
        if start:
            lenchange = max([int(round(random.gauss(medindel,medindel/2))),1])
            randval = random.randrange(20,numbases - lenchange - 20)
            start = False
        while randval in randr:
            randval = random.randrange(20,numbases - lenchange - 20)
        randr.append(randval)
        if val == 'I':
            lenchange = max([int(round(random.gauss(medindel,medindel/2))),1])
            #leninsert = max([int(round(random.gauss(medindel,medindel/2))),1])
            edittype = val
            instring = ''
            for k in range(lenchange):
                instring += random.choice(letters)
            newline = edittype + '\t' + str(randval) + '\t' + instring + '\n'
            numbases += lenchange
            outputFile.write(newline)
            for r in randr:
                if r > randval:
                    randr.remove(r)
                    randr.append(r+lenchange)
        elif val == 'D':
            lenchange = max([int(round(random.gauss(medindel,medindel/2))),1])
            edittype = val
            #lendel = max([int(round(random.gauss(medindel,medindel/2))),1])
            newline = edittype + '\t' + str(randval) + '\t' + str(randval+lenchange-1) + '\n'
            outputFile.write(newline)
            numbases -= lenchange
            for r in randr:
                if r > randval:
                    randr.remove(r)
                    randr.append(r-lenchange)
        elif val == 'S':
            edittype = val
            newSNP = random.choice(letters)
            newline = edittype + '\t' + str(randval) + '\t' + newSNP + '\n'
            outputFile.write(newline)
        #if ed%100 == 0:
            #print newline
    outputFile.close()

def main(args):
    genomeFile = args.genome
    totedits = args.totedits
    meanindel = args.meanindel
    insprob = args.insprob
    delprob = args.delprob
    outFile = args.output

    testmedian(genomeFile, totedits, meanindel, insprob, delprob, outFile)
    #testmedian(argv[1],int(argv[2]),int(argv[3]),float(argv[4]),float(argv[5]))

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Generate random edits following a gaussian distribution.")
    parser.add_argument('--genome', type=argparse.FileType('r'), help='path to the genome')
    parser.add_argument('--totedits', type=int, help='total number of edits to make')
    parser.add_argument('--meanindel', type=float, help='mean size of indel edits')
    parser.add_argument('--insprob', type=float, help='probability of insertion')
    parser.add_argument('--delprob', type=float, help='probability of deletion')
    parser.add_argument('--output', type=argparse.FileType('w'), help='file where edits should be written')
    args = parser.parse_args()
    main(args)
