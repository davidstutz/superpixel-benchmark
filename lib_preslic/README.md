 This package provides the C++ sources and a Matlab wrapper of the Preemptive SLIC 
 implementation described in:
 
 "Compact Watershed and Preemptive SLIC: On improving trade-offs of superpixel 
 segmentation algorithms" Peer Neubert and Peter Protzel, ICPR 2014 
 
The source code is only tested under Linux. Simply call 
$ make 
to compile the demo application in demo_singleImage.cpp

Running the application with
$ ./preemptiveSLIC
demonstrates several segmentations of the lena.bmp image with different compactness 
values. Each segmentation is done 100 times and the average runtime is shown.

To compile the Matlab wrapper go to the matlab folder and run:
$ mex mex_preemptiveSLIC.cpp ../preemptiveSLIC.cpp mex_helper.cpp -I.. $(pkg-config --cflags --libs opencv)

Call the Matlab function from Matlab with
>> L = mex_preemptiveSLIC(uint8(I), N, compactness)
to get the label image. E.g. setting the number of segments N=1000 and the compactness=25

