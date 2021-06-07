#인자로 해시값을 구할 파일들이 있는 경로를 입력한다 (./hash_srv /home/kwu/repository/)
#그러면 해당 경로에 있는 모든 파일들의 해시값을 계산하여 파일에 저장한다. (allHashsum.txt)
#이는 시스템과 별개로 실행해야하는 파일임


import hashlib
import sys
import os
import os.path

pwd = sys.argv[1]
filename = sys.argv[2]
hash_txt = pwd + "allHashsum.txt"

f = open(pwd+filename, 'rb') #file name = argv[1]
data = f.read()
f.close()

sys.stdout = open(hash_txt,'a+')
print(filename + ";" + hashlib.sha256(data).hexdigest()) #SHA-256
