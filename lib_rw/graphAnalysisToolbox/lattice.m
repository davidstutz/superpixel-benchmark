function [points,edges] = lattice(X,Y,connect)
%Function [points,edges]=lattice(X,Y,connect) generates a 2D, 
%"4-connected" Cartesian lattice with dimensions X and Y. 
%
%Inputs:    X/Y - The dimensions of the lattice (i.e., the number of 
%               nodes will equal X*Y)
%           connect - Optional iInteger indicating the connectivity of 
%               the lattice
%               0 - 4-connected (Default)
%               1 - 8-connected
%               K>1 - Radially connected with radius = K
%
%Outputs:   points - Nx2 lattice nodes
%           edges - Mx2 lattice edges
%
%
%6/5/03 - Leo Grady

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
% Date - $Id: lattice.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Check for single point
if X*Y==1
    points=[1;1];
    edges=[];
    return
end

%Read connect argument
if nargin < 3
    connect=0;
end

if connect < 2
    %Generate points
    rangex=0:(X-1);
    rangey=0:(Y-1);
    [x y]=meshgrid(rangey,rangex);
    points=[x(:),y(:)];
    N=X*Y;

    %Connect points
    edges=[[1:N]',[(1:N)+1]'];
    edges=[[edges(:,1);[1:N]'],[edges(:,2);[1:N]'+X]];
    if connect == 1 %8-connected
        border=1:N;
        border1=find(mod(border,X)-1)';
        border2=find(mod(border,X))';
        edges=[edges;[border1,border1+X-1;border2,border2+X+1]];
    end    
    excluded=find((edges(:,1)>N)|(edges(:,1)<1)|(edges(:,2)>N)| ...
        (edges(:,2)<1));
    edges([excluded;[X:X:((Y-1)*X)]'],:)=[]; 
else %Radial connectivity
    %Generate points
    X=X+2*connect;
    Y=Y+2*connect;
    rangex=0:(X-1);
    rangey=0:(Y-1);
    [x y]=meshgrid(rangey,rangex);
    points=[x(:),y(:)];
    N=X*Y;

    %Generate adjacency matrix
    distances=(points(:,1)-floor(Y/2)).^2 + (points(:,2)-floor(X/2)).^2;
    inside=find(distances<=(connect^2));
    adjVec=sparse(1,inside,1,1,N);
    midPoint=find(points(:,1)==floor(Y/2) & points(:,2)==floor(X/2));
    %Adjust vector to be centered around diagonal
    adjVec=adjVec([midPoint:N,1:(midPoint-1)]);
    adjVec(1)=0; %Remove self-connection
    W=circulant(adjVec);

    %Remove phantom points
    index=1:N;
    tmp=1:X:N;
    tmp=tmp'*ones(1,connect)+ones(length(tmp),1)*(0:(connect-1));
    tmp2=tmp+X-connect;
    index([tmp(:)',tmp2(:)',1:(X*connect),N+[-(X*connect-1):0]])=[];
    edges=adjtoedges(W(index,index));
    points=points(index,:);
end
