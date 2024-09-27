% This is the function used to calculate the gradient

function [grad]=Gradient(f,x,epsilon)
%    indexat = @(expr, index) expr(index);
    dim=size(x,1);
    grad=zeros(dim,1);
    for k=1:dim
        x_1=x;
        x_0=x;
        x_1(k)=x_1(k)+epsilon;
        x_0(k)=x_0(k)-epsilon;
        grad(k)=(f(x_1)-f(x_0))/(2*epsilon);
    end