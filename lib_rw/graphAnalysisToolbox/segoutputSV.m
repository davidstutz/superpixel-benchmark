function [valsOutline,valsMarkup]=segoutputSV(edges,vals,solution)
%Function [valsOutline,valsMarkup]=segoutputSV(edges,vals,solution) 
%generates output for a graph-based segmentation
%
%Inputs: edges - Mx2 edge list for the graph
%        vals - Original vals for the graph
%        solution - A 1xN vector assigning an integer to each node
%           indicating its membership in a segment
%
%Outputs: valsMarkup - The same vals as the inputs with the red channel 
%           (first column) set to 1 along the borders of segments
%         valsOutline - A white background with black lines indicating the 
%           segments borders
%
%
%5/23/03 - Leo Grady

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
% Date - $Id: segoutputSV.m,v 1.3 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Initialization
N=max(max(edges));

%Build incidence matrix
A=incidence(edges);

%Find segment borders
segBorders=find(A*solution);
segBorders=[edges(segBorders,1);edges(segBorders,2)];

%Build valsOutline
valsOutline=ones(N,1);
valsOutline(segBorders)=0;

%Build valsMarkup
valsMarkup=vals;
if size(vals,2) == 1
    valsMarkup(:,2:3)=[vals,vals];
end
valsMarkup(segBorders,:)=0;
valsMarkup(segBorders,1)=1;
