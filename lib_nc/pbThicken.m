% T = pbThicken(P)
%
% Thicken P by taking max in 3x3 window
function T = pbThicken(P)
[MM,NN] = size(P);
B = im2col(P,[3 3],'sliding');
B = max(B,[],1);

T_c = reshape(B,[MM-3+1 NN-3+1]);
T = zeros(size(P));
T(2:end-1,2:end-1) = T_c;