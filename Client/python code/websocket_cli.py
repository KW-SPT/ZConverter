import asyncio
import websockets
import os
import sys
import progressbar
import platform

#python cli.py 파일이름



async def connect():

    if platform.system()!="Windows":

        f_v="/"

    else:

        f_v="\\"

        

    async with websockets.connect(url) as websocket:

        try:

            srvdir=sys.argv[1]

            filename=sys.argv[2]

            nowdir = sys.argv[3]

            #nowdir=nowdir[0:len(nowdir)-1]

            

            await websocket.send(srvdir+"/"+filename) #send filename

            #nowdir = os.getcwd()        #클라이언트 path



            filesize = await websocket.recv() #recv filesize



            data_transferred = 0

            chunk = 1024

            num_bars = int(int(filesize) // chunk)

            if (int(filesize) % chunk)!=0:

                num_bars+=1

            bar =  progressbar.ProgressBar(maxval=num_bars).start()

            i=0

            with open(nowdir + f_v + filename, 'wb') as f:

                for _ in range(num_bars):

                    data = await websocket.recv()

                    f.write(data)

                    bar.update(i)

                    i+=1

            bar.finish()

        except Exception as ex:

            print(ex)

            



# connect to server with async

url = sys.argv[4]
asyncio.get_event_loop().run_until_complete(connect())


