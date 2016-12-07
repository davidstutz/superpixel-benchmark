function [ superpixel_region_label ] = Get_region_label( superpixel_Hedge_label,superpixel_Vedge_label,W,H )
%GET_REGION_LABEL Summary of this function goes here
%   Detailed explanation goes here

    [ height width ] = size(superpixel_Hedge_label);
    superpixel_region_label = zeros(height,width);
    superpixel_Hedge_label_temp=superpixel_Hedge_label;
    superpixel_Hedge_label_temp(superpixel_Hedge_label_temp==0)=W*H*10;
    superpixel_Vedge_label_temp=superpixel_Vedge_label;
    superpixel_Vedge_label_temp(superpixel_Vedge_label_temp==0)=W*H*10;
    for i=1:height
        for j=1:width
            idx_hd = min(superpixel_Hedge_label_temp(i,j:width)); 
            idx_vd = min(superpixel_Vedge_label_temp(i:height,j)); 
            superpixel_region_label(i,j) = idx_hd + idx_vd - 2*H;
            superpixel_region_label(i,j) = idx_hd + (idx_vd-1)*H;
        end
    end

end

