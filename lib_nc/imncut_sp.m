% function par = imncut_sp; or
%          [Sp,Seg,V,S,W] = imncut_sp(I,par)
% 
% Compute "superpixels."
% An intial segmentation into K1=par.nv segments is performed using cncut.
% These initial segments are then subdivided into K2=par.sp "superpixels"
% by running kmeans on the cncut eigenvector coordinates, followed by cleanup.
%
% Input:
%   I = image
%   par = parameters
%       par.nb_r = neighbour hood radius
%       par.sample_rate = sampling rate
%       par.mask = mask for bias, label matrix of image size
%       par.sig_ic = sigma for computing affinity from intervening contour
%       par.rep = offset for repulsion
%       par.reg = offset for regularization
%       par.nv = number of eigenvectors
% Output:
%   Sp = K2-way partition matrix, same size as I, after forcing K2=par.sp segments
%   Seg = initial K1-way partition matrix, same size as I
%   [V,S] = eigenvector and eigenvalues from constrained ncuts
%   W = affinity matrix
% Reference:
%   www.cs.berkeley.edu/~stellayu/thesis.html
% Examples:
%   par = imncut_sp; % return the default parameters
%   imncut_sp(I); % ncuts using the default parameters

% Stella X. Yu, July 13 2003.
% Modified by Greg Mori, April 2005.

function [Sp,Seg,V,S,W] = imncut_sp(I,par,varargin)

if nargin==0,
    par.nb_r = 8;
    par.nb_r = 24;
    par.sample_rate = 0.2;
%    par.nb_r = 30;
%    par.sample_rate = 0.2;
    par.mask = [];
    par.sig_ic = 0.4;
    par.sig_pb_ic = 0.2;
    par.sig_int = 0.15;
    par.sig_p = 10;
    par.rep = 0;
    par.reg = 0;
    par.nv = 3;
    par.hwx = 1;
    par.hwy = 1;
    par.pb_ic = 1;
    par.ic = 0;
    par.int = 0;
    par.prox = 0;
    par.count = 0;
    par.verbose = 0;
    par.visualize = 0;
    par.pb_timing = 1;
    par.sp = 3;
    Sp = par;
    return;
end

if nargin<2,
    par = ncutim;
end
if nargin<3
    visualize=1;
end
mask = par.mask;
nv = par.nv;
VERBOSE = par.verbose;
visualize = par.visualize;

sz = size(I); sz=sz(1:2);
np = prod(sz);

[W,samp] = computeW(I,par);

% constraints
p = find(samp);
if isempty(mask),
    U = [];
else
    U = pargrp(double(mask & samp));
    U = U(p,:);
end

% cuts on samples; and then interpolate to the rest pixels
if VERBOSE
    fprintf('cncutting... ');
end
[Vp,S] = cncut(W(p,p),U,nv);
if VERBOSE
    fprintf('done\n');
end
if VERBOSE
    fprintf('interpolating... ');
end
q = find(not(samp));
if isempty(q),
    Vq = [];
else
    C = W(q,p);
    C = spmd1(C,1./double(sum(C,2)+eps),[]);
    Vq = C * Vp;
end
V = zeros(np,nv);
V([p;q],:) = [Vp; Vq];
for j=1:nv,
    z = reshape(V(:,j),sz);
    z = medfilt2(z,[7,7],'symmetric');
    V(:,j) = z(:);
end
if VERBOSE
    fprintf('done\n');
end

% discretization
if VERBOSE
  fprintf('discretizing... ');
end
X = getbinsol(V); 
if VERBOSE
  fprintf('done\n');
end

% visualization
if visualize
    figure;
    range = [min(V(:)),max(V(:))];
    for j=1:nv,
        z = reshape(V(:,j),sz);
        subplot(nv,2,j+j-1); imagesc(z,range); axis image; axis off;
        colormap(gray);
        title(sprintf('%5.4f',S(j)));
        z = reshape(X(:,j),sz);
        subplot(nv,2,j+j); imagesc(im2double(rgb2gray(I)).*z+not(z),[0,1]); axis image; axis off;
        colormap(gray);
    end
end

[mv,C] = max(X,[],2);

% TO DO:: Possibly need to do connected components cleanup on Seg as well.
Seg = reshape(C,[size(I,1) size(I,2)]);

% Force par.sp clusters by running kmeans on clusters from C
Sp = zeros(size(C));
N_clusters = par.sp;
cc = unique(C)';
c_off=0;
for c_i=cc
  inds_use = find(C==c_i);
  V_use = V(inds_use,:);
  
  % This segment should contribute its fair share, at least 1 though.
  N_this = max(1,round(N_clusters * length(inds_use)/np));

  if N_this>1
    Sp_this = kmeans(V_use,N_this,'Start','sample','Replicates',5,'EmptyAction','singleton');  
  else
    Sp_this = ones(size(V_use,1),1);
  end
  Sp(inds_use) = Sp_this + c_off;
  c_off = c_off + max(Sp_this);
end
Sp = reshape(Sp,[size(I,1) size(I,2)]);

% Connected components analysis, cut apart disconnected clusters
% Clean up Sp.  Merge small segments, break disconnected clusters.
MIN_SIZE = 30;
min_c = min(Sp(:)); max_c = max(Sp(:));
new_num = max_c+1;
for c_i=min_c:max_c
  [L,num] = bwlabel(Sp==c_i,4);
  if num > 1 | sum(L(:)) < MIN_SIZE
    for n_i=1:num
      the_inds = find(L==n_i);
      if length(the_inds) < MIN_SIZE
	% Merge each small segment with a nearby one.
	the_perim = bwperim(imdilate(L==n_i,strel('diamond',1)));
	the_perim(the_inds)=0;  % bwperim has errors on boundary.
	nhbs = find(the_perim);
	the_dists = dist2(V(the_inds,:),V(nhbs,:));
	the_dists = min(the_dists,[],1);
	[mind,mind] = min(the_dists);
	Sp(the_inds) = Sp(nhbs(mind));
      else
	% Renumber large segments.
	Sp(the_inds) = new_num;
	new_num = new_num+1;
      end
    end
  end
end

% Renumber segments.
Spv = Sp';
Spv = Spv(:);
uu = unique(Spv);
N_seg = length(uu);
the_map(uu) = 1:N_seg;
Spv = the_map(Spv);
Sp = reshape(Spv,[size(I,2) size(I,1)])';

