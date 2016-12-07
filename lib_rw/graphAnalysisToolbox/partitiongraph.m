function [part1,part2,constant,xFunction]= ...
    partitiongraph(W,algFlag,volFlag,cutMethod,ground,normFlag) 
%Function [part1,part2,constant,xFunction]= ...
%partitiongraph(W,algFlag,volFlag,cutMethod,ground,normFlag) 
%computes the partition of a graph based on the isoperimetric or spectral
%algorithm.
%
%Inputs:    W - Adjacency matrix (weighted) for a graph
%           algFlag - Optional flag specifying the segmentation algorithm 
%               to use. 
%               0: Isoperimetric (Default)
%               1: Spectral
%           volFlag - Flag specifying which notion of volume to use. 
%               0: Degree  i.e. vol = sum(degree_of_neighbors) (Default)
%               1: Uniform  i.e. vol = 1
%           cutMethod - Optional parameter specifying the method of cut. 
%               0: Chooses the cut giving the smallest ratio of the
%                  appropriate criterion. i.e., the ratio cut (Default)
%               1: Weighted jump cut
%               2: Unweighted jump cut
%               3: Median cut (guarantees equal sized partitions)
%           ground - Optional parameter specifying the ground (using the 
%               isoperimetric algorithm - has no effect for spectral
%               methods).  Default is node with maximum weighted degree.
%           normFlag - Optional flag determining whether or not the
%               xFunction is normalized. Default = 0 (unnormalized)
%
%Outputs:   part1 - A list indexing into points indicating those points 
%                   in the first partition
%           part2 - A list indexing into points indicating those points 
%                   in the second partition
%           constant - The relevant constant indicating the "goodness" of 
%                   the partition (lower the better).
%           xFunction - Returns the Nx1 potential function for each node
%
%
%Note that the combination of algFlag=1 and volFlag=1 computes the
%Normalized Cuts algorithm and returns constant, ratio cut, etc. with
%respect to the Normalized Cut criterion.
%
%
%5/22/03 - Leo Grady
%For references, see file.

%References:
%Spectral partitioning
%@Article{pothen1990:partitioning,
%  author =	 {Pothen, Alex and Simon, Horst and Liou, Kang-Pu},
%  title =	 {Partitioning Sparse Matrices with Eigenvectors of
%             Graphs},
%  journal = {SIAM Journal of Matrix Analysis Applications},
%  year =	 1990,
%  volume =	 11,
%  number =	 3,
%  pages =	 {430--452} }
%
%
%Normalized cuts:
%@Article{shi2000:normalized,
%  author =  {Shi, Jianbo and Malik, Jitendra},
%  title = 	 {Normalized Cuts and Image Segmentation},
%  journal = {IEEE Transactions on Pattern Analysis and Machine 
%             Intelligence},
%  year = 	 {2000},
%  volume =  {22},
%  number =  {8},
%  pages = 	 {888--905},
%  month = 	 {August} }

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
% Date - $Id: partitiongraph.m,v 1.3 2003/08/21 17:29:29 lgrady Exp $
%========================================================================%

%Build Lapalcian matrix
d=sum(W,2);
L=diag(d)-W;
N=length(d);

%If algorithm not specified, assign Isoperimetric
if nargin < 2
    [algFlag,volFlag,cutMethod]=deal(0);    
    [dummy,ground]=max(d);
    ground=full(ground);
end

%Assign ground
if (~algFlag) & ((nargin < 5) | (ground < 1) | (ground > N))
    [dummy,ground]=max(d);
    ground=full(ground);
end

%If cutMethod not specified, assign zero value
if nargin < 4
    cutMethod=0;
end

%If volume not specified, assign default
if nargin < 3
    [volFlag,cutMethod]=deal(0);
end

%If normalization not specified, assign default
if nargin < 6
    normFlag=0;
end

%Find nodal function
if algFlag
    %Spectral
    opts.issym=1;
    opts.disp=0;
    warning off MATLAB:nearlySingularMatrix
    if ~volFlag %Use normalized Laplacian matrix
        D2=diag(d.^-.5);
        L2=D2*L*D2;
    	[V,F,convFlag]=eigs(L2,2,1e-7,opts); %1e-7 used for stability
    else
        [V,F,convFlag]=eigs(L,2,1e-7,opts); %1e-7 used for stability
    end
    warning on MATLAB:nearlySingularMatrix    
    if ~convFlag
        [a b]=max(max(F));    
        xFunction=V(:,b);
    else
        xFunction=ones(N,1);
        constant=2;
        part1=1:floor(N/2);
        part2=(floor(N/2)+1):N;
        return
    end
