import sys
cli_info_file="update.txt"
#[hostname] [ip] [client port] [os]

def update_file(hostname,ip, cli_port, os, protocol, ver):
   with open(cli_info_file,'r',encoding="UTF8") as f:
      temp=f.read()
   temp_list=temp.split("\n")
   index=0
   for i in temp_list:
      if ip in i:
         break
      index+=1
   
   if index==len(temp_list):
      if len(temp)>0 and temp[-1]!="\n":
         temp+="\n"
      temp+=f'{hostname} {ip} {cli_port} {os} {protocol} {ver}'
   else: 
      temp=temp.replace(temp_list[index],f'{hostname} {ip} {cli_port} {os} {protocol} {ver}')
      
   with open(cli_info_file,'w') as f:
      f.write(temp)

if __name__=="__main__":
   update_file(sys.argv[-6],sys.argv[-5],sys.argv[-4],sys.argv[-3], sys.argv[-2],sys.argv[-1])
