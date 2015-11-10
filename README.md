
SkipPatch: A Dynamically Updatable Index for Text Search 
---------------------------------------------------------

SkipPatch, is a data structure which can be used for maintaining a k-mer-based index over a dynamically changing text.
SkipPatch pairs a hash-based k-mer index with a skip list to efficiently maintain the set of edits that have been applied to the original text.

Running SkipPatch:

1. Once you do a 'git clone' of this repository, generate the binary 'main' by running "make" from the 'src' folder.

2. Then, do './main --help' to see the command line arguments which can be used to run SkipPatch 

Dependencies:
C++11
