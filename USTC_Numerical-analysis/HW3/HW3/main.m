N=6;
xx=zeros(N,1);
yy=zeros(N,1);
y0=zeros(N-1,1);
error=zeros(N-1,1);

for i=1:N
    xx(i)=(i-1)/(N-1);
    yy(i)=exp(xx(i));
end

for i=1:N-1
    y0(i)=sol_linear(xx,yy,(xx(i)+xx(i+1))/2,N);
end

for i=1:N-1
    error(i)=abs(y0(i)-exp((xx(i)+xx(i+1))/2));
end


ex=zeros(N-1,1);


for i=1:N-1
    ex(i)=abs(cubicspline(xx,yy,(xx(i)+xx(i+1))/2)-exp((xx(i)+xx(i+1))/2));
end

t1=max(error);
t_1=max(ex);



N=11;
xx=zeros(N,1);
yy=zeros(N,1);
y0=zeros(N-1,1);
error=zeros(N-1,1);

for i=1:N
    xx(i)=(i-1)/(N-1);
    yy(i)=exp(xx(i));
end

for i=1:N-1
    y0(i)=sol_linear(xx,yy,(xx(i)+xx(i+1))/2,N);
end

for i=1:N-1
    error(i)=abs(y0(i)-exp((xx(i)+xx(i+1))/2));
end


ex=zeros(N-1,1);

for i=1:N-1
    ex(i)=abs(cubicspline(xx,yy,(xx(i)+xx(i+1))/2)-exp((xx(i)+xx(i+1))/2));
end

t2=max(error);
t_2=max(ex);

ord11=log(t1/t2)/log(10/5);
ord21=log(t_1/t_2)/log(10/5);


N=21;
xx=zeros(N,1);
yy=zeros(N,1);
y0=zeros(N-1,1);
error=zeros(N-1,1);

for i=1:N
    xx(i)=(i-1)/(N-1);
    yy(i)=exp(xx(i));
end

for i=1:N-1
    y0(i)=sol_linear(xx,yy,(xx(i)+xx(i+1))/2,N);
end

for i=1:N-1
    error(i)=abs(y0(i)-exp((xx(i)+xx(i+1))/2));
end

ex=zeros(N-1,1);

for i=1:N-1
    ex(i)=abs(cubicspline(xx,yy,(xx(i)+xx(i+1))/2)-exp((xx(i)+xx(i+1))/2));
end

t3=max(error);
t_3=max(ex);

ord12=log(t2/t3)/log(20/10);
ord22=log(t_2/t_3)/log(20/10);

N=41;
xx=zeros(N,1);
yy=zeros(N,1);
y0=zeros(N-1,1);
error=zeros(N-1,1);

for i=1:N
    xx(i)=(i-1)/(N-1);
    yy(i)=exp(xx(i));
end

for i=1:N-1
    y0(i)=sol_linear(xx,yy,(xx(i)+xx(i+1))/2,N);
end

for i=1:N-1
    error(i)=abs(y0(i)-exp((xx(i)+xx(i+1))/2));
end


ex=zeros(N-1,1);

for i=1:N-1
    ex(i)=abs(cubicspline(xx,yy,(xx(i)+xx(i+1))/2)-exp((xx(i)+xx(i+1))/2));
end


t4=max(error);
t_4=max(ex);
ord13=log(t3/t4)/log(40/20);
ord23=log(t_3/t_4)/log(40/20);

disp('N=5');
disp(["Max Error of Method (1) :"  t1] );
disp(["Max Error of Method (2) :"  t_1 ]);

disp('N=10');
disp(["Max Error of Method (1) :"  t2] );
disp(["Max Error of Method (2) :"  t_2 ]);
disp(["order of Method (1) :" ord11]);
disp(["order of Method (2) :" ord21]);

disp('N=20');
disp(["Max Error of Method (1) :"  t3] );
disp(["Max Error of Method (2) :"  t_3] );
disp(["order of Method (1) :" ord12]);
disp(["order of Method (2) :" ord22]);

disp('N=40');
disp(["Max Error of Method (1) :"  t4] );
disp(["Max Error of Method (2) :"  t_4 ]);
disp(["order of Method (1) :" ord13]);
disp(["order of Method (2) :" ord23]);




