# -*- coding:utf-8 -*-
import numpy as np
import re
__author__ = 'linyi'


# 计算三角形的面积，计算叉乘，所得结果的向量模长是平行四边形的面积，除以二即可
def triangle_area(p1, p2, p3):
    x1 = p1-p2  # 必须是numpy才支持向量式运算
    x2 = p1-p3
    x3 = np.cross(x1, x2)
    return np.linalg.norm(x3)/2


# 生成三角形内的点
def generate_points(p1, p2, p3, density):
    area = triangle_area(p1, p2, p3)
    sample_size = area*density
    sample_size = int(sample_size)
    rnd1 = np.random.random(size=sample_size)
    rnd2 = np.random.random(size=sample_size)
    rnd2 = np.sqrt(rnd2)
    p = np.zeros([3, sample_size])
    for i in range(3):
        p[i] = rnd2 * (rnd1 * p1[i] + (1 - rnd1) * p2[i]) + (1 - rnd2) * p3[i]
    # print('p shape:', p.shape)
    return p


# 保存点数据
def save_points(filename, points):
    try:
        f = open(filename, 'w')
        f.writelines([str(points.shape[0]-1), '\n'])
        for i in range(points.shape[0]):
            if i == 0:
                continue
            f.writelines([str(points[i][0]), ' ', str(points[i][1]), ' ', str(points[i][2]), '\n'])
    finally:
        if f:
            f.close()
        print(filename, 'file close')


# 导入off文件
def load_off(filename):
    try:
        f = open(filename, 'r')
        f.readline()
        line = f.readline()
        para = re.split(r'\s+', line)
        points_cnt = int(para[0])
        triangles_cnt = int(para[1])
        points = np.zeros([points_cnt, 3])
        triangle = np.zeros([triangles_cnt, 3], dtype=int)
        for i in range(points_cnt):
            line = f.readline()
            para = re.split(r'\s+', line)
            points[i] = np.array([float(para[0]), float(para[1]), float(para[2])])
        for i in range(triangles_cnt):
            line = f.readline()
            para = re.split(r'\s+', line)
            triangle[i] = np.array([int(para[1]), int(para[2]), int(para[3])])
        return points, triangle
    finally:
        if f:
            f.close()
        print(filename, 'file close')


# 导入矩阵
def load_matrix(filename):
    try:
        f = open(filename, 'r')
        model_matrix = np.zeros([4, 4])
        for i in range(4):
            line = f.readline()
            para = re.split(r'\s+', line)
            model_matrix[i] = np.array([float(para[0]), float(para[1]), float(para[2]), float(para[3])])
        print(model_matrix)
        return model_matrix
    finally:
        if f:
            f.close()
        print(filename, 'file close')


# 对点数据进行矩阵操作
def model_transform(points, model_matrix):
    points_tmp = np.zeros(points.shape)
    for i in range(points.shape[0]):
        tmp = np.mat([points[i][0], points[i][1], points[i][2], 1])
        tmp = np.dot(model_matrix, tmp.T)
        # print(tmp[0, 0])
        points_tmp[i] = np.array([tmp[0, 0], tmp[1, 0], tmp[2, 0]])
    return points_tmp


# 在三角形内插值
def interpolation_triangle(points, triangles, para):
    points_cnt = points.shape[0]
    triangles_cnt = triangles.shape[0]
    print('points cnt: ', points_cnt)
    print('triangles cnt: ', triangles_cnt)
    points_total = np.array([1, 2, 3])
    for i in range(triangles_cnt):
        p = generate_points(points[triangles[i][0]], points[triangles[i][1]], points[triangles[i][2]], para)
        points_total = np.vstack([points_total, p.T])
    print(points_total.shape)
    # print('triangles cnt: ', triangles_cnt)
    return points_total


if __name__ == '__main__':
    pass
