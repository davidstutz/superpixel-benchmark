###################################################################
#                                                                 #
#         Superpixels via Pseudo-Boolean Optimization             #
#                                                                 #
#                            v2.0                                 #
#                                                                 #
#           http://yuhang.rsise.anu.edu.au/yuhang/                #
#                                                                 #
#            Yuhang Zhang(yuhang.zhang@anu.edu.au)                #
#            Research School of Computer Science                  #
#            The Australian National University                   #
#                                                                 #
###################################################################

1. This code can only be used for none commercial purpose. 

2. If you are using it for research, please cite paper [1] below. Depends on the optimization algorithm you choose, you might cite [2] or [3] as well.

3. CMakeLists.txt files have been provided to simplify the compiling. Users unfamiliar with CMake are recommended to read the step-by-step.txt file. 

4. To compile the code properly, a third party library named VXL is needed. Please download it from http://sourceforge.net/projects/vxl/ and unzip the downloaded file under the given directory SuperPB. In this case the unzipped files should be delivered into the empty folder "vxl-1.14.0". Then please build VXL under "vxl-1.14.0/bin". This will keep the consistency with the CMakeLists.txt. If you prefer to install VXL at another place or are using a version different from 1.14.0, please modify the corresponding lines in CMakeLists.txt
	
5. The executable file can be launched with the following command:

>SuperPB.exe imagename hsize vsize sigma algorithm

imagename: 	the input image, e.g. test.jpg
hsize:		the width of the initial vertical strips, controlling the width of superpixels, e.g. 20
vsize:		the height of the initial horizontal strips, controlling the height of superpixels, e.g. 20
sigma:		balancing the weight between regular shape and accurate edge, e.g. 20
algorithm:	0: maxflow; 1: Elimination, e.g. 1

Elimination runs faster than maxflow. The superpixels produced by the two optimization algorithms are both accurate but usually different.

6. If you have any questions about the code or the algorithm, please write to yuhang.zhang@anu.edu.au.

[1]Superpixels via pseudo-boolean optimization. 
	Y. Zhang, R. Hartley, J. Mashford, and S. Burn.
	In International Conference on Computer Vision, 2011.

[2]Minimizing energy functions on 4-connected lattices using elimination.
	Peter Carr, Richard Hartley
	In International Conference on Computer Vision, 2009.

[3]An Experimental Comparison of Min-Cut/Max-Flow Algorithms for Energy Minimization in Vision.
	Yuri Boykov and Vladimir Kolmogorov.
	In IEEE Transactions on Pattern Analysis and Machine Intelligence (PAMI), September 2004

###################################################################
#                                                                 #
#    MAXFLOW - software for computing mincut/maxflow in a graph   #
#                        Version 3.0                              #
#    http://www.cs.adastral.ucl.ac.uk/~vnk/software.html          #
#                                                                 #
#    Yuri Boykov (yuri@csd.uwo.ca)                                #
#    Vladimir Kolmogorov (vnk@adastral.ucl.ac.uk)                 #
#    2001-2006                                                    #
#                                                                 #
###################################################################

1. Introduction.

This software library implements the maxflow algorithm described in

	"An Experimental Comparison of Min-Cut/Max-Flow Algorithms for Energy Minimization in Vision."
	Yuri Boykov and Vladimir Kolmogorov.
	In IEEE Transactions on Pattern Analysis and Machine Intelligence (PAMI), 
	September 2004

This algorithm was developed by Yuri Boykov and Vladimir Kolmogorov
at Siemens Corporate Research. To make it available for public use,
it was later reimplemented by Vladimir Kolmogorov based on open publications.

If you use this software for research purposes, you should cite
the aforementioned paper in any resulting publication.

----------------------------------------------------------------------

REUSING TREES:

Starting with version 3.0, there is a also an option of reusing search
trees from one maxflow computation to the next, as described in

	"Efficiently Solving Dynamic Markov Random Fields Using Graph Cuts."
	Pushmeet Kohli and Philip H.S. Torr
	International Conference on Computer Vision (ICCV), 2005

If you use this option, you should cite
the aforementioned paper in any resulting publication.

Tested under windows, Visual C++ 6.0 compiler and unix (SunOS 5.8
and RedHat Linux 7.0, GNU c++ compiler).

##################################################################

2. License & disclaimer.

    Copyright 2001 Vladimir Kolmogorov (vnk@adastral.ucl.ac.uk), Yuri Boykov (yuri@csd.uwo.ca).

    This software can be used for research purposes only.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

##################################################################

3. Example usage.

This section shows how to use the library to compute
a minimum cut on the following graph:

		        SOURCE
		       /       \
		     1/         \2
		     /      3    \
		   node0 -----> node1
		     |   <-----   |
		     |      4     |
		     \            /
		     5\          /6
		       \        /
		          SINK

///////////////////////////////////////////////////

#include <stdio.h>
#include "graph.h"

int main()
{
	typedef Graph<int,int,int> GraphType;
	GraphType *g = new GraphType(/*estimated # of nodes*/ 2, /*estimated # of edges*/ 1); 

	g -> add_node(); 
	g -> add_node(); 

	g -> add_tweights( 0,   /* capacities */  1, 5 );
	g -> add_tweights( 1,   /* capacities */  2, 6 );
	g -> add_edge( 0, 1,    /* capacities */  3, 4 );

	int flow = g -> maxflow();

	printf("Flow = %d\n", flow);
	printf("Minimum cut:\n");
	if (g->what_segment(0) == GraphType::SOURCE)
		printf("node0 is in the SOURCE set\n");
	else
		printf("node0 is in the SINK set\n");
	if (g->what_segment(1) == GraphType::SOURCE)
		printf("node1 is in the SOURCE set\n");
	else
		printf("node1 is in the SINK set\n");

	delete g;

	return 0;
}


///////////////////////////////////////////////////
