% function [V,S] = cncut(W,U,nv,reg)
% Input:
%   W = N x N affinity matrix, negative entries treated as repulsion
%   U = N x C constraint matrix, default = []
%   nv = number of eigenvectors, default = 6
%   reg = regularization factor, default = 0
% Output:
%   V = N x nv, eigenvectors
%   S = nv x 1, eigenvalue of (W,D) s.t. U' * V = 0.
% Reference:
%   www.cs.berkeley.edu/~stellayu/thesis.html
%   Chapter 5: Grouping with Bias

% Stella X. Yu, July 11 2003.

function [V,S] = cncut(W,U,nv,reg)

np = size(W,1);

if nargin<2,
    U = [];
end

if nargin<3 | isempty(nv),
    nv = min(6,np);
end

if nargin<4 | isempty(reg),
    reg = 0;
end

W = sparse(W);
if not(isequal(W,W')),
    W = W + W';
end

% degrees and regularization
d = sum(abs(W),2);
dr = 0.5 * (d - sum(W,2));    
if reg>0,    
    d = d + reg * 2;
    dr = dr + reg;
end
W = W + spdiags(dr,0,np,np);
clear dr

% eigencuts
dih = 1./(sqrt(d)+eps);
W = spmd1(W,dih,dih);

% remove ill-conditioned constraints
if ~isempty(U),
    U = sparse(U);
    U = spmd1(U,[],1./full(eps+max(abs(U),[],1)));
    j = find(sum(abs(U),1)>1.005);
    i = [length(j),size(U,2)];
    if i(1)<i(2),
        disp(sprintf('%s: %d / %d constraints OK',mfilename,i));
    end
    U = U(:,j);
end

options.issym = 1;
options.disp = 0;
if isempty(U), 
    [V,S] = eigs(W,nv,'LM',options);
else   
    A = spmd1(U,dih,[]);
    B = inv(A'*A);
    [V,S] = eigs(@barqpqz,np,nv,'LM',options,W,A,B);
end    
[i,j] = sort(diag(S)); 
S = i(end:-1:1);
V = V(:,j(end:-1:1)); 
V = V .* repmat(dih,1,nv);
