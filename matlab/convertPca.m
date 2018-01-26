%%
% 对输入的off文件进行pca转换并输出成off文件

%%
% 进行转化
function [] = convert(inputName,outputName)
    [point triangle] = readOff(inputName);
    point = pcaProcess(point);
    saveOff(outputName,point,triangle);
end

%%
% 对点数据进行pca处理，返回处理后的点数据
function [pdata] = pcaProcess(data)
    d = cov(data);      % 每一行是一个数据,计算协方差矩阵
    [E,D] = eig(d);
    [dummy,order] = sort(diag(-D)); %对特征值进行排序
    E = E(:,order);
    pdata = E'*data';   % 空间转换
    pdata = pdata';
end

%%
% 写入off，将点和三角形索引写入文件
function [] = saveOff(filename,point,triangle)
    fid = fopen(filename,'w');
    fprintf(fid,'%s\r\n','OFF');
    fprintf(fid,'%d %d %d\r\n',size(point,1),size(triangle,1),0);
    for i =1:size(point,1)
        fprintf(fid,'%f %f %f\r\n',point(i,1),point(i,2),point(i,3));
    end
    for i =1:size(triangle,1)
        fprintf(fid,'%d %d %d %d\r\n',3,triangle(i,1),triangle(i,2),triangle(i,3));
    end
    fclose(fid);
end

%%
% 导入off，返回点和三角形索引
function [point,triangle] = readOff(filename)
    fid = fopen(filename,'r');
    d = fgetl(fid);
    d = fgetl(fid);
    data = sscanf(d,'%d %d %d');
    point =[];
    triangle = [];    
    for i =1:data(1)
        d = fgetl(fid);
        tmp = sscanf(d,'%f %f %f');
        tmp = tmp';
        point = [point;tmp];
    end
    for i = 1:data(2)
        d = fgetl(fid);
        tmp = sscanf(d,'%f %f %f');
        tmp = tmp';
        triangle = [triangle;tmp(2:4)];
    end
    fclose(fid);
end