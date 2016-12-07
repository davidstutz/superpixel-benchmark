function [Feats] = normalize(Feats, scaling)
    if(iscell(Feats))
        scaling.to_subtract = double(scaling.to_subtract);
        scaling.to_divide = double(scaling.to_divide);
        for i=1:length(Feats)
            Feats{i} = double(Feats{i});
            Feats{i} = single(full(spdiags(scaling.to_divide',0,size( Feats{i},1),size( Feats{i},1))*( Feats{i} - repmat(scaling.to_subtract,1,size( Feats{i},2)))));
        end
    else
      % break Feats into 1000 vectors chunks to save memory
      MAX_CHUNK_SIZE = 30000;
      n_chunks = ceil(size(Feats,2)/MAX_CHUNK_SIZE);
      chunk_size = repmat(MAX_CHUNK_SIZE,n_chunks,1);
      
      the_mod = mod(size(Feats,2),MAX_CHUNK_SIZE);
      if(the_mod)
        chunk_size(end) = the_mod;
      end
      previous_id = 0;
      for i=1:n_chunks
        interval = previous_id+1:previous_id + chunk_size(i);
        FeatsChunk = double(Feats(:,interval));
        scaling.to_divide = double(scaling.to_divide);
        scaling.to_subtract = double(scaling.to_subtract);
        Feats(:,interval) = single(full(spdiags(scaling.to_divide',0,size(FeatsChunk,1),size(FeatsChunk,1))*(FeatsChunk - repmat(scaling.to_subtract,1,size(FeatsChunk,2)))));
        previous_id = previous_id + chunk_size(i);
      end
      Feats(isnan(Feats)) = 0; % when a feature is constant in all training examples it gets NaN here  
    end
end