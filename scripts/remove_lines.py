
f =open("../src/edit","r")
d = dict()
print (d.keys())
for line in f:
    d[line.strip()]=1
print (d.keys())
f.close()
f= open("ecoli_test.txt","r")
lines = f.readlines()
f.close
f = open("op.txt","w")
i=0
for line in lines:
    if str(i) not in d.keys():
        f.write(line)
    i+=1
f.close()
