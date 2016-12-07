function [imgMasks,imgMarkup,segOutline,treeImg]= ...
    imgsegpyr(img,scale,stop,levels,uplevel,algFlag,volFlag,connect) 
%Function [imgMasks,imgMarkup,segOutline,treeImg]= ...
%   imgsegpyr(img,scale,stop,levels,uplevel,algFlag,volFlag,connect)
%computes the segmentation on a connected pyramid.
%
%Inputs:     img - The image to segmented 
%            scale - The scale parameter of the algorithm
%            stop - The stop parameter of the algorithm 
%            levels - Optional number of levels in the pyramid (Default is
%                max levels possible).  Note: Levels should include the
%                bottom level (i.e. the total number)
%            uplevel - Option flag determining method for obtaining higher
%                values in the pyramid
%                0: Average of lower level neighbors (Default)
%                1: Median of lower level neighbors
%            algFlag - Optional flag specifying the segmentation 
%                algorithm to use.
%                0: Isoperimetric (Default)
%                1: Spectral
%            volFlag - Optional flag specifying which notion of volume to 
%                use
%                0: Degree  i.e. vol = sum(degree_of_neighbors) (Default)
%                1: Uniform  i.e. vol = 1
%            connect - Optional flag specifies topology of the lattice.  
%                0: 4-connect 
%                1: 8-connect
%                K>1: radially connected with radius K   
%        
% 
%Outputs:   imgMasks - A cell array where every cell contains an image for
%                      each level of the pyramid that marks every pixel 
%                      with an integer that marks that pixel's segment 
%                      membership
%           imgMarkup - A cell array where every cell contains an image 
%                      for each level of the pyramid where the segments 
%                      are outlined in red; superimposed on the original 
%                      image at that level
%           segOutline - A cell array where every cell contains an image 
%                      for each level of the pyramid where the segments 
%                      are outlined in black against a white background
%           treeImg - An image displaying all levels of the tree with 
%                      their respective segments outlined in red
%
%
%
%5/24/03 - Leo Grady

% Copyright (C) 2002, 2003 Leo Grady <lgrady@cns.bu.edu>
%   Computer Vision and Computational Neuroscience Lab
%   Department of Cognitive and Neural Systems
%   Boston University
%   Boston, MA  02215
%
% This program is free software; you can redistribute it and/or
% modify it under the terms of the GNU General Public License
% as published by the Free Software Foundation; either version 2
% of the License, or (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program; if not, write to the Free Software
% Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
%
% Date - $Id: imgsegpyr.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Read image
[X Y Z]=size(img);
N=X*Y;

%Set defaults
if nargin == 3
    %Top level no smaller than 30x30
    levels=min(round(log2(X)-log2(30)),round(log2(Y)-log2(30))) 
    [uplevel,algFlag,volFlag,connect]=deal(0);
elseif nargin == 4
    [uplevel,algFlag,volFlag,connect]=deal(0);
elseif nargin == 5
    [algFlag,volFlag,connect]=deal(0);
elseif nargin == 6
    [volFlag,connect]=deal(0);
elseif nargin == 7
    connect=0;
end 

%Read and normalize image
if Z == 3
    vals=rgbimg2vals(img);
else
    vals=img(:);    
end
vals=normalize(vals);

%Create pyramid
[levelChildren,levelIndex,points,edges,edgeBias,levels]= ...
    latticepyramid(X,Y,levels);

%Import image to levels
for s=1:Z
    for k=1:(levels-1)
        tmp=vals(:,s);
        if uplevel
            %Propogate image via median
            newLevel=median(tmp(levelChildren(:, ...
                levelIndex(k):(levelIndex(k+1)-1))))';
        else
            newLevel=mean(tmp(levelChildren(:, ...
                levelIndex(k):(levelIndex(k+1)-1))))';
        end
        %Normalize new level data
        newLevel=normalize(newLevel);
        
        %Add new level data
        vals=[vals;newLevel];
    end
end

%Find weights
weights=makeweights(edges,vals,scale);

%Build Adjacency matrix
W=adjacency(edges,weights);

%Perform segmentation
segAnswer=recursivepartition(W,stop,algFlag,volFlag);

%Generate output for lowest level
if Z == 3
    img(:,:,1)=reshape(vals(1:N,1));
    img(:,:,2)=reshape(vals(1:N,2));
    img(:,:,3)=reshape(vals(1:N,3));
else
    img=reshape(vals(1:N,1),[X Y]);
end
[imgMasks{1},segOutline{1},imgMarkup{1}]=segoutput(img,segAnswer(1:N));
[Xlist,Ylist,dummy]=size(imgMarkup{1});

%Parse solution by level and generate output
currX=X;
currY=Y;
for k=1:(levels-1)
    currX=floor(currX/2);
    currY=floor(currY/2);    
    currLevel=levelIndex(k):(levelIndex(k+1)-1);
    imgLevel=reshape(vals(N+currLevel,:),[currX currY]);
    imgLevel=imresize(imgLevel,[currX currY]);
    [imgMasks{k+1},segOutline{k+1},imgMarkup{k+1}]= ...
        segoutput(imgLevel,segAnswer(N+currLevel));
    [Xlist(k+1),Ylist(k+1),dummy]=size(imgMarkup{k+1});    
end
bigLevels=levels;

%Create composite image
treeImg=zeros(sum(Xlist),Ylist(1),3);
middleY=floor(Ylist(1)/2);
for g=1:levels
    baseX=sum(Xlist(1:(g-1)))+1;
    baseY=middleY-floor(Ylist(g)/2)+1;
    treeImg(baseX:(baseX-1+Xlist(g)),baseY:(baseY-1+Ylist(g)),:)= ...
        imgMarkup{g};
end
