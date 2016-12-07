// KBasicStructures.cpp: implementation of the KBasicStructures class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BasicStructures.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BasicStructures::dPoint2D::dPoint2D(double dx , double dy )
{
	x = dx;
	y = dy;
}

BasicStructures::dPoint3D::dPoint3D(double dx , double dy, double dz )
{
	x = dx;
	y = dy;
	z = dz;
}


BasicStructures::node::node()
{
}

BasicStructures::Segnode::Segnode()
{
}

BasicStructures::HTransform::HTransform()
{
}



