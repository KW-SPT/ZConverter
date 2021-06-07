import hashlib
import sys
import os
 
f = open(sys.argv[1], 'rb') #file name = argv[1]
data = f.read()
f.close()

sys.stdout = open('hash.txt','w')

#print(hashlib.md5(data).hexdigest()) #MD5
#print(hashlib.sha1(data).hexdigest()) #SHA-1
print(hashlib.sha256(data).hexdigest()) #SHA-256
print(str(os.path.getsize(sys.argv[1]))) #file size
