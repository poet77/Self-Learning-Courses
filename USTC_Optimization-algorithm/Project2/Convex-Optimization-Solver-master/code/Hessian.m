% This is the function used to calculate the hessian matrix

function [hess]=Hessian(f,x,epsilon)
 %   indexat = @(expr, index) expr(index);
    dim=size(x,1);
    hess=zeros(dim,dim);
    for row=1:dim
        x_1=x;
        x_0=x;
        x_1(row)=x_1(row)+epsilon;
        x_0(row)=x_0(row)-epsilon;
        grad_1=Gradient(f,x_1,epsilon);
        grad_0=Gradient(f,x_0,epsilon);
        hess(row,:)=(grad_1-grad_0)/(2*epsilon);
    end
end