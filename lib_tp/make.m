% The make utility for all the C and MEX code

function make(command)

if (nargin > 0 && strcmp(command,'clean'))
    delete('*.mexglx');
    delete('*.mexw32');
    delete('lsmlib/*.mexglx');
    delete('lsmlib/*.mexw32');
    return;
end
mex CC=g++ DT.c
mex CC=g++ height_function_der.c
mex CC=g++ height_function_grad.c
mex CC=g++ local_min.c
mex CC=g++ zero_crossing.c
mex CC=g++ -lm get_full_speed.c
mex CC=gcc corrDn.c wrap.c convolve.c edges.c
mex CC=gcc upConv.c wrap.c convolve.c edges.c

cd lsmlib
mex CC=g++ computeDistanceFunction2d.c FMM_Core.c FMM_Heap.c lsm_FMM_field_extension2d.c
mex CC=g++ computeExtensionFields2d.c FMM_Core.c FMM_Heap.c lsm_FMM_field_extension2d.c
mex CC=g++ doHomotopicThinning.c FMM_Core.c FMM_Heap.c lsm_FMM_field_extension2d.c
cd ..