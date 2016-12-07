function FB = make_filterbank(num_ori,filter_scales,wsz,enlong)
%
%  F = make_filterbank(num_ori,num_scale,wsz)
%

if nargin<4,
    enlong = 3;
end

enlong = enlong*2;

% definine filterbank
%num_ori=6;
%num_scale=3;

num_scale = length(filter_scales);

M1=wsz; % size in pixels
M2=M1;

ori_incr=180/num_ori;
ori_offset=ori_incr/2; % helps with equalizing quantiz. error across filter set

FB=zeros(M1,M2,num_ori,num_scale);


% elongated filter set
counter = 1;

for m=1:num_scale  
   for n=1:num_ori
      % r=12 here is equivalent to Malik's r=3;
      f=doog1(filter_scales(m),enlong,ori_offset+(n-1)*ori_incr,M1);
      FB(:,:,n,m)=f;
   end
end

FB=reshape(FB,M1,M2,num_scale*num_ori);
total_num_filt=size(FB,3);

for j=1:total_num_filt,
  F = FB(:,:,j);
  a = sum(sum(abs(F)));
  FB(:,:,j) = FB(:,:,j)/a;
end

