% [W,samp] = computeW(I,par)
% [W,samp] = computeW(I,par,I_mask)
%
% Same style as imncut.
% Uses intervening contour and/or intensity
% if I_mask is set, compute sparse W connection matrix from points in Mask region to points outside Mask region
function [W,samp] = computeW(I,par,varargin)
nb_r = par.nb_r;
sample_rate = par.sample_rate;
mask = par.mask;
sig_ic = par.sig_ic;
sig_pb_ic = par.sig_pb_ic;
sig_int = par.sig_int;
rep = par.rep;
reg = par.reg;
nv = par.nv;
hwx = par.hwx;
hwy = par.hwy;
PB_IC = par.pb_ic;
IC = par.ic;
INT = par.int;
COUNT = par.count;
VERBOSE = par.verbose;
visualize = par.visualize;
pb_timing = par.pb_timing;

if nargin >= 3
  I_mask = varargin{1};
end

% sampling
sz = size(I); sz=sz(1:2);
np = prod(sz);
if length(sample_rate)==1,
  samp = double(rand(sz)<sample_rate);
else
  samp = sample_rate;
end

% pixel affinity
if VERBOSE, fprintf('computing W... '); end

if nargin >= 3
  [ci,cj] = imnb(nb_r,samp,I_mask);
else
  [ci,cj] = imnb(nb_r,samp);
end

% count how many pixels connect between the regions, and that's it
if COUNT
  W = csparse(ci,cj,ones(size(ci)),np);
  if VERBOSE, fprintf('done\n'); end
  return;
end

clear cw;
if 0
  figure; imagesc(samp);
  % test the nb pattern
  Con = csparse(ci,cj,ones(size(ci)),np);
  [MX,MY] = meshgrid(1:sz(1),1:sz(2));
  XX = [MX(:) MY(:)];
  w_visualize(Con,XX,I);
end
if PB_IC
  % intervening contour using mfm-pb
  emag = par.pb_emag;
  ephase = par.pb_ephase;
  
  if any(size(emag) ~= sz) | any(size(ephase) ~= sz)
    error('par.pb_emag or par.pb_ephase of incorrect size');
  end
  a = 0.5 / (sig_pb_ic * sig_pb_ic);
  cw = a*ic(emag,ephase,ci,cj,mask); 
elseif IC
  % intervening contour with Stella's code
  [emag,ephase] = quadeg(im2double(rgb2gray(I)));
  a = 0.5 / (sig_ic * sig_ic);
  cw = a*ic(emag,ephase,ci,cj,mask); 
end
if INT
  if ~exist('cw')
    cw = zeros(size(ci));
  end
  % intensity
  a = 0.5 / (sig_int * sig_int * (2*hwx+1)*(2*hwy+1));
  cw = cw + a*wij_intensity(ci,cj,I,hwx,hwy);
end
if ~exist('cw')
  error('no positive parameter for type of Wij');
end
offset = -rep;
W = exp(-cw) + offset;
W = csparse(ci,cj,W,np);            
clear ci cj cw
if VERBOSE, fprintf('done\n'); end
