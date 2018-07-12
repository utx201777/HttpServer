# -*- coding:utf-8 -*-
f = None
try:
    f = open('ninn', 'r')
except:
    print('打开失败')
finally:
    if f is not None:
        f.close()

