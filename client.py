#!E:\python\venv\Scripts

# -*- coding:utf-8 -*-

import socket


def post_request():
    req = 'POST /?ni=00 HTTP/1.1\r\n'
    req = req + 'Host: 127.0.0.1:9999\r\n\r\n'
    req = req + 'name=linyi\ndata=163'
    return req


def get_request():
    req = 'GET /img/logo/php.png HTTP/1.1\r\n'
    req = req + 'Host: tool.oschina.net\r\n\r\n'
    return req


def start_request():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('tool.oschina.net', 80))
    print 'eatablished'
    req = get_request()
    print req
    s.send(req)
    buff = []
    while True:
        d = s.recv(1024)
        print len(d)
        if d:
            buff.append(d)
        else:
            break
    print 'abcdef'
    data = ''.join(buff)
    print len(data)
    s.close()
    header, html = data.split('\r\n\r\n', 1)
    f = open('./test.jpg', 'wb')
    f.write(html)
    f.close()


if __name__ == '__main__':
    # start_request()
    raw_input("press any key to exit;")

