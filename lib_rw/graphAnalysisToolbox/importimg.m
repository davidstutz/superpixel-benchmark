function vals=importimg(imgGraph,img,fov,padColor,normFlag)
%Function vals=importimg(imgGraph,img,fov,padColor,normFlag) 
%imports an image onto the graph specified by imgGraph
%
%Inputs:    imgGraph - The imgGraph in question
%           img - The image to be imported
%           fov - Optional foveal location (origin).  Defaults to center
%               of the image.
%           padColor - Optional parameter defining what color out of 
%               range nodes are assigned to.  Defaults to black ([0 0 0])
%           normFlag - Optional parameter allowing user to normalize 
%               image from 0 to 1.  Default is no normalization.
%
%Outputs:   vals - The NxP list of image values
%
%
%
%05/16/03 - Leo Grady

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
% Date - $Id: importimg.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Find dimensions of the image
[X Y Z]=size(img);

%Set default fov location
if nargin < 3
    fov=[ceil(Y/2) ceil(X/2)+1];
end

%Set default pad color
if nargin < 4
    padColor=[0,0,0];
else
    padColor=padColor(:)'; %Error check for vector orientation
end

%Preprocess
Npixels=X*Y;
newPoints(:,1)=fov(1)+imgGraph.pntMap(:,1);
newPoints(:,2)=fov(2)-imgGraph.pntMap(:,2);
outRange=find((newPoints(:,1)<1)|(newPoints(:,1)>Y)| ...
    (newPoints(:,2)<1)|(newPoints(:,2)>X));
currPnts=fov(2)-imgGraph.pntMap(:,2)+ ...
    X*(fov(1)+imgGraph.pntMap(:,1)-1);
currPnts(outRange)=Npixels+1;

%Perform importation
if(Z==3)
    %Set up RGB vector
    redImg=img(:,:,1);  
    redImg=redImg(:);
    redImg(Npixels+1)=padColor(1);
    greenImg=img(:,:,2);  
    greenImg=greenImg(:);
    greenImg(Npixels+1)=padColor(2);
    blueImg=img(:,:,3);  
    blueImg=blueImg(:);
    blueImg(Npixels+1)=padColor(3);    
    RGBImg=[redImg,greenImg,blueImg];
    clear img redImg greenImg blueImg 
    
    %Process
    RGBweights=imgGraph.filtWeights*ones(1,3);
    currPnts=cumsum(RGBImg(currPnts,:).*RGBweights);
    tmpVals=currPnts(imgGraph.breakpoints-1,:);   
    vals=[tmpVals(1,:);diff(tmpVals)];
else      
    img=img(:);
    img(Npixels+1)=mean(padColor);
    currPnts2=cumsum(img(currPnts).*imgGraph.filtWeights);
    tmpVals=currPnts2(imgGraph.breakpoints-1);  
    vals=[tmpVals(1);diff(tmpVals)];
end

%Normalize to 0-1 range
if (nargin==4) & (normFlag)
    vals=normalize(vals);
end
