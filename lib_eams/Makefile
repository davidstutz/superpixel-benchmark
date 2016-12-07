#Makefile for Edge Detection Image SegmentatiON (EDISON) System
#Robust Image Understanding Laboratory
#by Chris M. Christoudias

#define compiler
CC		= g++

#define executable name
EXENAME		= edison

#define dependents
COMMAND_SOURCE	= prompt/main.cpp prompt/parser.cpp prompt/globalFnc.cpp prompt/edison.cpp
COMMAND_INCLUES	= prompt/parser.h prompt/defs.h prompt/error.h prompt/flags.h prompt/edison.h
IMAGE_SOURCE	= prompt/libppm.cpp
IMAGE_INCLUDES	= prompt/imfilt.h prompt/libppm.h
SEGM_SOURCE     = segm/ms.cpp segm/msSysPrompt.cpp segm/msImageProcessor.cpp segm/RAList.cpp segm/rlist.cpp
SEGM_INCLUDES   = segm/tdef.h segm/RAList.h segm/ms.h segm/msImageProcessor.h segm/msSys.h segm/rlist.h
EDGE_SOURCE     = edge/BgEdge.cpp edge/BgEdgeDetect.cpp edge/BgEdgeList.cpp edge/BgGlobalFc.cpp edge/BgImage.cpp
EDGE_INCLUDES	= edge/BgDefaults.h edge/BgEdge.h edge/BgEdgeDetect.h edge/BgImage.h

#define targets
all: EDISON

EDISON	: $(COMMAND_SOURCE) $(COMMAND_INCLUDES) $(IMAGE_SOURCE) $(IMAGE_INCLUDES) $(SEGM_SOURCE) $(SEGM_INCLUDES) $(EDGE_SOURCE) $(EDGE_INCLUDES)
	$(CC) -o $(EXENAME) $(COMMAND_SOURCE) $(IMAGE_SOURCE) $(SEGM_SOURCE) $(EDGE_SOURCE)





