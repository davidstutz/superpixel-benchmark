function voronoiStruct = voronoicells(points,boundFlag)
%Function voronoiStruct = voronoicells(points,boundFlag) 
%Construct voronoi cell information for visualization from an input point 
%   set
%
%Inputs:    points - An Nx2 point set
%           boundFlag - Optional flag indicating the type of bounding
%               function for voronoi points 
%               0 - Box (Default)
%               1 - Circle
%
%Outputs:   voronoiStruct, consisting of three fields:
%               pts - An Kx2 point list of the voronoi cell 
%                   vertices where K > N (i.e., more points are required to 
%		    specify the Voronoi cells than the nodes themselves)
%               faces - A matrix indexed by row of all the faces 
%                   of the voronoi diagram to be used by patch.m
%               index - A vector indicating which point in the 
%                   original set corresponds to which face in voronoiFaces
%
%
%5/20/03 - Leo Grady

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
% Date - $Id: voronoicells.m,v 1.3 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Initialization
OFFSET=1;
N=size(points,1);
halfN=ceil(N/2);

%Check inputs for optional argument
if nargin < 2
    boundFlag=0;
end

if boundFlag
    %Add circular bounding points
    radPoints=points(:,1).^2 + points(:,2).^2;
    maxR=sqrt(max(radPoints))+sqrt(2)*OFFSET;
    theta=linspace(0,2*pi-2*pi/halfN,halfN)';
    pointsPad=[points;[maxR*cos(theta),maxR*sin(theta)]];
else
    %Add rectangular bounding points
    maxX=max(points(:,1));
    minX=min(points(:,1));
    maxY=max(points(:,2));
    minY=min(points(:,2));
    widthBorder=linspace(minX-OFFSET,maxX+OFFSET,halfN)';
    widthBorder=widthBorder(2:(end-1)); %Don't duplicate corners
    heightBorder=linspace(minY-OFFSET,maxY+OFFSET,halfN)';
    pointsPad=[points;[minX*ones(halfN,1)-OFFSET;maxX*ones(halfN,1)+ ...
        OFFSET; widthBorder;widthBorder],[heightBorder;heightBorder; ...
        minY*ones(halfN-2,1)-OFFSET;maxY*ones(halfN-2,1)+OFFSET]]; 
        %OFFSET used to give a small buffer around node cluster 
end
    
%Get voronoi information
[voronoiPts, voronoiCells] = voronoin(pointsPad);

%Remove outlier points
if boundFlag
    %Circular bounding
    radVoronoi=voronoiPts(:,1).^2 + voronoiPts(:,2).^2;
    infIndex=find(radVoronoi>max(radPoints));
else
    %Rectangular bounding
    infIndex=find(voronoiPts(:,1)>(maxX+OFFSET));
    infIndex=[infIndex;find(voronoiPts(:,1)<(minX-OFFSET))];
    infIndex=[infIndex;find(voronoiPts(:,2)>(maxY+OFFSET))];
    infIndex=[infIndex;find(voronoiPts(:,2)<(minY-OFFSET))];
    %Remove duplicate indices
    dummy=zeros(1,max(infIndex));
    dummy(infIndex)=1;
    infIndex=find(dummy);
end

%Remove infinity voronoi points 
tmp=voronoiPts(:,1);
tmp2=voronoiPts(:,2);
tmp(infIndex)=[];
tmp2(infIndex)=[];
voronoiPts=[tmp,tmp2];

%Build voronoi faces to be sent to patch in 
%   showvoronoi (i.e., remove from cells and pad with NaNs)
infCell=[];
index=1; 
for k=1:N
    if(isempty(intersect(infIndex,voronoiCells{k})))
        cellSize=length(voronoiCells{k});
        voronoiFaces(index,1:cellSize)=voronoiCells{k};        
        index=index+1;
    else
        infCell=[infCell,k];
    end    
end

%Compensate for indexing
sortedInfIndex=sort(infIndex);
endIndex=length(infIndex)+1;
infLen=length(infIndex);
for s=1:length(infIndex)
    dummy=find(voronoiFaces>sortedInfIndex(endIndex-s));
    voronoiFaces(dummy)=voronoiFaces(dummy)-1;
end

%Pad trailing zeros with last vertex (to workaround openGL issue)
for r=1:size(voronoiFaces,1)
    trailingPoints=find(voronoiFaces(r,:)==0);
    if (~isempty(trailingPoints))
        voronoiFaces(r,trailingPoints)= ...
            voronoiFaces(r,(trailingPoints(1)-1));
    end
end

%Build voronoiIndex by excluding cells in infCell
voronoiIndex=1:N;
voronoiIndex(infCell)=[];

%Build output struct
voronoiStruct.index=voronoiIndex;
voronoiStruct.faces=voronoiFaces;
voronoiStruct.pts=voronoiPts;