else
    %Isoperimetric
    if volFlag
        xFunction=isosolve(L,ones(N,1),ground);
    else
        xFunction=isosolve(L,d,ground);
    end
end

%Determine cut point
if cutMethod == 0 %Ratio cut method    
    CUTOFF=5; %Defines the smallest cut allowable
    indicator=sparse(N,1);
    
    %Sort values
    sortX=sortrows([xFunction,[1:N]'],1)';
    
    %Find total volume
    if volFlag
        totalVolume=N;
    else
        totalVolume=sum(d);
    end
    halfTotalVolume=totalVolume/2;
    
    %Calculate denominators
    sortedDegree=d(sortX(2,:))';
    if volFlag
        denominators=1:N;
    else
        denominators=cumsum(sortedDegree);
    end
    tmpIndex=find(denominators>halfTotalVolume);
    denominators(tmpIndex)=totalVolume-denominators(tmpIndex);
    
    %Calculate numerators
    L=L(sortX(2,:),sortX(2,:))-diag(sortedDegree);
    numerators=cumsum(sum((L-2*triu(L)),2))';
    if min(numerators) < 0
        %Line used to avoid negative values due to precision issues
        numerators=numerators-min(numerators)+eps;         
    end
    
    %Calculate relevant ratios
    warning off %Avoids divide by zero warnings
    if algFlag & ~volFlag
        %Ncuts criteria
        [constant,minCut]=min(numerators(CUTOFF:(N-CUTOFF)).* ...
            (1./denominators(CUTOFF:(N-CUTOFF)) + ...
            1./(totalVolume-denominators(CUTOFF:(N-CUTOFF)))));
    else
        %Isoperimetric criteria
        [constant,minCut]=min(numerators(CUTOFF:(N-CUTOFF))./ ...
            denominators(CUTOFF:(N-CUTOFF)));
    end         
    minCut=minCut+CUTOFF-1;
    warning on
    
    %Error check
    if N < 2*CUTOFF+1
        constant=Inf;
        minCut=1;
    end
    
    %Output partitions
    part1=sortX(2,1:(minCut))';
    part2=sortX(2,(minCut+1):N);
else
    if cutMethod == 1 %Weighted jump cut method
        BIAS=10; %Empircally determined

        %Sort output
        sortX=sort(xFunction);
        diffX=diff(sortX)';
    
        %Generate squashing function
        sigma=-4*log(BIAS)/(N^2);
        range=linspace(0,1,(N-1));

        %Find optimal weighted jump
        [dummy,cutVal]=max(diffX.*(1+BIAS*(range-1)./ ...
            (range+BIAS)-BIAS*range./(BIAS+1-range)));

        %Error catch if best "cut" is only the first node
        if (cutVal==1)
            constant=Inf;
            part1=[];
            part2=[];
            return
        end

        %Assign partitions
        part1=find(xFunction<sortX(cutVal+1)); 
    elseif cutMethod == 2 %Unweighted jump cut method        
        %Sort output
        sortX=sort(xFunction);
        diffX=diff(sortX)';
        
        %Find optimal weighted jump
        [dummy,cutVal]=max(diffX)

        %Error catch if best "cut" is only the first node
        if cutVal == 1
            constant=Inf;
            part1=[];
            part2=[];
            return
        end

        %Assign partitions
        part1=find(xFunction<sortX(cutVal+1)); 
    elseif cutMethod == 3 %Median cut method
        %Error catch
        xFunction(find(isnan(xFunction)))=max(xFunction);
        
        %Median cut method
        med=median(xFunction);
    
        %Perform cut
        part1=find(xFunction<med);
        cusp=find(xFunction==med);
        part1=[part1;cusp(1:floor(length(cusp)/2))];
    end 
    %Calculate numerator for the selected cut
    part2=1:N;
    part2(part1)=[];
    indicator=sparse(N,1);
    indicator(part1)=1; 
    
    %Calculate denominator
    if volFlag
        denom1=length(part1);
        denom2=length(part2);        
    else
        denom1=sum(d(part1));
        denom2=sum(d(part2));
    end
    
    %Calculate ratio
    if algFlag & ~volFlag %Use Normalized Cuts criterion
        constant=full(indicator'*L*indicator*(1/denom1 + 1/denom2));
    else %Use isoperimetric ratio criterion
        constant=full(indicator'*L*indicator/min(denom1,denom2));
    end 
end

%Perform normalization
if normFlag
    xFunction=normalize(xFunction);
end
