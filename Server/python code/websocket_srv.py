#websocket_srv
import asyncio
import websockets
import base64
import os
from os.path import exists
from os.path import getsize
import platform
import sys
import socket
import datetime

#PORT = 9998
    
async def accept(websocket, path):
    if platform.system()!="Windows":
        f_v="/"
    else:
        f_v="\\"
   

    filename = await websocket.recv() #recv filename
    nowdir = os.getcwd()        #server address
    filesize = getsize(filename)
    print(filename)
    await websocket.send(str(filesize)) #send filesize
    
    with open(nowdir+f_v+filename, 'rb') as f:
        try:            
            data = f.read(1024)
            while data:
                await websocket.send(data)
                data = f.read(1024)
                
        except Exception as ex:
            print(ex)

# websocket server creation
PORT = int(sys.argv[1])
ipcheck = socket.gethostbyname(socket.getfqdn())

server_url = ipcheck + ":" + str(PORT)

buffer=1
sys.stdout=open('websocket_logfile.txt', 'a+', buffer)

now=datetime.datetime.now()

start_server = websockets.serve(accept, ipcheck , PORT)
print(now, "Websocket Server Serving at", server_url)



# waiting
asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
