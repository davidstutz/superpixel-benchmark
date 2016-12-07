% M is a segmentation mask, entries in {1,2,...,K}.
% C is a segmentation mask, entries in {1,2,...,L}.
%
% Compute a segmentation respecting the boundaries in M, clustering points until there
% are no more than p_max pixels in a cluster.
% k-means clustering on (x,y) position is run on each cluster from M.
function C = clusterLocations(M,p_max)
C = zeros(size(M));
K = max(M(:));

c_off=0;
for k_i=1:K
  [xx,yy] = find(M==k_i);
  l_this = ceil(length(xx)/p_max);
  
  if l_this > 1
    idx = kmeans([xx yy],l_this,'Start','sample','Replicates',10,'EmptyAction','singleton');
  else
    idx = ones(size(xx));
  end
  
  inds = sub2ind(size(M),xx,yy);
  C(inds) = idx + c_off;
  
  c_off = c_off + l_this;
end

% Uniquify just in case clusters were lost in k-means.
Cv = C';
Cv = Cv(:);
uu = unique(Cv);
N_seg = length(uu);
the_map(uu) = 1:N_seg;
Cv = the_map(Cv);
C = reshape(Cv,[size(M,2) size(M,1)])';
