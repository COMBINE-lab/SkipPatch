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

def make_output_dir(output_path,timestamp,comment):
    #print ("op path type ",type( (output_path)))
    #print ("op timestamp formatted ",type( (timestamp)))
    output_path_dirname = ""
    if output_path is None:
        return output_path_dirname
    output_path_dirname = format_path(output_path)+comment+'_'+format_path(timestamp)
    
    if os.path.isdir(output_path_dirname):
        #print "\nThe benchmark and updated genome files being saved in same directory!\n"
        pass
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
    print '\n'
    print "done running",name,"!"
    print '\n'

def format_args(arg, option):
    if arg is not None:
        return " -"+option+ " "+arg
    return ""


#TODO: write a script which does this naively instead of using the SA

parser = argparse.ArgumentParser(description='Benchmark, do a memory analysis or test the correctness of the Skip Patch (currently by diff with SA output) \n ')

parser.add_argument('-SPBinary','-sp', type=str, help = "path to the Skip Patch binary")
parser.add_argument('-SABinary', '-sa', type=str, help = "path to the Suffix Array binary")
parser.add_argument('-output_path_updated_genome','-o', type = str, action = writeable_dir, help='directory to write the output (useful for verifying correctness)')
parser.add_argument('-log_path','-l', type = str, action = writeable_dir, help='directory to write the logs')
parser.add_argument('-comments', '-c', type=str, help='why are you running this test?' )
parser.add_argument('-runCommand','-rc', type = str, help='the cli params to run the tests with')
parser.add_argument('-mem', action='store_true', default=False, help = "do a memory analysis")
parser.add_argument('-dontMake','-dm', action='store_true', default=False, help = "dont make before running")
args = parser.parse_args()

import datetime
timestamp = str(datetime.datetime.now()).replace(" ","-")

#building args for running SA and SP


output_path_formatted = ''
output_path_SP = ""
output_path_SA = ""
formatted_comments = str(args.comments).replace(' ','')
if args.output_path_updated_genome is not None:
    output_path = make_output_dir(args.output_path_updated_genome,timestamp,formatted_comments)

    output_path_SP = format_path(output_path) + 'SPGenome.fa'
    output_path_SA = format_path(output_path) + 'SAGenome.fa'
    output_path_formatted = format_args(output_path,'o')

log_path = ""
if args.log_path is not None:
    log_path = make_output_dir(args.log_path,timestamp,formatted_comments)
    log_path_formatted = format_args(format_path(log_path),'l')

if args.SPBinary is not None:
    run_SP = args.SPBinary+output_path_formatted+log_path_formatted+ ' '+args.runCommand

    if args.dontMake is False:
        print "Executing make for SP .."
        sb.call("make -C "+ args.SPBinary[:(args.SPBinary.rfind('/') +1 )],shell = True)  #getting dir name from path

    if args.mem is True:
        run_SP+= ' ) 2> ' + log_path + 'SPMem.txt'
        run_SP = '( /usr/bin/time -v ' + run_SP
    #print run_SP
    execute(run_SP,"Skip Patch",log_path)

if args.SABinary is not None:
    run_SA = args.SABinary+output_path_formatted+log_path_formatted+ ' '+args.runCommand + ' > '+ log_path + 'SA.log'
    if args.dontMake is False:
        print "Executing make for SA .."
        sb.call("make -C "+ args.SABinary[:(args.SABinary.rfind('/') + 1 )],shell = True)  #getting dir name from path
    if args.mem is True:
        run_SA+= ' ) 2> ' + log_path + 'SAMem.txt'
        run_SA = '( /usr/bin/time -v ' + run_SA
    #print run_SA
    execute(run_SA,"Suffix Array",log_path)


if args.output_path_updated_genome is not None:
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
                print "SP:", a, "SA:", b,"at pos",pos
                print fail
                flag = False
                break
        pos+=1

    if(flag):
        print success

print "\n\n"
#sb.call("aplay "+args.alert,shell = True) 
