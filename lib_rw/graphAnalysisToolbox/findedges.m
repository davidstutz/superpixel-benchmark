function edgeNodes=findedges(edges,vals,type,weights,thresh)
%Function edgeNodes=findedges(edges,vals,type,weights,thresh) 
%performs edge detection on the nodal field given by vals
%
%Inputs:    edges - The Mx2 edge list
%           vals - The nodal field to perform edge detection on
%           type - Optional flag indicating the type of edge detection
%                  0 - Gradient threshholding (Default)
%                  1 - Laplacian threshholding
%           weights - Optional Mx1 edge weight list
%           thresh - Optional threshhold to apply for gradient or 
%                  Laplacian threshholding (has no effect on Laplacian 
%                  zero-crossing detection)
%
%Outputs:   edgeNodes - A list of nodes determined to be on the border
%
%
%Note1: Traditional machine vision finds edges on the nodes from both
%gradient and Laplacian operators, despite the fact that the gradient
%operator inputs points and outputs vectors (in our case, edges).  We have
%followed convention here by returning the outputs of the gradient to 
%nodal values.
%
%Note2: If vals has more than one dimension, a Euclidean metric is used.
%
%Note3: Function assumes a connected graph
%
%5/25/03 - Leo Grady

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
% Date - $Id: findedges.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Initialization
N=max(max(edges));

%Read inputs
if (nargin < 3) | (nargin > 5)
    disp('Invalid number of arguments.  Please see type "help findedges"')
    edgeNodes=[];
    return
end
if nargin < 3
    type=0;
end
if nargin < 4
    weights=ones(length(edges),1);
end
if nargin < 5
    if type == 0
        thresh=.2;
    else
        thresh=.2;
    end
end

%Perform edge detection
if type == 0  %Gradient threshholding
    A=incidence(edges,weights);
    grads=sqrt(sum((A*vals).^2,2));
    boundaryEdges=find(grads>thresh);
    dummy=zeros(1,N); %Used to avoid duplicate node reporting
    dummy([edges(boundaryEdges,1),edges(boundaryEdges,2)])=1;
    edgeNodes=find(dummy);
else %Laplacian 
    vals=sqrt(sum(vals.^2,2));
    L=laplacian(edges,weights);
    lapVals=L*vals;
    edgeNodes=find(lapVals>thresh);
end        
