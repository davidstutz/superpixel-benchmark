%
% Egde-Avoiding Wavelets demo
%
% comparison to regular (1st generation) wavelets is implemnented
% by setting sigma=0
%
% Shows (Guided) Edge-Aware Interpolation by colorization, detail
% enhacement and edge-preservign detail suppression.
%
% This code implements the paper "Edge-Avoiding Wavelets and their
% Applications" SIGGRAPH 2009.
%
% Code written by Raanan Fattal 


function eaw_test



%%%%%%% Speed Check

clear

wave_type = 1 ;

figure(1)
clf
colormap(gray(256)) ;

I = imread('flower.bmp');
I = double(I) / 255 ;
[w h c] = size(I) ;
nlevels = floor(log2(min(size(I(:,:,1)))))-2 ;

figure(1)


A = cell(nlevels+1,3) ;
W = cell(nlevels,3) ;

nc = 3 ;

for k=1:3
R=I ;
for i=1:nlevels
    [tA tW] = EAW(R,1,wave_type,1,1) ; % process a single level at a time
                                       % (not really needed - it's just for
                                       % this demo)
    for c=1:nc
        A{i,c} = tA{1,c} ;
        W{i,c} = tW{1,c} ;
    end
    
    R = reshape([tA{2,1} tA{2,2} tA{2,3}],size(tA{2,1},1),size(tA{2,1},2),3) ;
    
    R(R>1)=1;
    R(R<0)=0;
    imagesc(R)
    drawnow
end
title('Forward Transform Speed Check')
drawnow
end
pause(0.1)


%%%%%%% Detail Enhacement


clear

wave_type = 1 ;

figure(1)
clf
colormap(gray(256)) ;

I = imread('flower.bmp');
I = double(I) / 255 ;
[w h c] = size(I) ;
nlevels = floor(log2(min(size(I(:,:,1)))))-1 ;

figure(1)

subplot(2,3,1), imagesc(I), title('Input'), axis off ;  drawnow
I = rgb2hsv(I) ;

R = I ;
[A W] = EAW(R(:,:,3),nlevels,wave_type,1,0) ; % sigma = 0
for i=1:nlevels
    A{i,1} = A{i,1} * 1.5^(nlevels-i) ;  % enhance fine detail  
end
R(:,:,3) = iEAW(A,W,wave_type) ;

R = hsv2rgb(R) ;

R(R<0) = 0 ;
R(R>1) = 1 ;

subplot(2,3,2), imagesc(R), title('Regular Wavelets'), axis off ; drawnow

R = I ;
[A W] = EAW(R(:,:,3),nlevels,wave_type,1,1) ;
for i=1:nlevels
    A{i,1} = A{i,1} * 1.4^(nlevels-i) ;
    
end
R(:,:,3) = iEAW(A,W,wave_type) ;

R = hsv2rgb(R) ;

R(R<0) = 0 ;
R(R>1) = 1 ;

subplot(2,3,3), imagesc(R), title('Edge-Avoiding Wavelets'), axis off ;  drawnow
nlevels = floor(log2(min(size(I(:,:,1)))))-2 ;

R = I ;
[A W] = EAW(R(:,:,3),nlevels,wave_type,1,0) ;
for i=1:nlevels
    A{i,1} = A{i,1} * 0.7^(nlevels+1-i) ;  % attenuate fine detail
end
R(:,:,3) = iEAW(A,W,wave_type) ;

R = hsv2rgb(R) ;

R(R<0) = 0 ;
R(R>1) = 1 ;

subplot(2,3,5), imagesc(R), title('Regular Wavelets'), axis off ;  drawnow


R = I ;
[A W] = EAW(R(:,:,3),nlevels,wave_type,1,1) ;
for i=1:nlevels
    A{i,1} = A{i,1} * 0.7^(nlevels+1-i) ;  
end
R(:,:,3) = iEAW(A,W,wave_type) ;

R = hsv2rgb(R) ;

R(R<0) = 0 ;
R(R>1) = 1 ;

subplot(2,3,6), imagesc(R), title('Edge-Avoiding Wavelets'), axis off ;  drawnow


%%%%%%% Guided Edge-Aware Interpolation

clear
figure(2)

S = imread('gili_sc.bmp');
S = double(S) / 255 ;
G = imread('gili_bw.bmp');
G = double(G) / 255 ;
[w h c] = size(S) ;
nlevels = floor(log2(min(size(S(:,:,1))))) ;

