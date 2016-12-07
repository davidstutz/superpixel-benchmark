function [imgGraph,points,edges,faces,voronoiStruct]= ...
    logz(radius,a,rings,rays)
%Function [imgGraph,points,edges,faces,voronoiStruct]= ...
%   logz(radius,a,rings,rays)
%generates an ImgGraph with a point set corresponding to the foveal 
%distribution w=log(z+a) and an edge set corresponding to the Delaunay 
%triangulation of those points.
%
%Inputs:    radius - The radius at which the points extend
%           a - The parameter 'a' in w=log(z+a).  'a' may be complex
%           rings/rays - The number of rings and rays (
%               i.e. sampling density) used for sampling
%
%Outputs:   imgGraph - The constructed ImgGraph
%           faces - The Nx3 matrix listing all the triangles which may 
%               be passed to showmesh()
%           voronoiStruct - The voronoi structure for visualization
%
%
%References:
%@Article{schwartz1977:spatial,
%  author =       {Schwartz, Eric L.},
%  title =        {Spatial mapping in the primate sensory projection:
%                  analytic structure and relevance to perception},
%  journal =      {Biological Cybernetics},
%  year =         1977,
%  volume =       25,
%  number =       4,
%  pages =        {181--194},
%  INSPEC =       1029313 }
%
%
%Note1: The medial line (x=0) will always be sampled with the 
%   corresponding number of rings.
%Note2: Blind spot not included.
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
% Date - $Id: logz.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Initialization
EPSILON=1e-6;
DITHER=.03;
rays=floor(rays/2);

%Generate retinal point list
xi=abs(exp(linspace(log(a+1),log(radius+a+1),rings)))-1;
eta=linspace(-pi/2+pi/(rays+1),pi/2-pi/(rays+1),rays);
[xi2,eta2]=meshgrid(xi,eta);
xDither=rand(rays,rings)-.5;
yDither=rand(rays,rings)-.5;

%Remove boundary dithering so that boundary looks smooth
xDither(:,(rings-1):rings)=zeros(rays,2); 
yDither(:,(rings-1):rings)=zeros(rays,2);

%Dither points
xPos=xi2.*cos(eta2)+xi2*DITHER.*xDither-real(a);  %Middle term to dither
yPos=xi2.*sin(eta2)+xi2*DITHER.*yDither-imag(a);  %Middle term to dither
dummyIndex=find(xPos>EPSILON & xPos<=(radius+abs(a)));
tempxPos=xPos(dummyIndex);
tempyPos=yPos(dummyIndex);
tempPoints=[tempxPos(:),tempyPos(:)];
    
%Assign total point values
N=size(tempPoints,1);
    
%Assign positive plane points
points=tempPoints;
    
%Assign negative plane points
tempPoints(:,1)=-tempPoints(:,1);
points=[points;tempPoints];
    
%Assign boundary (vertical meridian) points
index=0;
xiMeridian=abs(exp(linspace(log(a+1),log(a+radius+1),rings)))-1;
boundary=[];
for k=1:size(xiMeridian,2)
    if(xiMeridian(k)>(a+EPSILON))
        index=index+1;
        boundary=[boundary;[[0;0],[roots([1,2*imag(a),(imag(a)^2- ...
                            xiMeridian(k)^2+real(a)^2)])]]];
    end
end
index=index+1;
points=[[0,-imag(a)];boundary(2:length(boundary),:);points];

%Build filter information
[imgGraph,voronoiStruct]=findfilter(points,[2*radius 2*radius]);

%Generate edge set
[edges,faces]=triangulatepoints(points);
