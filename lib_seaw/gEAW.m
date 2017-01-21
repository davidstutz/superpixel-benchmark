%
% Guided Edge-Avoiding Wavelet Transform
%
% Input:
% I - 2D image (must have its values between 0 and 1)
% W - Guided wavelets weights
% wavelet_type - 0 for seperable wavelets and 1 for Red-Black wavelets
%
% Output:
% A - (cell array) approximation and detail coeficients 
%
% This code implements the paper "Edge-Avoiding Wavelets and their
% Applications" from SIGGRAPH 2009.
%
% Code written by Raanan Fattal

function [A W] = gEAW(I, W, wavelet_type)

nlevels = size(W,1) ;

nc = size(I,3) ;

A = cell(nlevels+1,nc) ;

for c=1:nc
    J = I(:,:,c) ;
    A{1,c} = J ;
     for i=1:nlevels
        J = geaw(J,W{i,c},wavelet_type) ;
        J = J(1:2:end,1:2:end) ;
        A{i+1,c} = J ;
        
        
    end
end

