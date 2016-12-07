/*
Copyright 2011, Ming-Yu Liu

All Rights Reserved 

Permission to use, copy, modify, and distribute this software and 
its documentation for any non-commercial purpose is hereby granted 
without fee, provided that the above copyright notice appear in 
all copies and that both that copyright notice and this permission 
notice appear in supporting documentation, and that the name of 
the author not be used in advertising or publicity pertaining to 
distribution of the software without specific, written prior 
permission. 

THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
ANY PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR 
ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING 
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
*/
#include "MERCOutput.h"

void MERCOutput::StoreClusteringMap(vector<int> &label,const char *filename)
{
	std::ofstream file;
	file.open(filename);

	for(int i=0;i<label.size();i++)
	{
		file<<label[i]<<endl;
	}
	file.close();
}


vector<int> MERCOutput::DisjointSetToLabel(MERCDisjointSet *u)
{
	int nSegments=0;
	int segIndex = 0;
	int nVertices = u->rNumVertices();
	std::vector<int> *sarray = new std::vector<int> [nVertices];
	vector<int> labeling(nVertices);

	for (int k=0; k<nVertices; k++) 
	{
	  int comp = u->Find(k);
	  sarray[comp].push_back(k);
	}
	
	for(int k=0;k<nVertices;k++)
	{
		if(sarray[k].size() > 0)
		{
			nSegments++;
		}
	}

	for(int k=0;k<nVertices;k++)
	{
		if(sarray[k].size() > 0)
		{
			for(unsigned int j=0;j<sarray[k].size();j++)
			{
				labeling[sarray[k][j]] = segIndex;
			}
			segIndex++;
		}
	}
	delete [] sarray;
	return labeling;
}
