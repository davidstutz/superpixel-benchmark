#ifndef MYRGB2LSB
#define MYRGB2LAB

#include<cmath>

// Change from RGB colour space to LAB colour space

void RGB2XYZ(unsigned char sR,unsigned char sG,unsigned char sB,double&	X,double& Y,double& Z)
{
	double R = sR/255.0;
	double G = sG/255.0;
	double B = sB/255.0;

	double r, g, b;

	if(R <= 0.04045)	r = R/12.92;
	else				r = pow((R+0.055)/1.055,2.4);
	if(G <= 0.04045)	g = G/12.92;
	else				g = pow((G+0.055)/1.055,2.4);
	if(B <= 0.04045)	b = B/12.92;
	else				b = pow((B+0.055)/1.055,2.4);

	X = r*0.412453 + g*0.357580 + b*0.180423;
	Y = r*0.212671 + g*0.715160 + b*0.072169;
	Z = r*0.019334 + g*0.119193 + b*0.950227;
}

void RGB2LAB(const unsigned char& sR, const unsigned char& sG, const unsigned char& sB, unsigned char& lval, unsigned char& aval, unsigned char& bval)
{
	double X, Y, Z;
	RGB2XYZ(sR, sG, sB, X, Y, Z);

	double epsilon = 0.008856;	//actual CIE standard
	double kappa   = 903.3;		//actual CIE standard

	double Xr = 0.950456;	//reference white
	double Yr = 1.0;		//reference white
	double Zr = 1.088754;	//reference white

	double xr = X/Xr;
	double yr = Y/Yr;
	double zr = Z/Zr;

	double fx, fy, fz;
	if(xr > epsilon)	fx = pow(xr, 1.0/3.0);
	else				fx = (kappa*xr + 16.0)/116.0;
	if(yr > epsilon)	fy = pow(yr, 1.0/3.0);
	else				fy = (kappa*yr + 16.0)/116.0;
	if(zr > epsilon)	fz = pow(zr, 1.0/3.0);
	else				fz = (kappa*zr + 16.0)/116.0;

	lval = (unsigned char)((116.0*fy-16.0)/100*255+0.5);
	aval = (unsigned char)(500.0*(fx-fy)+128+0.5);
	bval = (unsigned char)(200.0*(fy-fz)+128+0.5);
}

void myrgb2lab(unsigned char* r,unsigned char* g,unsigned char* b,
		unsigned char* L,unsigned char* A,unsigned char* B,
		int nRows,int nCols
	)
{
	for(int i=0;i<nRows;i++)
		for(int j=0;j<nCols;j++)
			RGB2LAB(r[i*nCols+j],g[i*nCols+j],b[i*nCols+j],L[i*nCols+j],A[i*nCols+j],B[i*nCols+j]);
}



#endif