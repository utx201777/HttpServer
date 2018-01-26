function [] = ICPprocess()
    %%
    %寻找的变换关系data2=Rdata1+T
    %data2不变，data1不断的矫正
    %加载数据选取控制点
    d1 = importPoint('data/cube_4_sp.txt');
    d2 = importPoint('data/cube_5_without_points_sp.txt');

    %[d1,d2] = generateData();
    data1 =d1;
    data2 =d2;

    figure(1);
    plot3(data1(:,1),data1(:,2),data1(:,3),'r.');
    hold on;
    plot3(data2(:,1),data2(:,2),data2(:,3),'b.');
    title('原始数据');
    axis tight equal;
    hold off;

    [m,n]=size(data2);
    controldata1=d1;    % 选取控制点
    controldatanum=size(controldata1,1);
    controldata2=zeros(controldatanum,3); %用来存储找到的对应点

    %%
    %初始化
    R=[1,0,0;0,1,0;0,0,1];
    T=[0,0,0];
    last_E=0; %上次的错误率
    iteration=50; %迭代次数
    R_Intermediate=zeros(3,3,iteration);    %存储每一次迭代的R矩阵
    T_Intermediate=zeros(3,1,iteration);    %存储每一次迭代的T矩阵
    delta_Intermediate=zeros(iteration,1);  %存储每一次迭代的delta
    e_Intermediate=zeros(iteration,1);      %存储每一次迭代的e
    index=zeros(controldatanum,1);          %用来存储对应点的索引

    %%
    %迭代
    for iter=1:iteration
        %寻找控制点的对应点，controldata2用来存储对应点的坐标，index存储对应点的索引
        for i=1:controldatanum
            temp_data1=repmat(controldata1(i,:),m,1); %对这一个数据进行复制，方便后面直接进行矢量化的运算
            diff=sqrt(sum((temp_data1-data2).^2,2));  %获得每一组数据到该点的距离,sum(,2)是行求和
            [minvalue,index(i,1)]=min(diff);          %找到其中最小的，并得到他的index
            controldata2(i,:)=data2(index(i,1),:);    %将对应的数据存储在对应的位置
        end
        %%
        %对于确定的关系，求解RT，其中controldata1是控制点的坐标，controldata2是求到控制点的最近距离
        %匹配后的中心坐标
        centroid1=mean(controldata1);    
        centroid2=mean(controldata2);
        %减去中心坐标 
        demeancontroldata1=controldata1-repmat(centroid1,controldatanum,1);
        demeancontroldata2=controldata2-repmat(centroid2,controldatanum,1);
        %获得H矩阵
        H=demeancontroldata1'*demeancontroldata2;
        [U,S,V]=svd(H);
        R=V*U';                         %获得R矩阵，S是特征值
        T=(centroid2-centroid1)';       %获得T矩阵

        R_Intermediate(:,:,iter)=R;     %记录到每一次迭代的RT
        T_Intermediate(:,:,iter)=T;
        %%
        %利用求解得到的RT计算变换之后的点
        controldata1=R*controldata1'+repmat(T,1,controldatanum);
        controldata1=controldata1';                 %新的控制点
        E=norm(controldata1-controldata2,2);        %计算范式距离
        e_Intermediate(iter,1)=E/controldatanum;    %计算误差
        delta=abs(E-last_E)/controldatanum;          %中间迭代的误差
        delta_Intermediate(iter,1)=delta;   
        if(delta<0.001)
            break;
        end
        last_E=E;           % 更新上一次的误差
    end

    %% 
    %显示计算过程中的误差
    figure(2);
    plot(1:iter,delta_Intermediate(1:iter,1)');
    xlabel('迭代次数');ylabel('delta');
    figure(3);
    plot(1:iter,e_Intermediate(1:iter)');
    xlabel('迭代次数');ylabel('loss');
    %%
    %计算最终的R与T
    temp_R=eye(3);
    temp_T=zeros(3,1);
    %将计算得到的RT进行合并计算，得到最终的RT
    for i=1:iter
       temp_R=R_Intermediate(:,:,i)*temp_R; 
       temp_T=R_Intermediate(:,:,i)*temp_T+T_Intermediate(:,:,i);
    end
    R_final=temp_R;
    T_final=temp_T;
    %计算经过最终完整的变换后的数据
    data1_transformed=R_final*data1'+repmat(T_final,1,size(data1,1));
    data1_transformed=data1_transformed';

    %%
    %显示最终结果
    figure(4);
    plot3(data1_transformed(:,1),data1_transformed(:,2),data1_transformed(:,3),'r.');
    hold on;
    plot3(data2(:,1),data2(:,2),data2(:,3),'b.');
    title('ICP results');
    hold on;
    %画连线
    for i =1:controldatanum
        data3 = [data2(index(i,1),:);data1_transformed(i,:)];        
        plot3(data3(:,1),data3(:,2),data3(:,3),'-');
    end
    axis equal tight;
    hold off;
    %对全部的点进行变换       
    
    ad1 = importPointSingle('data/cube_4_allpoints.txt');           %这些数据是经过归一化矩阵的
    ad2 = importPointSingle('data/cube_5_without_points_allpoints.txt');
    data1_transformed=R_final*ad1'+repmat(T_final,1,size(ad1,1));
    data1_transformed=data1_transformed';
    
    index = zeros(size(ad2,1),1);
    controldata2 = zeros(size(ad2));
                
    figure(5);
    plot3(data1_transformed(:,1),data1_transformed(:,2),data1_transformed(:,3),'r.');
    hold on;
    plot3(ad2(:,1),ad2(:,2),ad2(:,3),'b.');
    hold on;
    
    %寻找全局的对应点
    for i=1:size(ad1,1)
        temp_data1=repmat(ad2(i,:),size(data1_transformed,1),1); %对这一个数据进行复制，方便后面直接进行矢量化的运算
        diff=sqrt(sum((temp_data1-data1_transformed).^2,2));  %获得每一组数据到该点的距离,sum(,2)是行求和
        [minvalue,index(i,1)]=min(diff);          %找到其中最小的，并得到他的index        
        data = [data1_transformed(index(i,1),:);ad2(i,:)];
        plot3(data(:,1),data(:,2),data(:,3),'-');
    end  
    
    % 存储点对应的索引
    fid = fopen('data/cube_4_index.txt','w');
    fprintf(fid,'%d\r\n',size(index,1));
    for i=1:size(index,1)
        fprintf(fid,'%d\r\n',index(i,:));
    end
    fclose(fid);
    
    title('对1进行变换')
    hold off;
    
    data1_transformed=R_final'*ad2'-repmat(T_final,1,size(ad2,1));
    data1_transformed=data1_transformed';
    figure(6);
    plot3(data1_transformed(:,1),data1_transformed(:,2),data1_transformed(:,3),'r.');
    hold on;
    plot3(ad1(:,1),ad1(:,2),ad1(:,3),'b.');
    title('对2进行变换')
    hold off;
end

%% 
%生成数据
function [d1,d2] = generateData()
    size1 =[3,4,3];     % 设置的长方体的长宽高
    size2 =[3,3,3];
    sita1 =30/180*pi;
    Tr = [1.2,2.8,1.2];
    rotatoMat =[cos(sita1),0,-sin(sita1);
        0,1,0;
        sin(sita1),0,cos(sita1)];
    d1=[];
    d2=[];
    % 生成第一个长方体采样点数据
    for i=1:100
        % 左右两个面
        t1 = [size1(1),rand()*size1(2),rand()*size1(3)]; 
        d1 = [d1;t1];
        t1 = [0,rand()*size1(2),rand()*size1(3)];
        d1 = [d1;t1];
        % 上下两个面
        t1 = [rand()*size1(1),size1(2),rand()*size1(3)];
        d1 = [d1;t1];
        t1 = [rand()*size1(1),0,rand()*size1(3)];
        d1 = [d1;t1];
        % 前后两个面
        t1 = [rand()*size1(1),rand()*size1(2),size1(3)];
        d1 = [d1;t1];
        t1 = [rand()*size1(1),rand()*size1(2),0];
        d1 = [d1;t1];
    end 
    % 生成第二个长方体采样点数据
    for i=1:100
        t1 = [size2(1),rand()*size2(2),rand()*size2(3)];
        d2 = [d2;t1];
        t1 = [0,rand()*size2(2),rand()*size2(3)];
        d2 = [d2;t1];
        t1 = [rand()*size2(1),size2(2),rand()*size2(3)];
        d2 = [d2;t1];
        t1 = [rand()*size2(1),0,rand()*size2(3)];
        d2 = [d2;t1];
        t1 = [rand()*size2(1),rand()*size2(2),size2(3)];
        d2 = [d2;t1];
        t1 = [rand()*size2(1),rand()*size2(2),0];
        d2 = [d2;t1];
    end 
    %d1 = (rotatoMat*d1')+repmat(Tr',1,size(d1,1));
    dt = d1;
    d1 = d1'+repmat(Tr',1,size(d1,1));      % 对原来的数据进行偏移，偏移tr的方向
    d1 = d1';
end

%%
% 导入模型
function [data] = importPoint(filename)
    data =[];
    fid = fopen(filename,'r');
    str = fgetl(fid);
    cnt = sscanf(str,'%d');
    for i=1:cnt
        str = fgetl(fid);
        tmp = sscanf(str,'%f %f %f');
        data = [data;tmp'];
        % 读取两行
        str = fgetl(fid);
        str = fgetl(fid);
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