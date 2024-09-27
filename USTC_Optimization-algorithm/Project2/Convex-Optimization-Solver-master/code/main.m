%This is a test sample to solve the problem
% min_x f(x) 
% s.t. Cx = d
% g_i(x) <= 0 for all i.


%If you already have CVX installed
CVX_CHECK='ON'; %Set it to 'ON' to verify our solution with CVX's solution

diff_eps = 1e-6;         %finite difference to calculate the numerical gradient
stopping_eps = 1e-6;            %stopping check (xnew -x) < epsilon, terminate. 
alpha =  0.4; % in (0, 0.5)  -- backtracking line search parameter
beta = 0.7;   % in (0,1)     -- backtracking line search parameter


%C = [ 0.189653747547175   0.682223223591384   0.541673853898088];
C = [ -1.01  1.01 2];
d = 1;

%C=[0.1211 0.5431 0.6578];
%C=[0.99 0.111 0.867];

%d=4;


indexat = @(expr, index) expr(index); %helper function to encode inequality constraints. 

%fi{1}=@(x)(exp(x(1))-x(2)-log(1+x(3))); %This is our OBJECTIVE function. 2 other samples are given below to try

fi{1} = @(x)(exp(x(1)*6) + exp(x(2)*1) +exp(x(3))- 4); 
%fi{1}=@(x)(-2*log(x(1)+1)+3*x(2)^2+x(3)^2);
%fi{1}=@(x)(x(1)*x(1)+2*x(2)*x(2)+x(3)*x(3)-2*x(1)*x(2)+x(3));

%Now, we have some sample inequality constraints to try! In this problem,
%we have constraints of the form x_low<=x<=x_high, box constraints. Feel
%free to modify for other types, like quadratic constraints.

x_low = -1*ones(3,1); 
x_high = 1*ones(3,1); 



%fi{2}=@(x)indexat((x-1),1);
%fi{3}=@(x)indexat((x-1),2);
%fi{4}=@(x)indexat((x-1),3);
%fi{5}=@(x)indexat((-x-1),1);
%fi{6}=@(x)indexat((-x-1),1);
%fi{7}=@(x)indexat((-x-1),1);

fi{2}=@(x)indexat((0.4*x-0.11),1); 
fi{3}=@(x)indexat((0.5*x.^2-0.91),2); 
fi{4}=@(x)indexat((x.^2-1),3); 
fi{5}=@(x)indexat((x.^2-1),1); 
fi{6}=@(x)indexat((x.^2-0.87),2); 
fi{7}=@(x)indexat((x-1),3);  




x0_feasible = C\d; %only satisfies the equality constraint, not necessarily the inequality one.

% % we need to first find a feasible point to initialize our actual proble

%We implement phase 1!

%For this, we implemented Phase1
% min s
% subject to
% g_i(x)<=s for all i
% C*x=d

[x_feasible, feasibility] = Firstphase_method(fi, C, x0_feasible, diff_eps, stopping_eps, alpha, beta);% YOURS to implement

%The final element of x_feasible containts the value of s, we can discard
%it.
x_feasible=x_feasible(1:size(x_feasible)-1);

if feasibility==0
    error('Infeasible problem') %if problem is infeasible, error is called!
end


% % now let's solve our actual problem:




[x_sol,count] = Newton_method(fi,C,x_feasible, diff_eps, stopping_eps, alpha, beta);% YOURS to implement


if strcmp(CVX_CHECK,'ON')
   
    cvx_begin
     variable x(3);
 %  fi{1}=@(x)(exp(x(1))-x(2)-log(1+x(3)));
 fi{1} = @(x)(exp(x(1)*6) + exp(x(2)*1) +exp(x(3))- 4); 
% fi{1}=@(x)(-2*log(x(1)+1)+3*x(2)^2+x(3)^2);
fi{2}=@(x)indexat((0.4*x-0.11),1); 
fi{3}=@(x)indexat((0.5*x.^2-0.91),2); 
fi{4}=@(x)indexat((x.^2-1),3); 
fi{5}=@(x)indexat((x.^2-1),1); 
fi{6}=@(x)indexat((x.^2-0.87),2); 
fi{7}=@(x)indexat((x-1),3);  
 
   % minimize (exp(x(1))-x(2)-log(1+x(3)));  %change this according to the function being verified
   minimize fi{1}(x);
   subject to
   C*x==d;  %equality constraint
   x_low<=x<=x_high;
   fi{2}(x)<=0;
   fi{3}(x)<=0;
   fi{4}(x)<=0;
   fi{5}(x)<=0;
   fi{6}(x)<=0;
   fi{7}(x)<=0;
   cvx_end
 
fprintf('CVX found optimal value as %d and the solution is \n',fi{1}(x))
disp(x);
end

fprintf('The optimal value as %d and the solution is \n',fi{1}(x_sol))
disp(x_sol);

fprintf('The number of iterations is %d \n',count)



