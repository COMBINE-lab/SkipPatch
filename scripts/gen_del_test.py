import subprocess as sb
import argparse
import datetime
import os
timestamp = str(datetime.datetime.now()).replace(" ","-")

def format_path(s):
        delim = "/"
        if(s[len(s)-1]!=delim):
            s= s+delim
        return s

class writeable_dir(argparse.Action):
    def __call__(self,parser, namespace, values, option_string=None):
        prospective_dir=values
        if not os.path.isdir(prospective_dir):
            raise argparse.ArgumentTypeError("readable_dir:{0} is not a valid path".format(prospective_dir))
        if os.access(prospective_dir, os.W_OK):
            setattr(namespace,self.dest,prospective_dir)
        else:
            raise argparse.ArgumentTypeError("readable_dir:{0} is not a writable dir".format(prospective_dir))

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


def run_test(num_edits):
    run_command=''
    run_command += '''python /home/nitish/SkipPatch/scripts/run.py -SABinary "/home/nitish/SkipPatch/dyn_sa/src/test" -SPBinary "/home/nitish/SkipPatch/src/main" -o "/mnt/scratch2/nirm/results/nitish/gen_del/op/" -rc '-g /mnt/scratch2/nirm/data/ecoli.fa -e /mnt/scratch2/nirm/data/update_number/ecoli_100 -n '''
    run_command+= str(num_edits)
    run_command+=''' ' -c "sp_gen_del_on_ecoli" '''
    print run_command
    run_command= run_command+ ' > '+output_path
    sb.call(run_command,shell = True)

    run_command = ''
    run_command = 'cat '+output_path+'|grep FAIL'

    retval = sb.call(run_command,shell=True)
    return retval

def binary_search(begin,end):

    if begin>=end:
        return begin
    mid = (begin+end)/2
    print "first",begin,end,mid
    succeeded=run_test(mid)
    if succeeded:
        begin=mid+1
    else:
        end=mid-1
    print "second",begin,end,mid
    return binary_search(begin,end)

parser = argparse.ArgumentParser(description='Test generic deletion for different number of edits \n ')

parser.add_argument('-output_path','-o', type = str, action = writeable_dir, help='directory to write the output')
parser.add_argument('-num_edits','-e', type = int, help='number of edits')
parser.add_argument('-step_size','-s', type = int, default = 500, help='step size')
parser.add_argument('-comments','-c', type = str, help='comments')

args = parser.parse_args()

formatted_comments = str(args.comments).replace(' ','')
output_path = make_output_dir(args.output_path,timestamp,formatted_comments)
result_path = format_path(output_path)+'result.txt'
output_path = format_path(output_path)+ 'test.log'

begin=0
end=args.num_edits
retval=binary_search(begin,end)
with open(result_path,'w') as f:
    res = 'The failing line number is:'+ str(retval)
    f.write(res)

