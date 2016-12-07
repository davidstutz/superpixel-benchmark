function [ ref_x, ref_y ] = Refined_seed( Region, r,deta )
        
    [x,y]=meshgrid(1:(2*r));
    g=sqrt((x-r).^2+(y-r).^2);
    %creat  Gass function
     z=exp(-((g.^2)/(deta^2*2 )));
%    z=exp(-(1/((50*r)^2 *2))*g.^2);
    Region=Region+0.01;
    temp_Region = Region.* z; 

    max_temp = max(max(temp_Region));
    index=find(temp_Region==max_temp);
    ref_y = mod(index(1),(2*r));
    if( ref_y == 0)
        ref_y = 2*r;
    end
    ref_x = (index(1)-ref_y)/(2*r)+1;
    
end

