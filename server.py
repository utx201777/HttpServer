# -*- coding=utf-8 -*-
import socket
import threading
from HttpHead import HttpRequest


def tcp_link(sock, addr):
    print('Accept new connection from %s:%s...' % addr)
    request = sock.recv(1024)
    # print request
    http_req = HttpRequest()
    http_req.passRequest(request)
    # 发送数据
    sock.send(http_req.getResponse())
    sock.close()


# 开启服务器
def start_server():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # 监听端口:
    s.bind(('127.0.0.1', 9999))
    s.listen(5)
    # print('Waiting for connection...')
    while True:
        # 接受一个新连接:
        sock, addr = s.accept()
        # 创建新线程来处理TCP连接:
        t = threading.Thread(target=tcp_link, args=(sock, addr))
        t.start()


if __name__ == '__main__':
    start_server()
    pass

