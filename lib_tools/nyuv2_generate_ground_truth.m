% Copyright (c) 2016, David Stutz
% Contact: david.stutz@rwth-aachen.de, davidstutz.de
% All rights reserved.
% 
% Redistribution and use in source and binary forms, with or without modification,
% are permitted provided that the following conditions are met:
% 
% 1. Redistributions of source code must retain the above copyright notice,
%    this list of conditions and the following disclaimer.
% 
% 2. Redistributions in binary form must reproduce the above copyright notice,
%    this list of conditions and the following disclaimer in the
%    documentation and/or other materials provided with the distribution.
% 
% 3. Neither the name of the copyright holder nor the names of its contributors
%    may be used to endorse or promote products derived from this software
%    without specific prior written permission.
% 
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
% ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
% THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
% ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
% LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
% DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
% LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
% ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
% OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
% OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

function [] = nyuv2_generate_ground_truth(labels, instances, mask_border, list, outdir, factor)
% function generate_groundtruth_medfilt(labels, instances, mask_border, list, outdir, factor)
%
% This function is used to find and fill regions of the depth images that
% require filling.
%
% The factor defines the resizing factor for the images.

    if ~exist(outdir)
        system(['mkdir -p ' outdir]);
    end;

    maxlabel = 1000;
    assert(max(labels(:)) <= maxlabel);
    
    for ii=list'
      id = num2str(ii, '%08d');
      
      % find out the region that needs to be filled, use dilate/erode
      margin = 5;
      foreground = (labels(:, :, ii) > 0);
      foreground = imerode(imdilate(foreground, strel('disk', margin)), strel('disk', margin));
      cc = bwconncomp(~foreground);
      A = regionprops(cc, 'Area');
      min_background_area = 500;
      indA = find([A.Area] < min_background_area);
      
      for k = indA
          foreground(cc.PixelIdxList{k}) = 1;
      end;
      
      % now do repeated median filter to fill in foreground
      seg = double(instances(:, :, ii))*maxlabel + double(labels(:, :, ii));
      seg0 = seg;
      seg(seg == 0) = nan;
      
      for iter = 1: 100
        seg2 = nyuv2_mediannan(seg, 5);
        ind = find(isnan(seg) & ~isnan(seg2) & foreground);
        
        if isempty(ind)
            break;
        end;
        
        seg(ind) = seg2(ind);
      end;
      seg(isnan(seg)) = 0;

      step = 1./factor;
      
      groundTruth{1}.Segmentation = uint16(seg(step:step:end, step:step:end).*(1 - mask_border(step:step:end, step:step:end)));
      groundTruth{1}.Boundaries = logical(nyuv2_seg2bdry(seg(step:step:end, step:step:end), 'imageSize')) & ~imdilate(mask_border(step:step:end, step:step:end), strel('disk', 2));
      
      save([outdir '/' id '.mat'], 'groundTruth');
    end;
end