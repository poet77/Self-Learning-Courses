T1=zeros(12,1);
Sn1=zeros(12,1);
error1=zeros(12,1);
error2=zeros(12,1);
order1=zeros(12,1);
order2=zeros(12,1);
f=@(x)sin(x);

for k=1:12
    N=2^k;
    x=linspace(0,4,N+1);
    T1(k)=T(f,0,4,2^k);
   
    Sn1(k)=Sn(f,x);
    error1(k)=abs(T1(k)-1+cos(4));
    error2(k)=abs(Sn1(k)-1+cos(4));
    if(k>1)
      order1(k)=log(error1(k-1)/error1(k))/log(2);
      order2(k)=log(error2(k-1)/error2(k))/log(2);
    end

end

for k=1:12
    t=2^k;
    disp(["N is:" t]);
    disp(["error of trapezium error is:" error1(k)]);
    disp(["order of trapezium error is:" order1(k)]);
    disp(["error of Simpson error is:" error2(k)]);
    disp(["order of Simpson error is:" order2(k)]);
end