function [imgMasks, imgMarkup, segOutline]= ...
  imgsegment(img,valScale,stop,algFlag,volFlag,connect,geomScale,diagFlag) 
%Function [imgMasks, imgMarkup, segOutline]= ...
% imgsegment(img,valScale,stop,algFlag,volFlag,connect,geomScale,diagFlag)
%recursively segments a standard (cartesian) image using a graph 
%partitioning algorithm
%
%Inputs:    img - The image to segmented 
%           valScale - The scale parameter applied of the algorithm 
%               applied to image values
%           stop - The stop parameter of the algorithm 
%           algFlag - Flag specifying the segmentation algorithm to use
%               0: Isoperimetric (Default)
%               1: Spectral
%           volFlag - Flag specifying which notion of volume to use
%               0: Degree  i.e., vol = sum(degree_of_neighbors) (Default)
%               1: Uniform  i.e., vol = 1
%           connect - Flag specifies topology of the lattice
%               0: 4-connect (Default)
%               1: 8-connect
%               K>1: radially connected with radius K
%               K<0: 4-connected with abs(K) random edges (small world)
%               Inf: Fully connected - Only attempt if image is tiny 
%                    (<< 100x100)
%           geomScale - Optional scale parameter applied of the algorithm 
%               applied to geometric values (Default: geomScale = 0)
%           diagFlag - An optional flag specifying diagnostic mode
%               0: Standard (Default)
%               1: Diagnostic mode  
%       
%
%Outputs:   imgMasks - An image where every pixel is assigned an integer 
%               such that pixels sharing numbers belong to the same segment
%           imgMarkup - The same image as the inputs with the red channel 
%               set to 1 along the borders of segments
%           segOutline - A white background with black lines indicating 
%               the segments borders
%
%
%6/23/03 - Leo Grady

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
% Date - $Id: imgsegment.m,v 1.5 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Set defaults
if nargin == 3
    [algFlag,volFlag,connect,geomScale,diagFlag]=deal(0);
elseif nargin == 4
    [volFlag,connect,geomScale,diagFlag]=deal(0);
elseif nargin == 5
    [connect,geomScale,diagFlag]=deal(0);
elseif nargin == 6
    [geomScale,diagFlag]=deal(0);
elseif nargin == 7
    diagFlag=0;
end 

%Normalize img2
[X,Y,Z]=size(img);
maxImg=max(max(max(img)));
if maxImg
    img=img./maxImg;
end
if  Z == 3
    vals=rgbimg2vals(img,1);
else
    vals=img(:);
end

%Generate graph
if connect <= 0
    [points edges]=lattice(X,Y);
    if connect < 0
        edges=addrandedges(-connect, edges);
    end
elseif connect == 1
    [points edges]=lattice(X,Y,1);
elseif connect==Inf
    disp 'Fully connected....Proceed at your own risk'
    [points edges]=lattice(X,Y);
    edges=adjtoedges(ones(X*Y,X*Y));
else
    [points edges]=lattice(X,Y,connect);    
end

%Generate weights
weights=makeweights(edges,vals,valScale,points,geomScale);

%Generate adjacency matrix
W=adjacency(edges,weights);

%Recursively partition
warning off
if diagFlag
    segAnswer=recursivepartition(W,stop,algFlag,volFlag,points);
else
    segAnswer=recursivepartition(W,stop,algFlag,volFlag);    
end
warning on

%Build outputs
[imgMasks,segOutline,imgMarkup]=segoutput(img,segAnswer);
