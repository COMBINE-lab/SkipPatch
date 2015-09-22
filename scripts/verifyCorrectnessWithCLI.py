import argparse
import os
import subprocess as sb
import itertools

#http://stackoverflow.com/questions/11415570/directory-path-types-with-argparse
class readable_dir(argparse.Action):
    def __call__(self,parser, namespace, values, option_string=None):
        prospective_dir=values
        if not os.path.isdir(prospective_dir):
            raise argparse.ArgumentTypeError("readable_dir:{0} is not a valid path".format(prospective_dir))
        if os.access(prospective_dir, os.R_OK):
            setattr(namespace,self.dest,prospective_dir)
        else:
            raise argparse.ArgumentTypeError("readable_dir:{0} is not a readable dir".format(prospective_dir))
            
class writeable_dir(argparse.Action):
    def __call__(self,parser, namespace, values, option_string=None):
        prospective_dir=values
        if not os.path.isdir(prospective_dir):
            raise argparse.ArgumentTypeError("readable_dir:{0} is not a valid path".format(prospective_dir))
        if os.access(prospective_dir, os.W_OK):
            setattr(namespace,self.dest,prospective_dir)
        else:
            raise argparse.ArgumentTypeError("readable_dir:{0} is not a writable dir".format(prospective_dir))

class readable_file(argparse.Action):
    def __call__(self,parser, namespace, values, option_string=None):
        prospective_file=values
        if not os.path.isfile(prospective_file):
            raise argparse.ArgumentTypeError("readable_file:{0} is not a valid path".format(prospective_file))
        if os.access(prospective_file, os.R_OK):
            setattr(namespace,self.dest,prospective_file)
        else:
            raise argparse.ArgumentTypeError("readable_dir:{0} is not a readable file".format(prospective_dir))


def format_path(s):
    delim = "/"
    if(s[len(s)-1]!=delim):
        s= s+delim
    return s

def make_output_dir(output_path,timestamp):
    #print ("op path type ",type( (output_path)))
    #print ("op timestamp formatted ",type( (timestamp)))
    output_path_dirname =  format_path(output_path)+format_path(timestamp)
    
    if os.path.isdir(output_path_dirname):
        print "The benchmark and updated genome files being saved in same directory!"
    else:
        mknewdir = "mkdir "+ output_path_dirname
        sb.call([mknewdir],shell=True)
    
    return output_path_dirname
    
def write_args(path,args):

    f1 = open(path+"args.txt","a")
    f1.write(str(args))
    f1.write('\n')
    f1.close()


def execute(runCommand,name,output_path):
    print "\n"
    print "running",name,"...\n"
    print "runcommand:", runCommand,"\n"
    write_args(output_path,runCommand)
    retValSA = sb.call(runCommand,shell = True)
    print "done running",name,"!"


#TODO: write a script which does this naively instead of using the SA
parser = argparse.ArgumentParser(description='Test the correctness of the Skip Patch (currently by diff with SA output) \n All arguments are mandatory!')

parser.add_argument('-SPBinary','-sp', type=str, help = "path to the Skip Patch binary")
parser.add_argument('-SABinary', '-sa', type=str, help = "path to the Suffix Array binary")
parser.add_argument('-genome','-g', type=str, action = readable_file, help = "path to the genome fasta")
parser.add_argument('-edit_file','-e', action = readable_file, type=str, help = "The edit file.")
parser.add_argument('-output_path_updated_genome','-og', type = str, action = writeable_dir, help='directory to write the updated genome')
parser.add_argument('-output_path_benchmark','-ob', type=str, action =  writeable_dir, help='directory to write the benchmarks')
parser.add_argument('-num_edits', '-n', type=int, help = "Number of edits to perform")
#parser.add_argument('-alert','-a', type=str, action = readable_file, help='make a sound after the test ends, path to the .wav file' )
parser.add_argument('-comments', '-c', type=str, help='why are you running this test?' )

args = parser.parse_args()

import datetime
timestamp = str(datetime.datetime.now()).replace(" ","-")

output_path = make_output_dir(args.output_path_updated_genome,timestamp)
output_path_benchmark = make_output_dir(args.output_path_benchmark,timestamp)

#building args for running SA and SP

SPBinary = args.SPBinary
SABinary = args.SABinary
genome = " -g "+ args.genome
edit_file = " -e "+ args.edit_file
build_hash = " -b"
num_edits_formatted = " -n "+ str(args.num_edits)

output_path_SP= output_path + "SPGenome.fa"
output_path_SP_formatted = " -o " + output_path_SP 
output_path_SA= output_path + "SAGenome.fa"
output_path_benchmark_SA = " > "+ output_path_benchmark + "SABenchmark.txt"
output_path_benchmark_SP = " > "+ output_path_benchmark + "SPBenchmark.txt"

run_SP = SPBinary+genome+edit_file+ build_hash+num_edits_formatted+output_path_SP_formatted+output_path_benchmark_SP 
run_SA = SABinary+" "+args.genome+ " "+ args.edit_file+" "+str(args.num_edits)+" "+output_path_SA+ output_path_benchmark_SA 

execute(run_SP,"Skip Patch",output_path)
execute(run_SA,"Suffx Array",output_path)

print "\n\n\n"
with open (output_path_SP, "r") as myfile:
    g1=myfile.read()
myfile.close()

with open (output_path_SA, "r") as myfile:
    g2=myfile.read()
myfile.close()

fail = "TESTS FAILED!!"
success  = "TESTS PASS"
flag =  True

print "len(SP) = ",len(g1)," len(SA) ",len(g2)-1

pos = 0
for a,b in itertools.izip_longest(g1,g2):
    if(a!=b):
        if((ord(b) != 10)): # SA adds a LF ascii charater to the end
            print "The first position where the output files differ is ", pos
            print "SP:", a, "SA:", ord(b),"at pos",pos
            print fail
            flag = False
            break
    pos+=1

if(flag):
    print success

print "\n\n"
#sb.call("aplay "+args.alert,shell = True) 
