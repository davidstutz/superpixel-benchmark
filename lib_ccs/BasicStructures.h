// BasicStructures.h: interface for the KBasicStructures class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASICSTRUCTURES_H__6BB0E354_6313_421E_A682_CFCF087DA907__INCLUDED_)
#define AFX_BASICSTRUCTURES_H__6BB0E354_6313_421E_A682_CFCF087DA907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning ( disable : 4786)

#include <vector>



class BasicStructures  
{
public:

	class dPoint2D
	{
	public:
		dPoint2D(double dx=0 , double dy=0 ); 
		double x;
		double y;
	};
	class dPoint3D
	{

	public:
		dPoint3D(double dx=0 , double dy=0, double dz = 0 ); 
		double x;
		double y;
		double z;
	};

	class node 
	{
	public:
		node();

		int xL;
		int yL;
	
		int xR;
		int index;
		double value;
		double valueR;
		double valueG;
		double valueB;
	};

	class Segnode
	{
	public:
		Segnode();

		int Xmin;
		int Xmax;
		int Ymin;
		int Ymax;
		double Xmean;
		double Ymean;
		double n1;
		double n2;
		double n3;
		double n4;
		int depthNo;
		double Imean;
		double ImeanR;
		double ImeanG;
		double ImeanB;
		double MotionX;
		double MotionY;
		int size;
	};

	class HTransform
	{
	public:
		HTransform();
		double h11;
		double h12;
		double h13;
		double h21;
		double h22;
		double h23;
		double h31;
		double h32;
		double h33;
	};
};

typedef BasicStructures::node node;
typedef BasicStructures::Segnode Segnode;
typedef BasicStructures::dPoint2D dPoint2D;
typedef BasicStructures::dPoint3D dPoint3D;
typedef BasicStructures::HTransform HTransform;


typedef std::vector<  int  > Dynamic1D;
typedef std::vector<Dynamic1D*> DynMatrix;

typedef std::vector<double*> Dynamic1Dp;
typedef std::vector<dPoint2D> Dynamic2D;
typedef std::vector<dPoint3D> Dynamic3D;

typedef std::vector< node  > DynamicList;
typedef std::vector< Segnode  > DynamicSegList;
typedef std::vector< HTransform  > HomographyList;

#endif 
