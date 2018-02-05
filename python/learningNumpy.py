# -*- coding:utf-8 -*-

import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import geoNormal as gn


# 在一个三角形中生成均匀的点并画出来
def draw():
    fig = plt.figure()
    ax = Axes3D(fig)
    # ax = plt.subplot(111, projection='3d')  # 创建一个三维的绘图工程
    p1 = np.array([0, 30, 100])
    p2 = np.array([200, -10, 20])
    p3 = np.array([100, 200, 50])
    theta = np.linspace(0, 1, num=1000)
    line1 = np.zeros([3, 1000])
    line2 = np.zeros([3, 1000])
    line3 = np.zeros([3, 1000])
    for i in range(p1.size):
        line1[i] = theta * p1[i] + (1 - theta) * p2[i]
        line2[i] = theta * p1[i] + (1 - theta) * p3[i]
        line3[i] = theta * p2[i] + (1 - theta) * p3[i]
    ax.plot(line1[0], line1[1], line1[2])
    ax.plot(line2[0], line2[1], line2[2])
    ax.plot(line3[0], line3[1], line3[2])
    p = gn.generate_points(p1, p2, p3, 0.01)
    ax.scatter(p[0], p[1], p[2], c='y')  # 绘制数据点
    ax.set_zlabel('Z')  # 坐标轴
    ax.set_ylabel('Y')
    ax.set_xlabel('X')
    plt.show()


# 测试函数
def func(a):
    a += a


if __name__ == '__main__':
    # draw()
    points, triangles = gn.load_off(r'model/cube_4_.off')
    # points_total = gn.interpolation_triangle(points, triangles, 1)       # 三角形插值
    points_total = points
    model_matrix = gn.load_matrix(r'model/cube_4_matrix.txt')      # 导入矩阵
    gn.save_points(r'model/cube_4_points_all_.txt', points_total)
    points_total = gn.model_transform(points_total, model_matrix.T)    # 矩阵转换
    gn.save_points(r'model/cube_4_points_all.txt', points_total)
