%%
%根据对应点生成对应点
function [] = generatePoint()
    pointStandard = importPointSingle('data/cube_5_without_points_allpoints_.txt');      %标定模型点集
    pointTest = importPointSingle('data/cube_4_allpoints_.txt');                         %待标定点集，带下划线的是没有经过归一化的model矩阵的
    posStandard = importPointSingle('data/cube_5_points_pos.txt');      %标定灯位置
    indexTest = importPointindex('data/cube_4_index.txt');              %标定和待标定点集的index对应关系
    
    figure(1)
    plot3(pointStandard(:,1),pointStandard(:,2),pointStandard(:,3),'r.');
    hold on
    plot3(posStandard(:,1),posStandard(:,2),posStandard(:,3),'b.');
    hold off
    
    index = zeros(size(posStandard,1),1);
    posTest = zeros(size(posStandard,1),3);
    
    for i=1:size(posStandard,1)         %对于每个灯
        pos = posStandard(i,:);
        pos = repmat(pos,size(pointStandard,1),1);
        diff=sqrt(sum((pos-pointStandard).^2,2));   %找到标定点集中的最近点
        [minvalue,index(i,1)]=min(diff);
        disp(index(i,1))
        id = indexTest(index(i,1));     %找到在待标定点集中的索引
        posTest(i,:) = pointTest(id,:);
    end
    
    fid = fopen('data/cube_4_pos.txt','w');
    fprintf(fid,'%d\r\n',size(index,1));    
    for i=1:size(index,1)
        fprintf(fid,'%f %f %f\r\n',posTest(i,1),posTest(i,2),posTest(i,3)); 
    end
    fclose(fid);
end


function [data] = importPointSingle(filename)
    data =[];
    fid = fopen(filename,'r');
    str = fgetl(fid);
    cnt = sscanf(str,'%d');
    for i=1:cnt
        str = fgetl(fid);
        tmp = sscanf(str,'%f %f %f');
        data = [data;tmp'];
    end
    fclose(fid);
end

function [data] = importPointindex(filename)
    data =[];
    fid = fopen(filename,'r');
    str = fgetl(fid);
    cnt = sscanf(str,'%d');
    for i=1:cnt
        str = fgetl(fid);
        tmp = sscanf(str,'%d %d %d');
        data = [data;tmp'];
    end
    fclose(fid);
end