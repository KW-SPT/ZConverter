# http_server
import http.server # Our http server handler for http requests
import socketserver # Establish the TCP Socket connections
import sys
import socket
import datetime
 
#PORT = 8000

class MyHttpRequestHandler(http.server.SimpleHTTPRequestHandler):
    '''def do_GET(self):
        #self.path = 'index.html'
        http.server.SimpleHTTPRequestHandler.do_GET(self)
        httpd.shutdown
        return sys.exit()'''

 
Handler = MyHttpRequestHandler
ipcheck = socket.gethostbyname(socket.getfqdn())

PORT = int(sys.argv[1])
server_url = ipcheck + ":" + str(PORT)

with socketserver.TCPServer((ipcheck, PORT), Handler) as httpd:
    buffer = 1
    sys.stderr=open('http_logfile.txt', 'a+', buffer)
    #print(now, "Http Server Serving at", server_url)
    httpd.serve_forever()