figure(2)
subplot(2,2,1), imagesc(G), title('Input Image'), axis off, drawnow
subplot(2,2,2), imagesc(S), title('Input Scribbles'), axis off, drawnow

smth_factor = 0.125 ;

wave_type = 1 ;

[A W] = EAW(G(:,:,1),nlevels,wave_type,1,0) ; % construction wavelets weights based on guiding BW image (A not needed)

N = (abs(S(:,:,1) - S(:,:,2)) > 0.01) ; % extract scribbles 

for c=1:3
    tS = S(:,:,c) ;
    tS(~N) = 0;
    S(:,:,c) = tS ;
end

yuv = rgb2yuv(S) ; % operating on the UV of the YUV color space
U = yuv(:,:,2) ;
V = yuv(:,:,3) ;

N = double(N) ;  % normalization field

Au = gEAW(U,W,wave_type) ; % Forward transform using weights W
Av = gEAW(V,W,wave_type) ; 
An = gEAW(N,W,wave_type) ; 

for i=1:nlevels+1
    Au{i,1} = Au{i,1} * smth_factor^i ;
    Av{i,1} = Av{i,1} * smth_factor^i ;
    An{i,1} = An{i,1} * smth_factor^i ;
end

yuv(:,:,2) = igEAW(Au,W,wave_type) ; % inv transform using weight W
yuv(:,:,3) = igEAW(Av,W,wave_type) ;
N = igEAW(An,W,wave_type) ;


N(N<1e-8) = 1 ;
    
yuv(:,:,2) = yuv(:,:,2)./N;  % normalize (like Shepard method)
yuv(:,:,3) = yuv(:,:,3)./N;

Y = rgb2yuv(G) ;
yuv(:,:,1) = Y(:,:,1) ; % retrieve old Y channel
C = yuv2rgb(yuv) ;

C(C<0) = 0 ;
C(C>1) = 1 ;

subplot(2,2,3), imagesc(C), title('Regular Wavelets'), axis off, drawnow



[A W] = EAW(G(:,:,1),nlevels,wave_type,1,1) ; 

N = (abs(S(:,:,1) - S(:,:,2)) > 0.01) ; % extracting scribbles 

for c=1:3
    tS = S(:,:,c) ;
    tS(~N) = 0;
    S(:,:,c) = tS ;
end

yuv = rgb2yuv(S) ; % operating on the UV of the YUV color space
U = yuv(:,:,2) ;
V = yuv(:,:,3) ;

N = double(N) ;  % normalization field

Au = gEAW(U,W,wave_type) ; 
Av = gEAW(V,W,wave_type) ; 
An = gEAW(N,W,wave_type) ; 

for i=1:nlevels+1
    Au{i,1} = Au{i,1} * smth_factor^i ;
    Av{i,1} = Av{i,1} * smth_factor^i ;
    An{i,1} = An{i,1} * smth_factor^i ;
end

yuv(:,:,2) = igEAW(Au,W,wave_type) ;
yuv(:,:,3) = igEAW(Av,W,wave_type) ;
N = igEAW(An,W,wave_type) ;

N(N<1e-8) = 1 ;
    
yuv(:,:,2) = yuv(:,:,2)./N;  % normalize (like Shepard method)
yuv(:,:,3) = yuv(:,:,3)./N;


Y = rgb2yuv(G) ;
yuv(:,:,1) = Y(:,:,1) ; % retrieve old Y channel
C = yuv2rgb(yuv) ;

C(C<0) = 0 ;
C(C>1) = 1 ;

subplot(2,2,4), imagesc(C), title('Edge-Avoiding Wavelets'), axis off, drawnow




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function J = rgb2yuv(I)

M = [0.299 0.587 0.114 ; -0.147 -0.289 0.436 ; 0.615 -0.515 -0.1] ;

for c=1:3
    J(:,:,c) = M(c,1) * I(:,:,1) + M(c,2) * I(:,:,2) + M(c,3) * I(:,:,3) ;
end

function J = yuv2rgb(I)

M = inv([0.299 0.587 0.114 ; -0.147 -0.289 0.436 ; 0.615 -0.515 -0.1]) ;

for c=1:3
    J(:,:,c) = M(c,1) * I(:,:,1) + M(c,2) * I(:,:,2) + M(c,3) * I(:,:,3) ;
end
