#progress
import sys
import os

def upload(localpath, filepath, scp):
   #put - upload
   sys.stdout.write("put the file to the remote server")
   scp.put(localpath, filepath) #localpath --upload(send)--> filepath
   sys.stdout.write('\n')   

def progress(filename, size, sent):
   sys.stdout.write("%s\'s progress: %.2f%%    \r" % (filename, float(sent)/float(size)*100))
   
# from the server
if len(sys.argv) < 7:
   sys.stdout.write('{0} <IP> <PORT> <username> <password> <localpath> <filepath>'.format(sys.argv[0]))
   sys.exit()

#scp
from scp import SCPClient
import paramiko
import socket
import hashlib
import sys
import os
paramiko.util.log_to_file("paramiko.log")

ssh = paramiko.SSHClient()
ssh.load_system_host_keys()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy()) #connect to ssh server(connection)

#sys.argv[]: <IP> <PORT> <username> <password> <localpath> <filepath>
IP = sys.argv[1]
PORT = sys.argv[2]
usr_name = sys.argv[3]
usr_pwd = sys.argv[4]
srv_path = sys.argv[5]
cli_path = sys.argv[6]

try:
   ssh.connect(hostname = IP, username = usr_name, password = usr_pwd, port = PORT)
   sys.stdout.write("connected successfully")
   sys.stdout.write("\n")   

   #localpath: where to find files to upload
   #filepath: upload location

   localpath = srv_path
   filepath = cli_path
   
   scp = SCPClient(ssh.get_transport(), socket_timeout=15.0, progress = progress)
   upload(localpath, filepath, scp) #send file

   scp.close()
   ssh.close()

except   (paramiko.ssh_exception.SSHException, socket.error) as e: 
   sys.stdout.write("ERROR: ", e)
   
