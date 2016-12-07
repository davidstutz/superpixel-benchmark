function [FeatsMatrix, scaling] = scale_data(FeatsMatrix, type, scaling_in) 
  FeatsMatrix = full(FeatsMatrix);
  scaling = []; 
  
  if(any(any(any(isinf(FeatsMatrix)))))
      display('Found infinity values in Features!! Setting them to 0' )
      FeatsMatrix(isinf(FeatsMatrix)) = 0;
  end
  
  if(any(any(any(isnan(FeatsMatrix)))))
      display('Found NaN values in Features!! Setting them to 0' )
      FeatsMatrix(isnan(FeatsMatrix)) = 0;
  end
  
  if(strcmp(type, 'zero_one'))
      %
      %       FeatsMatrix = FeatsMatrix';
      %       FeatsMatrix = (FeatsMatrix - repmat(min(FeatsMatrix,[],1),size(FeatsMatrix,1),1))...
      %           *spdiags(1./(max(FeatsMatrix,[],1)-min(FeatsMatrix,[],1))',0,size(FeatsMatrix,2),size(FeatsMatrix,2));
      %       FeatsMatrix = FeatsMatrix';
      %
      
      if(nargin == 2)
          %FeatsMatrix = double(FeatsMatrix);
          % from libsvm's faq
          scaling = zero_one_scaling(FeatsMatrix);
          %FeatsMatrix = single(FeatsMatrix);
      else
          scaling.to_subtract = scaling_in.to_subtract;
          scaling.to_divide = scaling_in.to_divide;
      end
      [FeatsMatrix] = normalize(FeatsMatrix, scaling);
  elseif(strcmp(type,'norm_1'))
      FeatsMatrix = FeatsMatrix./(repmat(sum(FeatsMatrix), size(FeatsMatrix,1), 1)+eps);
      scaling = [];
  elseif(strcmp(type, 'zscore'))
      if(nargin == 3)
          [FeatsMatrix] = normalize(FeatsMatrix, scaling_in);
          scaling = scaling_in;
      else
          [FeatsMatrix, m, sigma] = zscore(FeatsMatrix,0, 2);
          scaling.to_subtract = m;
          scaling.to_divide = 1./(sigma');
      end
  elseif(strcmp(type, 'none'))
      scaling = [];
  end
end

