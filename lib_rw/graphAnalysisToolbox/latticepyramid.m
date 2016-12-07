function [levelChildren,levelIndex,points,edges,edgeBias,levels]= ...
    latticepyramid(X,Y,levels,connect)
%Function [levelChildren,levelIndex,points,edges,edgeBias,levels]= ...
%   latticepyramid(X,Y,levels,connect)
%computes a quadtree image pyramid with "levels" number of levels and returns
%information for transmitting image information between levels as well as
%the graph
%
%Inputs:    X/Y - The X/Y dimensions of the lattice
%           levels - The number of levels of the pyramid. 
%               Note: Levels should include the bottom level 
%               (i.e., the total number)
%           connect - Optional flag indicating 4- or 8-connectedness for
%                each level
%                0 - 4-connected (Default)
%                1 - 8-connected
%                K>1 - Radial connection with radius K
%
%Outputs:   levelChildren - A 4xP matrix indicating the children of all
%                nodes in higher levels
%           levelIndex - A vector indicating the breakpoints in
%                points/levelChildren for each level
%           points - A Nx3 set of the complete points in the pyramid
%           edges - A Mx2 set of the complete edges in the pyramid
%           edgeBias - A 1xM set of integers indicating with value '1' if
%                the edge is in the bottom level or between levels and value 'k'
%                if the edge is internal to level k
%           levels - The actual number of levels used...if the requested
%               number was too high
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
% Date - $Id: latticepyramid.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Inputs
if nargin == 2
    [levels,connect]=deal(0);
elseif nargin == 3
    connect=0;
end

%Error checking on levels
maxLevels=floor(log(X*Y)/log(4))+1;  %To get log_4
if levels > maxLevels
    levels=maxLevels;
end

%Initialization
setIndex=0;
levelChildren=[];
oldX=X; oldY=Y;
origN=oldX*oldY;

%Build lowest level
[points edges]=lattice(oldX,oldY,connect);
oldN=oldX*oldY;
N=oldN;
points=[points,zeros(oldN,1)]; %Add z-coordinate        

%Build additional levels and connect
levelIndex=0;
count=1;
edgeBias=ones(1,length(edges));
for k=1:(levels-1)
    %Find indices for new level children
    index=[];
    dummy=zeros(oldX,oldY);
    tmp=repmat([1 0;0 0],floor(oldX/2),floor(oldY/2));
    dummy(1:size(tmp,1),1:size(tmp,2))=tmp;
    index=[index;find(dummy)'];
    dummy=zeros(oldX,oldY);    
    tmp=repmat([0 1;0 0],floor(oldX/2),floor(oldY/2));
    dummy(1:size(tmp,1),1:size(tmp,2))=tmp;
    index=[index;find(dummy)'];
    dummy=zeros(oldX,oldY);
    tmp=repmat([0 0;1 0],floor(oldX/2),floor(oldY/2));
    dummy(1:size(tmp,1),1:size(tmp,2))=tmp;
    index=[index;find(dummy)'];
    dummy=zeros(oldX,oldY);
    tmp=repmat([0 0;0 1],floor(oldX/2),floor(oldY/2));
    dummy(1:size(tmp,1),1:size(tmp,2))=tmp;
    index=[index;find(dummy)'];
    index=index+setIndex;

    %Build new level
    if ~isempty(index)
        count=count+1;
        
        %Create new level
        newX=floor(oldX/2);
        newY=floor(oldY/2);
        newN=newX*newY;
	[newPoints newEdges]=lattice(newX,newY,connect);
        tmpX=points(:,1);
        tmpY=points(:,2);
        
        %Center node above its "children"
        newPoints=[sum(tmpX(index))./4;sum(tmpY(index))./4]; 
        edgeBias=[edgeBias,count*ones(1,length(newEdges)), ...
                count*ones(1,4*length(index))];

        %Connect new level to old level
        newEdges=newEdges+N;
        newIndex=(N+1):(newN+N);
        newEdges=[newEdges;[newIndex,newIndex,newIndex, ...
                    newIndex;index(1,:),index(2,:),index(3,:), ...
                    index(4,:)]'];

        %Combine with old information
        points=[points;[newPoints;k*ones(1,newN)]'];
        edges=[edges;newEdges];
        
        %Build outputs
        levelIndex(k)=N;
        levelChildren=[levelChildren,index];
    
        %Increment loop variables
        setIndex=setIndex+oldN;
        N=N+newN;
        oldN=newN;
        oldX=newX;
        oldY=newY;    
    end
end

%Fix level index
levelIndex=levelIndex-min(levelIndex)+1;
levelIndex=[levelIndex,length(points)-origN+1];

%Output actual levels used
levels=count;
