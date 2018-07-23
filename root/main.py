# !E:\python\venv\Scripts\
# -*- coding:utf-8 -*-

# 全局变量
SESSION = None
POST = None
GET = None


def app():
    if POST is not None:
        print 'POST ', POST
        if ('name' in POST.keys()) and ('password' in POST.keys()) and (POST['name'] == '123') and (POST['password']=='123'):
            SESSION.setCookie('name', '123')
            SESSION.write2XML()
            return 'login success'
        else:
            with open('root/login.html', 'r') as f:
                data = f.read()
            return data
    else:
        if SESSION.getCookie('name') is not None:
            return 'hello, '+SESSION.getCookie('name')
        with open('root/login.html', 'r') as f:
            data = f.read()
        return data