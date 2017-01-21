%
% Forward Edge-Avoiding Wavelet Transform
%
% Input: 
% I - 2D image (must have its values between 0 and 1)
% nlevels - number of transformation levels (each is full octave)
% wavelet_type - 0 for seperable wavelets and 1 for Red-Black wavelets
% dist_func - 0 for exp(-(I-J)^2/sigma^2) and 1 for 1/(|I-J|^sigma+eps)
% sigma - sets the range scale used in the pixel-range distance function
% 
% Output: 
% A - (cell array) approximation and detail coeficients (approx.
% not really needed), 
% W - (cell array) wavelets weights
%
% This code implements the paper "Edge-Avoiding Wavelets and their
% Applications" from SIGGRAPH 2009.
%
% Code written by Raanan Fattal 

function [A W] = EAW(I, nlevels, wavelet_type, dist_func, sigma)

nc = size(I,3) ;

A = cell(nlevels+1,nc) ;
W = cell(nlevels,nc) ;  

for c=1:nc
    J = I(:,:,c) ;
    for i=1:nlevels
        [J tW] = eaw(J,wavelet_type,dist_func,sigma) ;
        A{i,c} = J ;
        J = J(1:2:end,1:2:end) ;
        W{i,c} = tW ;
    end

A{nlevels+1,c} = J ;
end

