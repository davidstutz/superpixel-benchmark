function pdfImg=contour2pdf(img)
%Function pdfImg=contour2pdf(img) inputs an RGB contour image with contours
%of different grayscale and (possibly) a red spot indicating a blind area.
%Function outputs an interpolated and normalized grayscale image with zeros
%in the "blind spot"
%
%Inputs:    img - The input RGB contour image
%
%Outputs:   pdfImg - Interpolated and normalized pdf
%
%
%5/21/03 - Leo Grady

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
% Date - $Id: contour2pdf.m,v 1.2 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Read image
[X Y Z]=size(img);

%Find red "blind spot"
blindSpot=find(img(:,:,1)>img(:,:,2));

%Find contour lines
redChannel=img(:,:,1);
contours=find(redChannel<1);

%Dirichlet problem interpolation
[points,edges]=lattice(X,Y);
L=laplacian(edges);
vals=dirichletboundary(L,contours,redChannel(contours));
pdfImg=reshape(vals,[X Y]);

%Apply blindspot
pdfImg(blindSpot)=0;

%Make region circular
radius=floor(min([X Y])/2);
circleX=radius*cos(linspace(0,2*pi,100));
circleY=radius*sin(linspace(0,2*pi,100));
[x,y]=meshgrid([1:Y]-Y/2-1, [1:X]-X/2-1);
incircle = inpolygon(x,y, circleX, circleY);
pdfImg(find(~incircle))=0;

%Normalize so that the sum of the probabilities is unity
pdfImg=pdfImg./sum(sum(pdfImg));
