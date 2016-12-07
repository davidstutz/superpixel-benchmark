function segAnswer=recursivepartition(W,stop,algFlag,volFlag,points)
%Function segAnswer=recursivepartition(W,stop,algFlag,volFlag,points) 
%recursively calls partitiongraph.m until the stop criteria is statisfied.
%Function outputs a vector containing an integer label for every node 
%corresponding to the unsupervised partitions.
%
%Inputs:    W - Adjacency matrix (weighted) for a graph
%           stop - The recursion stop criterion
%           algFlag - Flag specifying the segmentation algorithm to use
%               0: Isoperimetric (Default)
%               1: Spectral
%           volFlag - Flag specifying which notion of volume to use 
%               0: Degree  i.e. vol = sum(degree_of_neighbors) (Default)
%               1: Uniform  i.e. vol = 1
%           points - Optional parameter giving the coordinates of the 
%              total point set (puts function into diagnostic mode)
%
%Outputs:   answer - A vector containing an integer label of every node 
%               indicating its group
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
% Date - $Id: recursivepartition.m,v 1.3 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Read inputs
if nargin < 3
    [algFlag,volFlag]=deal(0);
end
if nargin < 4
    volFlag=0;
end

%Initialization
N=length(W);

%Determine diagnostic mode and partition
if nargin == 5
    segAnswer=performrecursion(W,stop,algFlag,volFlag,zeros(N,1), ...
        [1:N]',1,points,1:N);
else
    segAnswer=performrecursion(W,stop,algFlag,volFlag,zeros(N,1), ...
        [1:N]',1);
end



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function answer=performrecursion(W,stop,algFlag,volFlag, ...
    answer,index,recursionDepth,points,currPoints)
%Function answer=performrecursion(W,stop,algFlag,volFlag, ...
%    answer,index,recursionDepth,points,currPoints) actually performs the
%    recursion.
%
%Inputs:    W - Adjacency matrix (weighted) for a graph
%           stop - The recursion stop criterion
%           algFlag - Flag specifying the segmentation algorithm to use
%               0: Isoperimetric (Default)
%               1: Spectral
%           volFlag - Flag specifying which notion of volume to use
%               0: Degree  i.e. vol = sum(degree_of_neighbors) (Default)
%               1: Uniform  i.e. vol = 1
%           answer - The current answer vector (start with all zeros)
%           index - Index of current point set in the answer vector
%           recursionDepth - Current recursion depth
%           points - Optional parameter giving the Nx2 coordinates of the 
%               total point set (puts function into diagnostic mode)
%           currPoints - Optional parameter (necessary if points are used) 
%               that gives a list of the current points under 
%               consideration
%
%Outputs:   answer - A vector containing an integer label of every node 
%               indicating its group
%
%5/23/03 - Leo Grady

%Initialization
N=length(W);
CUTOFF=5; %Number of nodes below which a new partition is not attempted
RECURSIONCAP=90;

%Partition current graph
if N > CUTOFF
    [part1,part2,constant,xFunction]=partitiongraph(W,algFlag,volFlag);
else
    constant=2;
end

%Check for diagnostic mode
if nargin > 7    
    %%Diagnostic mode
    %Output sizes of partition and their constant
    constant
    sizePart1=size(part1)
    sizePart2=size(part2)

    %Output partition to figure
    figure
    plot(points(:,1),points(:,2),'k.','MarkerSize',24);
    hold on
    plot(points(currPoints(part1),1),max(points(:,2))- ...
        points(currPoints(part1),2),'r.','MarkerSize',24);
    plot(points(currPoints(part2),1),max(points(:,2))- ...
        points(currPoints(part2),2),'b.','MarkerSize',24);
    title(sprintf('Constant: %d',full(constant)))
    axis equal
    axis tight
    axis off
    hold off
    tilefigs
    
    %If partition is of high enough quality, continue recursion
    if (constant < stop) & (recursionDepth < RECURSIONCAP)
        %Accept partition and update answer vector
        tmpInd=find(answer>answer(index(1)));
        answer(tmpInd)=answer(tmpInd)+1; %Make room for new class
        answer(index(part2))=answer(index(part2))+1; %Mark new class
    
        %Continue recursion on each partition
        if size(part1,1) > CUTOFF
            answer=performrecursion(W(part1,part1),stop,algFlag, ...
                volFlag,answer,index(part1),recursionDepth+1,points, ...
                currPoints(part1));
        end
    
        if size(part2,2) > CUTOFF
            answer=performrecursion(W(part2,part2),stop,algFlag, ...
                volFlag,answer,index(part2),recursionDepth+1,points, ...
                currPoints(part2));    
        end
    else
        answer=answer;
    end
else
    %%Standard (non-diagnostic) mode
    %If partition is of high enough quality, continue recursion
    if (constant < stop) & (recursionDepth < RECURSIONCAP)
        %Accept partition and update answer vector
        tmpInd=find(answer>answer(index(1)));
        answer(tmpInd)=answer(tmpInd)+1; %Make room for new class
        answer(index(part2))=answer(index(part2))+1; %Mark new class
    
        %Continue recursion on each partition
        if size(part1,1) > CUTOFF
            answer=performrecursion(W(part1,part1),stop,algFlag, ...
                volFlag,answer,index(part1),recursionDepth+1);
        end
    
        if size(part2,2) > CUTOFF
            answer=performrecursion(W(part2,part2),stop,algFlag, ...
                volFlag,answer,index(part2),recursionDepth+1);    
        end
    else
        answer=answer;
    end
end
