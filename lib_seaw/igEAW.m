%
% Backward Guided Edge-Avoiding Wavelet Transform
%
% Input:  
% A - (cell array) approximation and detail coeficients
% W - (cell array) wavelets weights
% wavelet_type - 0 for seperable wavelets and 1 for Red-Black wavelet
%
% Output:
% I - 2D image
%
% This code implements the paper "Edge-Avoiding Wavelets and their
% Applications" from SIGGRAPH 2009.
%
% Code written by Raanan Fattal 

function I = igEAW(A,W,wavelet_type)

nc = size(A,2) ;

nlevels = size(W,1) ;

for c=1:nc
    J = A{nlevels+1,c} ;
   
    for i=nlevels:-1:1
        Jt = zeros(size(A{i,c})) ;
        Jt(1:2:end,1:2:end) = J  ;
        Jt = Jt + A{i,c} ;
        J = igeaw(Jt,W{i,c},wavelet_type) ;     
    end
    I(:,:,c) = J ;
end

