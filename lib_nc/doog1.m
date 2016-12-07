function H=doog1(sig,r,th,N);
% H=doog1(sig,r,th,N);


% by Serge Belongie

no_pts=N;  % no. of points in x,y grid

[x,y]=meshgrid(-(N/2)+1/2:(N/2)-1/2,-(N/2)+1/2:(N/2)-1/2);

phi=pi*th/180;
sigy=sig;
sigx=r*sig;
R=[cos(phi) -sin(phi); sin(phi) cos(phi)];
C=R*diag([sigx,sigy])*R';

X=[x(:) y(:)];

Gb=gaussian(X,[0 0]',C);
Gb=reshape(Gb,N,N);

m=R*[0 sig]';

a=1;
b=-1;

% make odd-symmetric filter
Ga=gaussian(X,m/2,C);
Ga=reshape(Ga,N,N);
Gb=rot90(Ga,2);
H=a*Ga+b*Gb; 

