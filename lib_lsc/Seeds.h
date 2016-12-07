#ifndef SEEDS
#define SEEDS

#include<iostream>
#include<cmath>
#include"point.h"
using namespace std;

//Initialize the seeds

int Seeds(int nRows,int nCols,int Row_num,int Col_num,int Row_step,int Col_step,int seed_num,point* point_array)
{
	int Row_remain=nRows-Row_step*Row_num;
	int Col_remain=nCols-Col_step*Col_num;
	int t1=1,t2=1;
	int count=0;
	int centerx,centery;
	for(int i=0;i<Row_num;i++)
	{
		t2=1;
		for(int j=0;j<Col_num;j++)
		{
			centerx=i*Row_step+0.5*Row_step+t1;
			centery=j*Col_step+0.5*Col_step+t2;
			centerx=(centerx>=nRows-1)?nRows-1:centerx;
			centery=(centery>=nCols-1)?nCols-1:centery;
			if(t2<Col_remain)
				t2++;
			point_array[count]=point(centerx,centery);
			count++;
		}
		if(t1<Row_remain)
				t1++;
	}
	return count;
}

#endif