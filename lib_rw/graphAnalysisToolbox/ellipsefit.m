function [coeffs,centroid]=ellipsefit(points,F)
%Function [coeffs,centroid]=ellipsefit(points,F) finds coefficients to 
%an ellipse that fits the polygon described by the points
%
%The ellipse is parameterized by:
%c_1(x - x_0)^2 + c_2(x - x_0)(y - y_0) + c_3(y - y_0)^2 = F
%
%Inputs:    points - An Nx2 set of N points in the plane
%           F - Optional parameter 'F' in the above equation (Default: 
%               F = 1)
%
%Outputs:   coeffs - A 3x1 vector consisting of coeffs = [c_1 ; c_2 ; c_3]
%                    from the above equation
%           centroid - A 1x2 vector consisting of centroid = [x_0 , y_0]
%                    from the above equation
%
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
% Date - $Id: ellipsefit.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Check if F is specified
if nargin < 2    
    F=1;
end

%Determine number of points
N=size(points,1);

%Find centroid
x=points(:,1);
y=points(:,2);
shiftedy=y([2:N,1]);
shiftedx=x([2:N,1]);
tmpVec=(x.*shiftedy-shiftedx.*y);
polyArea=.5*ones(1,N)*tmpVec;
centroid(1)=(x+shiftedx)'*tmpVec/polyArea/6;
centroid(2)=(y+shiftedy)'*tmpVec/polyArea/6;           

%Cluster points around origin
points=points-ones(N,1)*centroid;

%Find polynomial coefficients
tmpMat=[points(:,1).^2,(points(:,1).*points(:,2)),(points(:,2).^2)];
coeffs=tmpMat\ones(N,1);
