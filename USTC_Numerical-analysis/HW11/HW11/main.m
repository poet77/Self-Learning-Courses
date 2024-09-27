u1=@(t)0;
u2=@(t)(2*exp(t));
v=@(t)-1;
w=@(t)0;

f=@(t)(7*sin(t)+3*cos(t));
g=@(t)(exp(t)+cos(t));


[y1]=differ(0,pi/2,10,3,7,u1,v,w,f);
[y2]=differ(0,pi/2,20,3,7,u1,v,w,f);
[y3]=differ(0,pi/2,40,3,7,u1,v,w,f);
[y4]=differ(0,pi/2,80,3,7,u1,v,w,f);
[y5]=differ(0,pi/2,160,3,7,u1,v,w,f);

[z1]=differ(0,1,10,2,(exp(1)+cos(1)),u2,v,w,g);
[z2]=differ(0,1,20,2,(exp(1)+cos(1)),u2,v,w,g);
[z3]=differ(0,1,40,2,(exp(1)+cos(1)),u2,v,w,g);
[z4]=differ(0,1,80,2,(exp(1)+cos(1)),u2,v,w,g);
[z5]=differ(0,1,160,2,(exp(1)+cos(1)),u2,v,w,g);

error11=max(y1);
error12=max(y2);
error13=max(y3);
error14=max(y4);
error15=max(y5);

error21=max(z1);
error22=max(z2);
error23=max(z3);
error24=max(z4);
error25=max(z5);


order12=log(error11/error12)/log(2);
order13=log(error12/error13)/log(2);
order14=log(error13/error14)/log(2);
order15=log(error14/error15)/log(2);


order22=log(error21/error22)/log(2);
order23=log(error22/error23)/log(2);
order24=log(error23/error24)/log(2);
order25=log(error24/error25)/log(2);

