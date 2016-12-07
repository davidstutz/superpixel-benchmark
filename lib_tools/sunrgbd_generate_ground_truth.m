function [] = sunrgbd_generate_ground_truth(mat_file, meta, gt_dir)

    mat = matfile(mat_file);
    index = 0;
    
    mkdir(gt_dir);
    
    for i = 1: 10335
        if strcmp(meta(1, i).sequenceName(13:19), 'NYUdata') ~= 1
            % This is not an NYUV2 image ...
            labels = mat.SUNRGBD2Dseg(1, i);
            labels = labels.seglabelall;
            
            % find out the region that needs to be filled, use dilate/erode
            margin = 5;
            foreground = (labels > 0);
            foreground = imerode(imdilate(foreground, strel('disk', margin)), strel('disk', margin));
            cc = bwconncomp(~foreground);
            A = regionprops(cc, 'Area');
            min_background_area = 500;
            indA = find([A.Area] < min_background_area);

            for k = indA
              foreground(cc.PixelIdxList{k}) = 1;
            end;

            % now do repeated median filter to fill in foreground
            seg = double(labels);
            seg(seg == 0) = nan;

            for iter = 1: 100
                seg2 = sunrgbd_mediannan(seg, 5);
                ind = find(isnan(seg) & ~isnan(seg2) & foreground);

                if isempty(ind)
                    break;
                end;

                seg(ind) = seg2(ind);
            end;
            
            seg(isnan(seg)) = 0;
            
            % End of processing!
            labels = seg(1:end, 1:end);
            labels = sp_fast_connected_relabel_k8(labels); 
            labels = sp_enforce_minimum_size(int32(labels));
            %[gt_dir '/' sprintf('%08d', index) '.csv'], i
            csvwrite([gt_dir '/' sprintf('%08d', index) '.csv'], labels);
            
            index = index + 1;
        end;
    end;
end

