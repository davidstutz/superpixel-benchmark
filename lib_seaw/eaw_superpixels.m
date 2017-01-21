%
% Edge-Avoiding Wavelets Superpixel Creation
%
% creates eaw-based superpixels based on scale x and images in folder 'Images' as follows:
% 
% Folder level_x: contains all scaling functions y of scale x  for each image 'image_name' as 'image_name_y.mat' 
% Folder imlevel_x: contains images to all scaling functions y of scale x  for each image 'image_name' as 'image_name_y.jpeg' 
% 
% Folder index_x: contains (superpixel-)matrices with indices to highest scaling functions of scale x of image 'image_name' as 'image_name.mat'
% Folder imindex_x: contains images of matrices with indices to highest scaling functions of scale x of image 'image_name' as 'image_name.jpeg'
%
%
% This code uses the implementation of the paper "Edge-Avoiding Wavelets and their
% Applications" SIGGRAPH 2009 which is written by Raanan Fattal 


function eaw_superpixels

clear

img_dir = '../../BSDS500/data/images/val/';
files   = dir(fullfile(img_dir,'*.jpg'));
progressbar
wave_type = 1;
for i = 1:length(files)
   
    I_path = fullfile(img_dir,files(i).name);
    
    G = imread(I_path);
    G = double(G) / 255 ;
    
    nlevels = floor(log2(min(size(G(:,:,1))))) ;
    
    [A W] = EAW(G(:,:,1),nlevels,wave_type,1,1) ;
    [a,b,c] = fileparts(files(i).name);

    outfileName = fullfile(strcat('imIndex_',num2str(nlevels)),sprintf('%s.jpeg',b));
    % if ~exist(outfileName, 'file')
        for j=nlevels:-1:3

            s = j+1;

            im = zeros(size(A{s}));
            imcell = cell(numel(im), 1);
            for k=1:numel(im)


                im(k) = 1;

                for level=1:j


                    ss = cell(2,1);
                    ss{2} = im;
                    ss{1} = zeros(size(A{s-level}));
                    w = cell(1,1);
                    w{1} = W{s-level};
                    im = igEAW(ss,w,wave_type); 
                    
                end
                imcell{k} = im;
                % mkdir(strcat('level_',num2str(j)));
                % outFileName = fullfile(strcat('level_',num2str(j)),sprintf('%s.mat',strcat(b,'_k_',num2str(k))));
                
                % save(outFileName,'im');

                % mkdir(strcat('imlevel_',num2str(j)));
                % outFileName = fullfile(strcat('imlevel_',num2str(j)),sprintf('%s.jpeg',strcat(b,'_k_',num2str(k))));
                
                % imwrite(im,outFileName);

                im = zeros(size(A{s}));
            end

            m = zeros([size(imcell{1},1) size(imcell{1},2) numel(im)]);
            for c=1:numel(im)

                m(:,:,c) = imcell{c};
            end
            
            [val,ind] = max(m,[],3);
            mkdir(strcat('index_',num2str(j)));
            outFileName = fullfile(strcat('index_',num2str(j)),sprintf('%s.mat',b));
            
            save(outFileName,'ind');
            % mkdir(strcat('imIndex_',num2str(j)));
            % outFileName = fullfile(strcat('imIndex_',num2str(j)),sprintf('%s.jpeg',b));
            % imwrite(ind,outFileName);
            
        end
    % end
    
    progressbar(i/length(files));
    
end

