function rgb = Luv2RGB(luvim)

if size(luvim,3) ~= 3
    error('im must have three color channels');
end
if ~isa(luvim,'float')
    luvim = im2single(luvim);
end
imsiz = size(luvim);

RGB = [ 3.2405, -1.5371, -0.4985 ; ...
    -0.9693,  1.8760,  0.0416 ; ...
    0.0556, -0.2040,  1.0573 ];
Up = 0.19784977571475;
Vp = 0.46834507665248;
Yn = 1.00000;

l = luvim(:,:,1);

y = Yn*l./903.3;
y(l>.8) = (l(l>.8)+16)/116;
y(l>.8) = Yn*(y(l>.8)).^3;

u = Up+luvim(:,:,2)./(13*l);
v = Vp+luvim(:,:,3)./(13*l);

x = 9*u.*y./(4*v);
z = (12-3*u-20*v).*y./(4*v);

rgb = RGB*reshape(permute(cat(3, x, y, z),[3 1 2]),[3 prod(imsiz(1:2))]);
rgb = reshape(rgb',imsiz);

zr = find(l < .1);
rgb([zr zr+prod(imsiz(1:2)) zr+2*prod(imsiz(1:2))]) = 0;
rgb = min(rgb,1);
rgb = max(rgb,0);