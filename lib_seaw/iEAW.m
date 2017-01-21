%
% Backward Edge-Avoiding Wavelet Transform
%
% Input:  
% A - (cell array) approximation and detail coeficients
% W - (cell array) wavelets weights
% wavelet_type - 0 for seperable wavelets and 1 for Red-Black wavelets
%
% Output:
% I - 2D image
%
% This code implements the paper "Edge-Avoiding Wavelets and their
% Applications" from SIGGRAPH 2009.
%
% Code written by Raanan Fattal 

function I = iEAW(A,W,wavelet_type)

nc = size(A,2) ;

nlevels = size(W,1) ;

for c=1:nc
    J = A{nlevels+1,c} ;

    for i=nlevels:-1:1    
        A{i,c}(1:2:end,1:2:end) = J ;
    
        J = ieaw(A{i,c},W{i,c},wavelet_type) ;
    end
    I(:,:,c) = J ;
end

