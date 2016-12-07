% function [X,R,Z,iter] = getbinsol(V,n_iter,X0)
%   iteratively solving X = \tV R, R R' = I
% Input:
%   V = N x K (#nodes x #segments), eigensolutions from cncut.m
%   n_iter = # iterations for refinement. default = 50; 
%      n_iter = 0, no refinement
%   X0 = initial estimation of a partitioning
% Output:
%   X = N x K, K-way discrete partition matrix
%   R = best orthonormal matrix applied to V
%   Z = V R, best continuous partitioning
%   iter = # iterations actually used
% Reference:  
%   www.cs.berkeley.edu/~stellayu/thesis.html
%   Chapter 2: Multiclass Spectral Clustering

% Stella X. Yu, Feb 2003.


function [X,R,Z,iter] = getbinsol(V,n_iter,X0);

if nargin<2,
    n_iter = 50;
end

[np,ns] = size(V);
vm = sqrt(sum(V.*V,2));
V = V./repmat(vm+eps,1,ns);

% initialization
if nargin==3,
    [i,j] = max(X0,[],2);
    [UB,sigma,UA] = svd(parmatV(V,j,ns));     
    R = UA * UB';    
else
    R = zeros(ns);
    [i,i] = max(vm);
    %i = ceil(rand*np);
    R(:,1) = V(i,:)';
    z = zeros(np,1);
    for k=2:ns,
        corr = V * R(:,k-1);
        z = z + abs(corr);
        [j,i] = min(z);
        R(:,k) = V(i,:)';
    end
end

olderr = 0;
th = eps * ns;
for iter=0:n_iter,
    Z = V * R;      
    [i,j] = max(Z,[],2);    
    [UB,sigma,UA] = svd(parmatV(V,j,ns));     
    err = trace(sigma);
    if abs(err-olderr)<th,
        break;
    end
    olderr = err;    
    R = UA * UB';    
end
X = sparse([1:np]',j,ones(np,1),np,ns);
