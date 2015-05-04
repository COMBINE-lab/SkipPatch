import random
import numpy
import sys
def testmedian(genome,totedits, medindel, insprob, delprob):
    #genome - path to genome
    #totedits - total number of edits to make
    #medindel - median (mean) size of indel edits. actual edit length determined from gaussian with mean medindel and std medindel/2
    #insprob - probability of insertion
    #delprob - probability of deletion
    #outputs all edits into a text file called "sampleedits.txt"
    gfile = open(genome)
    genome = gfile.readline()
    gfile.close()
    numbases = len(genome)-1
    genome = genome[0:numbases]
    outfile = 'sampleedits.txt'
    f = open(outfile,'w')
    letters = ['A','C','G','T']
    randr = []
    allinds = []
    totrange = range(totedits)
    t1 = 1 - delprob
    t2 = t1 - insprob
    SNPrange = int(t1*totedits)
    insrange = int(insprob*totedits)
    delrange = int(delprob*totedits)
    start = True
    while sum([SNPrange,insrange,delrange]) > 0:
        if start:
            lenchange = max([int(round(random.gauss(medindel,medindel/2))),1])
            randval = random.randrange(20,numbases - lenchange - 20)
            start = False
        val = random.choice(['I','D','S'])
        while randval in randr:
            randval = random.randrange(20,numbases - lenchange - 20)
        randr.append(randval)
        if val == 'I' and insrange > 0:
            insrange -= 1
            lenchange = max([int(round(random.gauss(medindel,medindel/2))),1])
            #leninsert = max([int(round(random.gauss(medindel,medindel/2))),1])
            edittype = val
            instring = ''
            for k in range(lenchange):
                instring += random.choice(letters)
            newline = edittype + '\t' + str(randval) + '\t' + instring + '\n'
            numbases += lenchange
            f.write(newline)
            for r in randr:
                if r > randval:
                    randr.remove(r)
                    randr.append(r+lenchange)
        elif val == 'D' and delrange > 0:
            delrange -= 1
            lenchange = max([int(round(random.gauss(medindel,medindel/2))),1])
            edittype = val
            #lendel = max([int(round(random.gauss(medindel,medindel/2))),1])
            newline = edittype + '\t' + str(randval) + '\t' + str(randval+lenchange-1) + '\n'
            f.write(newline)
            numbases -= lenchange
            for r in randr:
                if r > randval:
                    randr.remove(r)
                    randr.append(r-lenchange)
        elif val == 'S' and SNPrange > 0:
            SNPrange -= 1
            edittype = val
            newSNP = random.choice(letters)
            newline = edittype + '\t' + str(randval) + '\t' + newSNP + '\n'
            f.write(newline)
        #if ed%100 == 0:
            #print newline
    f.close()

def main(argv=None):
    testmedian(argv[1],int(argv[2]),int(argv[3]),float(argv[4]),float(argv[5]))
            
if __name__ == '__main__':
    main(sys.argv)
