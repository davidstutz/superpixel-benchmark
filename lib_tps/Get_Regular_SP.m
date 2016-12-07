function [ superpixel_region_label, superpixel_map] = Get_Regular_SP( img, edge_map, H_num,W_num )
% Get the Regular Superpixel
        
    if nargin < 5
        gamma = 0.5;
    end;
    
    [ height width channel ] = size(img);
    superpixel_map = zeros(height,width);
    Hedge_label = zeros(height,width);
    Vedge_label = zeros(height,width);
    SP_num = H_num*W_num;
    r = min([round(width/(H_num*2));round(height/(W_num*2))]) -1;
    alpha = r/2;
    deta = 0.1*r;
    % generate the initial grid seeds 
    seed_h = zeros(W_num+1,H_num+1);
    seed_w = zeros(W_num+1,H_num+1);
    for i = 1:W_num+1
        for j = 1:H_num+1
            seed_h(i,j) = round((height/W_num)*(i-1))+1; 
            seed_w(i,j) = round((width/H_num)*(j-1))+1; 
        end
    end
    seed_h( seed_h > height )= height;
    seed_w( seed_w > width ) = width;

    % relocate the seeds
    refined_seed_w=seed_w;
    refined_seed_h=seed_h;
    for i = 2:W_num
        for j = 2:H_num
            Region = edge_map( seed_h(i,j) - r +1: seed_h(i,j) + r,  seed_w(i,j) - r +1: seed_w(i,j) + r );
            [refined_w, refined_h] = Refined_seed(Region, r, deta);
            refined_seed_w(i,j)=seed_w(i,j)-r+refined_w;
            refined_seed_h(i,j)=seed_h(i,j)-r+refined_h;
        end
    end

    temp_x = 1-r:r;
    Gauss_1D = exp(-temp_x.^2/(deta^2));
    for i = 2:W_num
        j = 1;  %col1
        RegionCol_1 = edge_map( seed_h(i,j) - r+1: seed_h(i,j) + r, seed_w(i,j)+2)+0.1;
        refinedC1_I = RegionCol_1.*Gauss_1D';
        temp = find(refinedC1_I==max(refinedC1_I));%in case of two max values
        refined_seed_h(i,j) = round(seed_h(i,j) - r + temp(1)); 

        j = H_num + 1;  %col m+1
        RegionCol_m = edge_map(( seed_h(i,j) - r+1):(seed_h(i,j)) + r, (seed_w(i,j))-2)+0.1;
        refinedCm_I = RegionCol_m.*Gauss_1D';
        temp = find(refinedCm_I==max(refinedCm_I));%in case of two max values
        refined_seed_h(i,j) = round(seed_h(i,j) - r + temp(1)); 
    end

    for j = 2:H_num
        i = 1;  %row 1
        RegionRow_1 = edge_map( seed_h(i,j)+2, seed_w(i,j) - r+1: seed_w(i,j) + r)+0.1;
        refinedR1_J = Gauss_1D.*RegionRow_1;
        temp = find(refinedR1_J==max(refinedR1_J));%in case of two max values
        refined_seed_w(i,j) = round(seed_w(i,j) - r + temp(1)); 

        i = W_num+1;    %row n+1
        RegionRow_n = edge_map( seed_h(i,j)-2, seed_w(i,j) - r+1: seed_w(i,j) + r)+0.1;
        refinedRn_J = Gauss_1D.*RegionRow_n;
        temp = find(refinedRn_J==max(refinedRn_J));%in case of two max values
        refined_seed_w(i,j) = round(seed_w(i,j) - r + temp(1)); 
    end

    % compute the shorest path between two seed.
    % Here we employ the function graphshortestpath() of Bioinformatics Toolbox

    % Generate horizontal edges
    for i = 1:W_num  
        for j = 2:H_num
            Cup_i = max(1,refined_seed_h(i,j) - alpha);
            Ddown_i = min(height,refined_seed_h(i+1,j) + alpha);
            Cleft_j = refined_seed_w(i,j) - alpha;
            Dleft_j = refined_seed_w(i+1,j) - alpha;
            Cright_j = refined_seed_w(i,j) + alpha;
            Dright_j = refined_seed_w(i+1,j) + alpha;  
            left_j = max(1, round(min(Cleft_j,Dleft_j)));
            right_j = min(width,round(max(Cright_j,Dright_j)));
            region_d = edge_map(round(Cup_i):round(Ddown_i), left_j:right_j);
            region_d_width = size(region_d,2);%region_d width
            start_C=round(refined_seed_h(i,j)-round(Cup_i))*region_d_width +round(refined_seed_w(i,j))-left_j+1;
            start_D=round(refined_seed_h(i+1,j)-round(Cup_i))*region_d_width + round(refined_seed_w(i+1,j))-left_j+1;
            PATH = Get_path_of_region( region_d ,start_C, start_D);
            for p=1:size(PATH,2)
                path_m=mod(PATH(p),region_d_width);
                if( path_m == 0)
                    path_m = region_d_width;
                end
                path_n=(PATH(p)-path_m)/(region_d_width);
                superpixel_map(path_n+round(Cup_i) ,path_m+round(left_j)-1)=255;
                if (Hedge_label(path_n+round(Cup_i) ,path_m+round(left_j)-1)==0)
                    Hedge_label(path_n+round(Cup_i) ,path_m+round(left_j)-1)=j-1;
                end
           end

        end
    end
    Hedge_label(:,width)=H_num;
    % Generate vertical edges
    for i = 2:W_num 
        for j = 1:H_num
            Aup_i = refined_seed_h(i,j) - alpha;
            Bup_i = refined_seed_h(i,j+1) - alpha;
            Adown_i = refined_seed_h(i,j) + alpha;
            Bdown_i = refined_seed_h(i,j+1) + alpha;
            Aleft_j = max(1, refined_seed_w(i,j) - alpha);
            Bright_j = min(width, refined_seed_w(i,j+1) + alpha);
            up_i = max(1, round(min(Aup_i,Bup_i)));
            down_i = min(height, round(max(Adown_i,Bdown_i)));
            % region_r is the original coordinate in edge map
            region_r = edge_map(up_i:down_i, round(Aleft_j):round(Bright_j));
            region_r_width = size(region_r,2);

            %get relative coordinate for the start and end point
            start_A=round(refined_seed_h(i,j)-up_i)*region_r_width +round(refined_seed_w(i,j))-round(Aleft_j)+1;
            start_B=round(refined_seed_h(i,j+1)-up_i)*region_r_width + round(refined_seed_w(i,j+1))-round(Aleft_j)+1;
            PATH = Get_path_of_region( region_r ,start_A, start_B);
            for p=1:size(PATH,2)
                path_m=mod(PATH(p),region_r_width);%col
                if( path_m == 0)
                    path_m = region_r_width;
                end
                path_n=(PATH(p)-path_m)/(region_r_width);%row
                superpixel_map(path_n+up_i,path_m+round(Aleft_j)-1)=255;
                if Vedge_label(path_n+up_i,path_m+round(Aleft_j)-1)==0
                    Vedge_label(path_n+up_i,path_m+round(Aleft_j)-1)=i-1;
                end
            end
        end
    end

    Vedge_label(height,:)=W_num;

    % obtain the region label.
    superpixel_region_label = Get_region_label( Hedge_label,Vedge_label,W_num,H_num );
    
end

