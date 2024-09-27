error=zeros(101,2,4);
xi=zeros(101,1);
p1=zeros(101,1); %#ok
p2=zeros(101,1); %#ok
fi=zeros(101,1);
N=5;
x1=zeros(N+1,1);
y1=zeros(N+1,1);
f1=zeros(101,1); %#ok
x2=zeros(N+1,1);
y2=zeros(N+1,1);
t=2*N+2;

for i=0:N
    x1(i+1)=5-(10/N)*i;
    y1(i+1)=1/(1+x1(i+1)*x1(i+1));
    x2(i+1)=-5*cos(pi*((2*i+1)/t));
    y2(i+1)=1/(1+x2(i+1)*x2(i+1));
end

for i=0:100
    xi(i+1)=i/10-5;
    fi(i+1)=1/(1+xi(i+1)*xi(i+1));
end
p1=Lagrange(x1,y1,xi);
p2=Lagrange(x2,y2,xi);

for i=0:100
    error(i+1,1,1)=abs(p1(i+1)-fi(i+1));
    error(i+1,2,1)=abs(p2(i+1)-fi(i+1));
end


N=10;

x1=zeros(N+1,1);
y1=zeros(N+1,1);
f1=zeros(101,1);
x2=zeros(N+1,1);
y2=zeros(N+1,1);
t=2*N+2;

for i=0:N
    x1(i+1)=5-(10/N)*i;
    y1(i+1)=1/(1+x1(i+1)*x1(i+1));
    x2(i+1)=-5*cos(pi*((2*i+1)/t));
    y2(i+1)=1/(1+x2(i+1)*x2(i+1));
end

for i=0:100
    xi(i+1)=i/10-5;
    fi(i+1)=1/(1+xi(i+1)*xi(i+1));
end
p1=Lagrange(x1,y1,xi);
p2=Lagrange(x2,y2,xi);


for i=0:100
    error(i+1,1,2)=abs(p1(i+1)-fi(i+1));
    error(i+1,2,2)=abs(p2(i+1)-fi(i+1));
end




N=20;

x1=zeros(N+1,1);
y1=zeros(N+1,1);
f1=zeros(101,1);
x2=zeros(N+1,1);
y2=zeros(N+1,1);
t=2*N+2;

for i=0:N
    x1(i+1)=5-(10/N)*i;
    y1(i+1)=1/(1+x1(i+1)*x1(i+1));
    x2(i+1)=-5*cos(pi*((2*i+1)/t));
    y2(i+1)=1/(1+x2(i+1)*x2(i+1));
end

for i=0:100
    xi(i+1)=i/10-5;
    fi(i+1)=1/(1+xi(i+1)*xi(i+1));
end
p1=Lagrange(x1,y1,xi);
p2=Lagrange(x2,y2,xi);


for i=0:100
    error(i+1,1,3)=abs(p1(i+1)-fi(i+1));
    error(i+1,2,3)=abs(p2(i+1)-fi(i+1));
end

plot(xi,p1);
hold on;
plot(xi,p2);

N=40;

x1=zeros(N+1,1);
y1=zeros(N+1,1);
f1=zeros(101,1);
x2=zeros(N+1,1);
y2=zeros(N+1,1);
t=2*N+2;

for i=0:N
    x1(i+1)=5-(10/N)*i;
    y1(i+1)=1/(1+x1(i+1)*x1(i+1));
    x2(i+1)=-5*cos(pi*((2*i+1)/t));
    y2(i+1)=1/(1+x2(i+1)*x2(i+1));
end

for i=0:100
    xi(i+1)=i/10-5;
    fi(i+1)=1/(1+xi(i+1)*xi(i+1));
end
p1=Lagrange(x1,y1,xi);
p2=Lagrange(x2,y2,xi);


for i=0:100
    error(i+1,1,4)=abs(p1(i+1)-fi(i+1));
    error(i+1,2,4)=abs(p2(i+1)-fi(i+1));
end

e=max(error);

format long
e;

disp('N=5');
disp(["Max Error of grid (1) :"  e(1,1,1)] );
disp(["Max Error of grid (2) :"  e(1,2,1)] );

disp('N=10');
disp(["Max Error of grid (1) :"  e(1,1,2)] );
disp(["Max Error of grid (2) :"  e(1,2,2)] );

disp('N=20');
disp(["Max Error of grid (1) :"  e(1,1,3)] );
disp(["Max Error of grid (2) :"  e(1,2,3)] );

disp('N=40');
disp(["Max Error of grid (1) :"  e(1,1,4)] );
disp(["Max Error of grid (2) :"  e(1,2,4)] );


















