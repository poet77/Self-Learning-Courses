fun1=@(x)(log(x));
x1=3;max1=4;

D1=diffext1(fun1,x1,max1);

fun2=@(x)(tan(x));
x2=asin(0.8);max2=5;

D2=diffext1(fun2,x2,max2);

fun3=@(x)(sin(x^2+x/3));
x3=0;max3=6;

D3=diffext1(fun3,x3,max3);




digits(6);

disp(vpa(D1));
disp(vpa(D2));
disp(vpa(D3));


