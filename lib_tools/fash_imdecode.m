% Taken from <http://vision.is.tohoku.ac.jp/~kyamagu/research/paperdoll/>

function output = fash_imdecode( input, fmt, varargin )
% IMDECODE Decompress image data in the specified format
%
%    output = imencode(input)
%    output = imencode(input, fmt)
%
% IMDECODE decompresses binary array INPUT into image data OUTPUT using
% specified format FMT. FMT is a name of image file extension that is
% recognized by IMFORMATS function, such as 'jpg' or 'png'. When FMT is
% omitted, 'png' is used as default.
%
% See also imencode imformats imread
%
% Taken from <http://vision.is.tohoku.ac.jp/~kyamagu/research/paperdoll/>

    if nargin < 2, fmt = 'png'; end

    tempfile = sprintf('%s.%s', tempname, fmt);
    try
        fid = fopen(tempfile, 'w');
        fwrite(fid, input, 'uint8');
        fclose(fid);
        output = imread(tempfile, fmt, varargin{:});
        delete(tempfile);
    catch e
        if exist(tempfile, 'file')
            delete(tempfile);
        end
        rethrow(e);
    end
end

