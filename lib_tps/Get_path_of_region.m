function [ PATH ] = Get_path_of_region( region_r ,start_A, start_B)
%GET_PATH_OF_REGION Summary of this function goes here
%   Detailed explanation goes here
        region_r_height = size(region_r,1);
        region_r_width = size(region_r,2);
        node_num=(region_r_width - 1)*(region_r_height - 1)*2;
        s_1=zeros(1,node_num); 
        s_2=zeros(1,node_num); 
        s_w=zeros(1,node_num);
        idx=1;
        for n = 1:region_r_height - 1% row
            for m = 1:region_r_width - 1% col
                index = (n - 1)*region_r_width + m;
                 s_1(idx) = index;
                 s_1(idx+1) = index;
                index_right = index + 1;
                index_down = index + region_r_width;
                s_2(idx) = index_right;
                s_2(idx+1) = index_down;
                weight_r = 1/(region_r(n,m)+region_r(n,m+1)+1);
                weight_d = 1/(region_r(n,m)+region_r(n+1,m)+1);
                s_w(idx) = weight_r;
                s_w(idx+1) = weight_d;
                idx=idx+2;
            end
        end
        G = sparse([s_1 s_2],[s_2 s_1],[s_w s_w]);
%         [x1,PATH,x3] = graphshortestpath(G,start_A,start_B,'METHOD','Dijkstra');
        [x1,PATH,x3] = graphshortestpath(G,start_A,start_B,'Directed',false);
end

