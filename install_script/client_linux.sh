#! /bin/bash

echo "Linux client bash script test!"

#: << 'END'
#cli_option.txt
echo "enter the ip"
read ip
echo "enter the server port number"
read port
echo "enter the version"
read ver
echo "enter the protocol"
read protocol
echo "enter the path"
read path
echo "enter the yout port number"
read port_num
echo "$ip" >> ./cli_option.txt
echo "$port" >> ./cli_option.txt
echo "$ver" >> ./cli_option.txt
echo "$protocol" >> ./cli_option.txt
echo "$path" >> ./cli_option.txt
echo "$port_num" >> ./cli_option.txt
#END

#ssh_info.txt
echo "enter the ssh user name"
read ssh_username
echo "enter the ssh password"
read ssh_password
echo "enter the ssh port"
read ssh_port
echo "$ssh_username;username" >> ./ssh_info.txt
echo "$ssh_password;password" >> ./ssh_info.txt
echo "$ssh_ssh_port;ssh_port" >> ./ssh_info.txt

mkdir client
cp Client.tar.gz client #move compress file to directory 'client'
#rm -rf Client.tar.gz	#delete compress file
cp cli_option.txt client
cp ssh_info.txt client
rm -rf cli_option.txt
rm -rf ssh_info.txt
cd client
tar -zxvf Client.tar.gz
tar -zxvf pyEXE_cli.tar.gz
rm -rf Client.tar.gz
rm -rf pyEXE_cli.tar.gz

gcc client_demon.c -o client


