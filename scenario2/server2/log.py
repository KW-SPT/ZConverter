import time
import sys
cli_info_file="update.txt"
result_file="scenario2.txt"

#[hostname] [ip] [client port] [os] [전송방식] [버전]

def extract_cli(request_ver):
   with open(cli_info_file,'r',encoding="UTF8") as f:
      temp=f.read()
   temp_list=temp.split("\n")
   result=""
   for i in temp_list:
      if request_ver in i:
         result+=f'{i}\n'

   if result!= "":
      result=result[:-1]
      with open(result_file,'w') as f:
         f.write(result)

if __name__=="__main__":
   extract_cli(sys.argv[-1])
