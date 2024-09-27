syms x;
int1=zeros(8,1);
int2=zeros(8,1);
error1=zeros(8,1);
error2=zeros(8,1);
for k=1:8
    N=5*k;
    sym1=0;
    sym2=0;
    xi1=zeros(N+1,1);
    xi2=zeros(N+1,1);
    yi1=zeros(N+1,1);
    yi2=zeros(N+1,1);
    poly1=zeros(N+1,1);
    poly2=zeros(N+1,1);

    for j=0:N
      xi1(j+1)=1-2*j/N;
      xi2(j+1)=-cos(pi*(j+1)/(N+2));
      yi1(j+1)=1/(1+25*xi1(j+1)^2);
      yi2(j+1)=1/(1+25*xi2(j+1)^2);
    end

    for i=1:N+1
     t=1;
     q=1;
    for p=1:N+1
        if p~=i
            t=t*(x-xi1(p))/(xi1(i)-xi1(p));
            q=q*(x-xi2(p))/(xi2(i)-xi2(p));
        end
    end


    
    g = matlabFunction(t);
    ab= matlabFunction(q);


    sum11 = 0;
   sum12 = 0;
   sum21 = 0;
    sum22 = 0;
    a=-1;
    b=1;
    h=0.002;
    for c = 0:999
        z=a+h*(c+1/2);
        sum11 = sum11 + g(z);
        sum12 = sum12 + ab(z);
    end
    for j = 1:999
        y=a+h*j;
       sum21 = sum21 + g(y);
        sum22 = sum22 + ab(y);
   end
    poly1(i) = h/6*(g(a)+4*sum11+2*sum21+g(b));
    poly2(i) = h/6*(ab(a)+4*sum12+2*sum22+ab(b));

    sym1=sym1+poly1(i)*yi1(i);
    sym2=sym2+poly2(i)*yi2(i);

    end

    int1(k)=sym1;
    int2(k)=sym2;
    error1(k)=abs(int1(k)-0.4*atan(5));
    error2(k)=abs(int2(k)-0.4*atan(5));

    
    disp(["N is:" N]);
    disp(["integral of first nodes is:" int1(k)]);
    disp(["integral of second nodes is:" int2(k)]);
    disp(["integral of the function is:" 0.4*atan(5)]);
    disp(["error of first nodes is:" error1(k)]);
    disp(["error of second nodes is:" error2(k)]);


end


