function [imgGraph,newPoints,newEdges]=findfilter(points,extent,F,edges)
%Function [imgGraph,newPoints,newEdges]=findfilter(points,extent,F,edges)
%Generates an ImgGraph with the appropriate filters for an input 2D point 
%set and Delaunay triangles for edges.
%
%Inputs:    points - A Nx2 point set intended to represent the sampling of
%                   visual space
%           extent - Optional vector [a b] specifying the extent of the 
%                   points (i.e. set a/b to input an AxB image). Default 
%                   is to leave points as input                   
%           F - Optional input specifying the scale of the ellipses
%                   relative to the Voronoi cells (Default: 1)
%           edges - Optional Mx2 edge set.  In input, the returned edge set
%                   will be a subset of the input edges with edges 
%                   connecting removing points removed.  If not input, 
%                   the returned edge set will correspond to a 
%                   retriangulation of the points
%
%Outputs:   imgGraph - The imgGraph with appropriate filters for the
%                   given point set
%           newPoints - Nx2 set of non-removed points
%           newEdges - Optional Mx2 set of non-removed edges
%
%References:
%@MastersThesis{heckbert1989:fundamentals,
%  author = 	 {Heckbert, Paul},
%  title = 	     {Fundamentals of Texture Mapping and Image Warping},
%  school = 	 {University of California at Berkeley},
%  year = 	     {1989} }
%
%
%Note: Assumes that the points are scattered about the origin
%Note2: Based on script precursorFindFilt.m
%
%
%5/20/02 - Leo Grady

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
% Date - $Id: findfilter.m,v 1.3 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Check inputs
if nargin < 4
    F=1;
end
if nargin < 5
    dispFlag=0;
end    

%Initialization
EPSILON=1e-6;
OFFSET=1.5;
N=length(points);
[pntMap,filtWeights,removalIndex,singleBoundPointIndex,vx,vy]=deal([]);
[breakpoints,weightInd]=deal(1);

%Find extremal nodes
minX=min(points(1,:));
maxX=max(points(1,:));
minY=min(points(2,:));
maxY=max(points(2,:));

%Adjust extent, if requested
if nargin > 1
    minX=-extent(1)/2;
    maxX=extent(1)/2;
    minY=-extent(2)/2;
    maxY=extent(2)/2;        
    points=normalize(points,[minX,minY;maxX,maxY]);
end

%Add phantom border points for Voronoi
halfN=ceil(N/2);
widthBorder=linspace(minX-OFFSET,maxX+OFFSET,halfN)';
widthBorder=widthBorder(2:(end-1)); %Don't duplicate corners
heightBorder=linspace(minY-OFFSET,maxY+OFFSET,halfN)';
points=[points;[minX*ones(halfN,1)-OFFSET;maxX*ones(halfN,1)+OFFSET; ...
            widthBorder;widthBorder],[heightBorder;heightBorder; ...
            minY*ones(halfN-2,1)-OFFSET;maxY*ones(halfN-2,1)+OFFSET]]; 
            %OFFSET used to give a small buffer around node cluster

%Find voronoi cells
[vpoints, vcells] = voronoin(points);

%Find infinity point
infPoints=find(vpoints(:,1)==Inf);

%Find filters
for k=1:N
    if ~(sum(vcells{k}==infPoints))
        %Fit ellipse to Voronoi cells with LSE
        [coeffs, centroid]=ellipsefit(vpoints(vcells{k},:),F);
        offset=abs(round(centroid)-centroid);
        
        %Find bounding box
        xBound=2*sqrt(-coeffs(3)*F/(coeffs(2)^2-4*coeffs(1)*coeffs(3)));
        yBound=2*sqrt(-coeffs(1)*F/(coeffs(2)^2-4*coeffs(1)*coeffs(3)));
        [xgrid,ygrid]=meshgrid(-ceil(xBound):ceil(xBound), ...
            -ceil(yBound):ceil(yBound));
        xgrid=xgrid+offset(1);
        ygrid=ygrid+offset(2);
        
        %Using Heckbert's notation, find bounded points
        domainEllipse=coeffs(1)*(xgrid).^2 + ...
            coeffs(2)*(xgrid).*(ygrid) ...
            + coeffs(3)*(ygrid).^2;
        boundedPts=find(domainEllipse<=F);
        numbBoundPts=length(boundedPts);
        
        %Add new points
        if(isempty(boundedPts)) %Remove any nodes which bound zero pixels
            removalIndex=[removalIndex,k];
        else
            %Assign pixels to array
            newWeight=exp(-.5*(xgrid(boundedPts).^2 ...
                + ygrid(boundedPts).^2));
            realWeight=find(newWeight>EPSILON); 
                %Prevent storage of values that have no effect
            newWeight=newWeight(realWeight);
            boundedPts=boundedPts(realWeight);
            if isempty(newWeight) %If the filtWeights are all subthreshold
               removalIndex=[removalIndex,k];
            else
                pntMap=[pntMap;[xgrid(boundedPts)+points(k,1),...
                    ygrid(boundedPts)+points(k,2)]];
                newWeight=newWeight./sum(newWeight);  %Sum to unity
                filtWeights=[filtWeights;newWeight];
                breakpoints(weightInd+1,:)=breakpoints(weightInd)+ ...
                    length(realWeight);
                weightInd=weightInd+1;
            end
        end
    end
end

%Postprocess
pntMap=floor(abs(pntMap)).*sign(pntMap);
breakpoints=breakpoints(2:end); %Remove leading '1' value

%Remove nodes that bound zero pixels
if(~isempty(removalIndex))
    %Remove points from point list
    keepPts=1:N;
    keepPts(removalIndex)=[];
    N2=N-length(removalIndex);
else
    N2=N;
    keepPts=1:N;
end
newPoints=points(keepPts,:);

%Remove edges to removed nodes
if (nargout > 2) & (nargin < 4)
    [newEdges,faces]=triangulatepoints(newPoints);
elseif (nargout > 2)
    W=adjacency(edges);
    newEdges=adjtoedges(W(keepPts,keepPts));
end

%Generate ImgGraph
imgGraph.pntMap=pntMap;
imgGraph.breakpoints=breakpoints;
imgGraph.filtWeights=filtWeights;
