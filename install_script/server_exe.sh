#! /bin/bash

echo "server script test!"

#define.txt
#: << 'END'
echo "enter the port number"
read port
echo "enter the server ip"
read ip
echo "enter the https server port number"
read http_port
echo "enter the websocket server port number"
read ws_port
echo "enter the server repository path"
read srv_localpath

http_url="\"http://$ip:$http_port/\""
websocket_url="ws://$ip:$ws_port"

echo "$port;port number" >> ./define.txt
echo "$ip;ip" >> ./define.txt
echo "$http_url;http_url" >> ./define.txt
echo "$websocket_url;websocket_url" >> ./define.txt
echo "$srv_localpath;server repository path" >> ./define.txt

#END

#new.txt
#: << 'END'
echo "enter the windows's latest version"
read win_ver
echo "enter the linux's latest version"
read lin_ver

echo "$win_ver" >> ./new.txt
echo "$lin_ver" >> ./new.txt
#END

chmod +w "$srv_localpath"
cp define.txt $srv_localpath
cp new.txt $srv_localpath
cp Server.tar.gz $srv_localpath
rm -rf define.txt
rm -rf new.txt
rm -rf Server.tar.gz

cd $srv_localpath
tar -zxvf Server.tar.gz
rm -rf Server.tar.gz
tar -zxvf pyEXE_srv.tar.gz
rm -rf pyEXE_srv.tar.gz
gcc server.c -o server -lpthread







