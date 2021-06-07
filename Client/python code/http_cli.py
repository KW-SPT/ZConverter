# python3.7 http_cli.py http://127.0.1.1:8000/n.mp4 n.mp4 "/home/os2018202001/git/3.5.1"
import sys
import progressbar
import requests
import platform

def http_download(url,file_name,path):
       r = requests.get(url, stream=True)
       f = open(path+file_name, 'wb')
       file_size = int(r.headers['Content-Length'])
       chunk = 1
       num_bars = file_size / chunk
       bar =  progressbar.ProgressBar(maxval=num_bars).start()
       i = 0
       for chunk in r.iter_content(chunk_size=1024):
           f.write(chunk)
           bar.update(i)
           i+=1

       f.close()
       bar.finish()
       return

if __name__=="__main__":
	url = sys.argv[1]
	file_name = sys.argv[2]
	path = sys.argv[3]
	#path = path[0:len(path)-1]

	if platform.system()!="Windows":
               f_v="/"
	else:
               f_v="\\"
	path = path + f_v

	http_download(url,file_name,path)
