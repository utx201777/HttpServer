# -*- coding=utf-8 -*-
import socket
import threading
from HttpHead import HttpRequest


def tcp_link(sock, addr):
    print('Accept new connection from %s:%s...' % addr)
    request = sock.recv(1024)    
    http_req = HttpRequest()
    http_req.passRequest(request)    
    sock.send(http_req.getResponse())
    sock.close()


# 开启服务器
def start_server():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)    
    s.bind(('127.0.0.1', 9999))
    s.listen(5)    
    while True:        
        sock, addr = s.accept()        
        t = threading.Thread(target=tcp_link, args=(sock, addr))
        t.start()

if __name__ == '__main__':
    start_server()
    pass

