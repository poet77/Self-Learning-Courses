f=@(x)exp(-x*x);
g=@(x)(1/(1+x*x));
h=@(x)(1/(2+cos(x)));
f1=@(x)exp(-(0.5+0.5*x)*(0.5+0.5*x));

int11=zeros(7,1);
int12=zeros(7,1);
int13=zeros(7,1);
int21=zeros(7,1);
int22=zeros(7,1);
int23=zeros(7,1);
error11=zeros(7,1);
error12=zeros(7,1);
error13=zeros(7,1);
error21=zeros(7,1);
error22=zeros(7,1);
error23=zeros(7,1);
order11=zeros(7,1);
order12=zeros(7,1);
order13=zeros(7,1);
order21=zeros(7,1);
order22=zeros(7,1);
order23=zeros(7,1);

for k=1:7
    
    N=2^k;

    int11(k)=T(f,0,1,2^k);
    int12(k)=T(g,0,4,2^k);
    int13(k)=T(h,0,2*pi,2^k);
 int21(k)=G(f,0,1,2^k);
 int22(k)=G(g,0,4,2^k);
 int23(k)=G(h,0,2*pi,2^k);

    error11(k)=abs(0.5*sqrt(pi)*erf(1)-int11(k));
    error12(k)=abs(atan(4)-int12(k));
    error13(k)=abs(2*pi/(sqrt(3))-int13(k));
     error21(k)=abs(0.5*sqrt(pi)*erf(1)-int21(k));
    error22(k)=abs(atan(4)-int22(k));
    error23(k)=abs(2*pi/(sqrt(3))-int23(k));

    if(k>1)
      order11(k)=log(error11(k-1)/error11(k))/log(2);
      order12(k)=log(error12(k-1)/error12(k))/log(2);
      order13(k)=log(error13(k-1)/error13(k))/log(2);
      order21(k)=log(error21(k-1)/error21(k))/log(2);
      order22(k)=log(error22(k-1)/error22(k))/log(2);
      order23(k)=log(error23(k-1)/error23(k))/log(2);
    end

end
for k=1:7
    t=2^k;

    disp(["N is:" t]);
    disp(["l1:error of trapezium error is:" error11(k)]);
    disp(["l1:order of trapezium error is:" order11(k)]);
    disp(["l2:error of trapezium error is:" error12(k)]);
    disp(["l2:order of trapezium error is:" order12(k)]);
    disp(["l3:error of trapezium error is:" error13(k)]);
    disp(["l3:order of trapezium error is:" order13(k)]);
    disp(["1l:error of Gauss error is:" error21(k)]);
    disp(["1l:order of Gauss error is:" order21(k)]);
    disp(["12:error of Gauss error is:" error22(k)]);
    disp(["12:order of Gauss error is:" order22(k)]);
    disp(["13:error of Gauss error is:" error23(k)]);
    disp(["13:order of Gauss error is:" order23(k)]);
end

