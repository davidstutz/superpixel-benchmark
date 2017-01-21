/******************************************/
/*  Edge-Avoiding Wavelets Implementation */
/*  See "Edge-Avoiding Wavelets and their */
/*  Applications" SIGGRAPH 2009 R. Fattal */
/******************************************/


#ifndef _EAW_HH
#define _EAW_HH

#define UPDT 0.5

#define TABLE_LEN 1024

double *dist_table ;

/*************************************/
/*        Warp for mxArray           */
/*************************************/

struct Grid {
    int len_x ;
    int len_y ;
    double *data ;
    mxArray *mxa ;
    
    Grid() { data = 0 ; }
    
    Grid(const mxArray* pmxa) {
        // does not have valid poiner 
        mxa = 0 ;
        data = mxGetPr(pmxa);
        len_x = mxGetM(pmxa);
        len_y = mxGetN(pmxa) ;
    }
           
    Grid(int lx, int ly) {
        set(lx,ly) ;
    }
    
    void operator=(Grid& A) {        
        set(A.len_x,A.len_y);
        for(int i = 0 ; i < len_x*len_y ; i++)
            data[i] = A.data[i] ;
    }
    
    double& operator()(int x, int y) {
        return data[x+y*len_x] ;
    }
    
    double& operator[](int i) { return data[i] ; }
            
    void set(int lx, int ly) {
        len_x = lx ;
        len_y = ly ;
        mxa = mxCreateDoubleMatrix(len_x,len_y,mxREAL) ;
        data = mxGetPr(mxa);
    }
}  ;


// initiates distance table (1/d)

void init(double alpha, double eps=0.0001) {
    dist_table = new double[TABLE_LEN] ;
    
    for(int i = 0 ; i < TABLE_LEN ; i++) {
        double v = 4.0 * (((double)i+0.5) / TABLE_LEN - 0.5) ;
        dist_table[i] = pow(fabs(v) + eps, -alpha) ;
        
    }
}

// initiates distance table (exp(-d^2))

void init_exp(double alpha, double eps=0.001) {
    
    dist_table = new double[TABLE_LEN] ;
    
    for(int i = 0 ; i < TABLE_LEN ; i++) {
        double v = 4.0 * (((double)i+0.5) / TABLE_LEN - 0.5) ;
        v *= alpha ;
        dist_table[i] = exp(-(v*v)) ;
    }
}

double dist(double v) {
  
    if(v > 2 || v < -2)
      return dist_table[0] ;
  
    int i = (int)((v + 2) * 0.25 * TABLE_LEN) ;
    return dist_table[i] ;
}

/***************************************************/
/*             Separable Wavelets                  */
/***************************************************/


void SPW(Grid& OA, Grid& A, Grid& W) {
    
    double w1, w2, w3, w4, sw ;
    double A1, A2, A3, A4 ;
    double Axy ;
    
    int nx = OA.len_x ;
    int ny = OA.len_y ;
    
    W.set(nx*ny*3 ,1) ;
    
    int nxm = 2 * ((nx-1)/2) ;
    int nym = 2 * ((ny-1)/2) ;
    
    int i = 0 ;
    
    A = OA ;
    
    for(int y = 0 ; y < nym ; y+=2) {
        for(int x = 0 ; x < nxm ; x+=2) {
            
            Axy = A(x+1,y) ;
            
            A1 = A(x,y) ;
            A2 = A(x+2,y) ;
            
            w1 = dist(A1-Axy) ;
            w2 = dist(A2-Axy) ;
            sw = 1.0 / (w1 + w2) ;
            
            w1 *= sw ;
            w2 *= sw ;
            W[i++] = w1 ;
            W[i++] = w2 ;
            
            A(x+1,y) = Axy - (w1 * A1 + w2 * A2) ;
            
            Axy = A(x,y+1) ;
            // A1 = A(x,y) ;
            A2 = A(x,y+2) ;
            
            w1 = dist(A1-Axy) ;
            w2 = dist(A2-Axy) ;
            sw = 1.0 / (w1 + w2) ;
            
            w1 *= sw ;
            w2 *= sw ;
            W[i++] = w1 ;
            W[i++] = w2 ;
            
            A(x,y+1) = Axy - (w1 * A1 + w2 * A2) ;
            
            Axy = A(x+1,y+1) ;
            // A1 = A(x,y) ;
            // A2 = A(x,y+2) ;
            A3 = A(x+2,y) ;
            A4 = A(x+2,y+2) ;
            
            w1 = dist(A1-Axy) ;
            w2 = dist(A2-Axy) ;
            w3 = dist(A3-Axy) ;
            w4 = dist(A4-Axy) ;
            
            sw = 1.0 / (w1 + w2 + w3 + w4) ;
            
            w1 *= sw ;
            w2 *= sw ;
            w3 *= sw ;
            w4 *= sw ;
            
            W[i++] = w1 ;
            W[i++] = w2 ;
            W[i++] = w3 ;
            W[i++] = w4 ;
            
            A(x+1,y+1) = Axy - (w1 * A1 + w2 * A2 + w3 * A3 + w4 * A4) ;
        }
        
        Axy = A(nxm,y) ;
        A(nxm,y+1) = A(nxm,y+1) - Axy ;
        
        if(nxm + 1 < nx) {
            A(nxm+1,y) = A(nxm+1,y) - Axy ;
            A(nxm+1,y+1) = A(nxm+1,y+1) - Axy ;
        }
    }
    
    
    for(int x = 0 ; x < nxm+1 ; x+=2) {
        
        Axy = A(x,nym) ;
        
        if(x + 1 < nx)
            A(x+1,nym) = A(x+1,nym) - Axy ;
        
        if(nym + 1 < ny) {
            A(x,nym+1) = A(x,nym+1) - Axy ;
            
            if(x + 1 < nx)
                A(x+1,nym+1) = A(x+1,nym+1) - Axy ;
        }
    }
    
    i = nx * ny * 2 ;
    
    nxm = nx - 1 ;
    nym = ny - 1 ;
    
    for(int y = 2 ; y < nym ; y+=2)
        for(int x = 2 ; x < nxm ; x+=2) {
        Axy = OA(x,y) ;
        
        w1 = dist(OA(x+1,y)-Axy) ;
        w2 = dist(OA(x-1,y)-Axy) ;
        w3 = dist(OA(x,y+1)-Axy) ;
        w4 = dist(OA(x,y-1)-Axy) ;
        
        sw = UPDT / (w1 + w2 + w3 + w4) ;
        
        w1 *= sw ;
        w2 *= sw ;
        w3 *= sw ;
        w4 *= sw ;
        
        W[i++] = w1 ;
        W[i++] = w2 ;
        W[i++] = w3 ;
        W[i++] = w4 ;
        
        A(x,y) += (w1 * A(x+1,y) + w2 * A(x-1,y) + w3 * A(x,y+1) + w4 * A(x,y-1)) ;
        }
}



/***************************************************/
/*             Separable Wavelets                  */
/***************************************************/



void iSPW(Grid& OA, Grid& A, Grid& W) {
    A = OA ;
    
    int nx = A.len_x ;
    int ny = A.len_y ;
    
    double w1, w2, w3, w4 ;
    double A1, A2, A3, A4 ;
    double Axy ;
    
    int i = nx * ny * 2 ;
    
    int nxm = nx - 1 ;
    int nym = ny - 1 ;
    
    double t ;
    
    for(int y = 2 ; y < nym ; y+=2)
        for(int x = 2 ; x < nxm ; x+=2) {
        t = W[i++] * A(x+1,y) ;
        t += W[i++] * A(x-1,y) ;
        t += W[i++] * A(x,y+1) ;
        t += W[i++] * A(x,y-1) ;
        
        A(x,y) += t ;
        }
    
    i = 0 ;
    
    nxm = 2 * ((nx-1)/2) ;
    nym = 2 * ((ny-1)/2) ;
    
    for(int y = 0 ; y < nym ; y+=2) {
        for(int x = 0 ; x < nxm ; x+=2) {
            
            Axy = A(x+1,y) ;
            A1 = A(x,y) ;
            A2 = A(x+2,y) ;
            
            w1 = W[i++] ;
            w2 = W[i++] ;
            
            A(x+1,y) = Axy + (w1 * A1 + w2 * A2) ;
            
            Axy = A(x,y+1) ;
            // A1 = A(x,y) ;
            A2 = A(x,y+2) ;
            
            w1 = W[i++] ;
            w2 = W[i++] ;
            
            A(x,y+1) = Axy + (w1 * A1 + w2 * A2) ;
            
            Axy = A(x+1,y+1) ;
            // A1 = A(x,y) ;
            // A2 = A(x,y+2) ;
            A3 = A(x+2,y) ;
            A4 = A(x+2,y+2) ;
            
            w1 = W[i++] ;
            w2 = W[i++] ;
            w3 = W[i++] ;
            w4 = W[i++] ;
            
            A(x+1,y+1) = Axy + (w1 * A1 + w2 * A2 + w3 * A3 + w4 * A4) ;
        }
        
        Axy = A(nxm,y) ;
        A(nxm,y+1) = A(nxm,y+1) + Axy ;
        
        if(nxm + 1 < nx) {
            A(nxm+1,y) = A(nxm+1,y) + Axy ;
            A(nxm+1,y+1) = A(nxm+1,y+1) + Axy ;
        }
    }
    
    for(int x = 0 ; x < nxm+1 ; x+=2) {
        
        Axy = A(x,nym) ;
        
        if(x + 1 < nx)
            A(x+1,nym) = A(x+1,nym) + Axy ;
        
        if(nym + 1 < ny) {
            A(x,nym+1) = A(x,nym+1) + Axy ;
            
            if(x + 1 < nx)
                A(x+1,nym+1) = A(x+1,nym+1) + Axy ;
        }
    }
}




/***************************************************/
/*             Red-Black Wavelets                  */
/***************************************************/


void WRB(Grid& OA, Grid& A, Grid& W) {
    
    A = OA ;
    
    int nx = A.len_x ;
    int ny = A.len_y ;
    
    double w1, w2, w3, w4, sw ;
    double A1=0, A2=0, A3=0, A4=0 ;
    double Axy ;
    
    W.set(nx*ny*8,1) ;
    
    int nxm = nx - 1 ;
    int nym = ny - 1 ;
    
    int i = 0 ;
    
    // PREDICT I
    
    for(int y = 0 ; y < ny ; y++) {
        for(int x = 0 ; x < nx ; x++) {
            if((x+y) % 2 == 0)
                continue ;
            
            if(x > 0 && y > 0 && x < nxm && y < nym) {
                
                Axy = A(x,y) ;
                A1 = A(x+1,y) ;
                A2 = A(x-1,y) ;
                A3 = A(x,y+1) ;
                A4 = A(x,y-1) ;
                
                w1 = dist(A1-Axy) ;
                w2 = dist(A2-Axy) ;
                w3 = dist(A3-Axy) ;
                w4 = dist(A4-Axy) ;
                
                sw = 1.0 / (w1 + w2 + w3 + w4) ;
                
                w1 *= sw ;
                w2 *= sw ;
                w3 *= sw ;
                w4 *= sw ;
                
                W[i++] = w1 ;
                W[i++] = w2 ;
                W[i++] = w3 ;
                W[i++] = w4 ;
                
                A(x,y) -= (w1 * A1 + w2 * A2 + w3 * A3 + w4 * A4) ;
            }
            else {
                
                Axy = A(x,y) ;
                
                if(x + 1 < nx) {
                    A1 = A(x+1,y) ;
                    w1 = dist(A1-Axy) ;
                }
                else
                    w1 = 0 ;
                
                
                if(x - 1 >= 0) {
                    A2 = A(x-1,y) ;
                    w2 = dist(A2-Axy) ;
                }
                else
                    w2 = 0 ;
                
                
                if(y + 1 < ny) {
                    A3 = A(x,y+1) ;
                    w3 = dist(A3-Axy) ;
                }
                else
                    w3 = 0 ;
                
                
                if(y - 1 >= 0) {
                    A4 = A(x,y-1) ;
                    w4 = dist(A4-Axy) ;
                }
                else
                    w4 = 0 ;
                
                sw = 1.0 / (w1 + w2 + w3 + w4) ;
                
                w1 *= sw ;
                w2 *= sw ;
                w3 *= sw ;
                w4 *= sw ;
                
                if(x + 1 < nx) {
                    W[i++] = w1 ;
                    A(x,y) -= w1 * A1 ;
                }
                
                if(x - 1 >= 0) {
                    W[i++] = w2 ;
                    A(x,y) -= w2 * A2 ;
                }
                
                if(y + 1 < ny) {
                    W[i++] = w3 ;
                    A(x,y) -= w3 * A3 ;
                }
                
                if(y - 1 >= 0) {
                    W[i++] = w4 ;
                    A(x,y) -= w4 * A4 ;
                }
            }
        }
    }
    
    
    i = 2 * nx * ny ;
    
    
    
    // UPDATE I
    
    for(int y = 0 ; y < ny ; y++) {
        for(int x = 0 ; x < nx ; x++) {
            if((x+y) % 2 == 1)
                continue ;
            
            if(x > 0 && y > 0 && x < nxm && y < nym) {
                
	        Axy = OA(x,y) ; // needed for weights
                A1 = OA(x+1,y) ;
                A2 = OA(x-1,y) ;
                A3 = OA(x,y+1) ;
                A4 = OA(x,y-1) ;
                
                w1 = dist(A1-Axy) ;
                w2 = dist(A2-Axy) ;
                w3 = dist(A3-Axy) ;
                w4 = dist(A4-Axy) ;
                
                sw = UPDT / (w1 + w2 + w3 + w4) ;
                
                w1 *= sw ;
                w2 *= sw ;
                w3 *= sw ;
                w4 *= sw ;
                
                W[i++] = w1 ;
                W[i++] = w2 ;
                W[i++] = w3 ;
                W[i++] = w4 ;
                
                A1 = A(x+1,y) ;
                A2 = A(x-1,y) ;
                A3 = A(x,y+1) ;
                A4 = A(x,y-1) ;
                
                A(x,y) += (w1 * A1 + w2 * A2 + w3 * A3 + w4 * A4) ;
            }
            else {
                
                Axy = OA(x,y) ;
                
                if(x + 1 < nx) {
                    A1 = OA(x+1,y) ;
                    w1 = dist(A1-Axy) ;
                }
                else
                    w1 = 0 ;
                
                
                if(x - 1 >= 0) {
                    A2 = OA(x-1,y) ;
                    w2 = dist(A2-Axy) ;
                }
                else
                    w2 = 0 ;
                
                
                if(y + 1 < ny) {
                    A3 = OA(x,y+1) ;
                    w3 = dist(A3-Axy) ;
                }
                else
                    w3 = 0 ;
                
                
                if(y - 1 >= 0) {
                    A4 = OA(x,y-1) ;
                    w4 = dist(A4-Axy) ;
                }
                else
                    w4 = 0 ;
                
                sw = UPDT / (w1 + w2 + w3 + w4) ;
                
                w1 *= sw ;
                w2 *= sw ;
                w3 *= sw ;
                w4 *= sw ;
                
                if(x + 1 < nx) {
                    W[i++] = w1 ;
                    A(x,y) += w1 * A(x+1,y) ;
                }
                
                if(x - 1 >= 0) {
                    W[i++] = w2 ;
                    A(x,y) += w2 * A(x-1,y) ;
                }
                
                if(y + 1 < ny) {
                    W[i++] = w3 ;
                    A(x,y) += w3 * A(x,y+1) ;
                }
                
                if(y - 1 >= 0) {
                    W[i++] = w4 ;
                    A(x,y) += w4 * A(x,y-1) ;
                }
            }
        }
    }
    
    
    i = 4 * nx * ny ;
    
    
    // PREDICT II
    
    for(int y = 1 ; y < ny ; y+=2) {
        for(int x = 1 ; x < nx ; x+=2) {
            
            if(x > 0 && y > 0 && x < nxm && y < nym) {
                
                Axy = A(x,y) ;
                A1 = A(x-1,y+1) ;
                A2 = A(x-1,y-1) ;
                A3 = A(x+1,y+1) ;
                A4 = A(x+1,y-1) ;

	        w1 = dist(A1-Axy) ;
                w2 = dist(A2-Axy) ;
                w3 = dist(A3-Axy) ;
                w4 = dist(A4-Axy) ;
                
                sw = 1.0 / (w1 + w2 + w3 + w4) ;
                
                w1 *= sw ;
                w2 *= sw ;
                w3 *= sw ;
                w4 *= sw ;
                
                W[i++] = w1 ;
                W[i++] = w2 ;
                W[i++] = w3 ;
                W[i++] = w4 ;
                
                
                A(x,y) -= (w1 * A1 + w2 * A2 + w3 * A3 + w4 * A4) ;
            }
            else {
                
                Axy = A(x,y) ;
                
                if(x - 1 >=0 && y + 1 < ny)
                    w1 = dist(A(x-1,y+1)-Axy) ;
                else
                    w1 = 0 ;
                
                
                if(x - 1 >=0 && y - 1 >= 0)
                    w2 = dist(A(x-1,y-1)-Axy) ;
                else
                    w2 = 0 ;
                
                
                if(x + 1 < nx && y + 1 < ny)
                    w3 = dist(A(x+1,y+1)-Axy) ;
                else
                    w3 = 0 ;
                
                
                if(x + 1 < nx && y - 1 >= 0)
                    w4 = dist(A(x+1,y-1)-Axy) ;
                else
                    w4 = 0 ;
                
                sw = 1.0 / (w1 + w2 + w3 + w4) ;
                
                w1 *= sw ;
                w2 *= sw ;
                w3 *= sw ;
                w4 *= sw ;
                
                if(x - 1 >=0 && y + 1 < ny) {
                    W[i++] = w1 ;
                    A(x,y) -= w1 * A(x-1,y+1) ;
                }
                
                if(x - 1 >=0 && y - 1 >= 0) {
                    W[i++] = w2 ;
                    A(x,y) -= w2 * A(x-1,y-1) ;
                }
                
                if(x + 1 < nx && y + 1 < ny) {
                    W[i++] = w3 ;
                    A(x,y) -= w3 * A(x+1,y+1) ;
                }
                
                if(x + 1 < nx && y - 1 >= 0) {
                    W[i++] = w4 ;
                    A(x,y) -= w4 * A(x+1,y-1) ;
                }
            }
        }
    }
    
    i = 6 * nx * ny ;
    
    
    // UPDATE II
    
    for(int y = 0 ; y < ny ; y+=2) {
        for(int x = 0 ; x < nx ; x+=2) {
            
            if(x > 0 && y > 0 && x < nxm && y < nym) {
                
                Axy = OA(x,y) ;
                A1 = OA(x-1,y+1) ;
                A2 = OA(x-1,y-1) ;
                A3 = OA(x+1,y+1) ;
                A4 = OA(x+1,y-1) ;
                
                w1 = dist(A1-Axy) ;
                w2 = dist(A2-Axy) ;
                w3 = dist(A3-Axy) ;
                w4 = dist(A4-Axy) ;
                
                sw = UPDT / (w1 + w2 + w3 + w4) ;
                
                w1 *= sw ;
                w2 *= sw ;
                w3 *= sw ;
                w4 *= sw ;
                
                W[i++] = w1 ;
                W[i++] = w2 ;
                W[i++] = w3 ;
                W[i++] = w4 ;
                
                A1 = A(x-1,y+1) ;
                A2 = A(x-1,y-1) ;
                A3 = A(x+1,y+1) ;
                A4 = A(x+1,y-1) ;

                A(x,y) += (w1 * A1 + w2 * A2 + w3 * A3 + w4 * A4) ;
            }
            else {
                
                Axy = OA(x,y) ;
                
                if(x - 1 >=0 && y + 1 < ny)
                    w1 = dist(OA(x-1,y+1)-Axy) ;
                else
                    w1 = 0 ;
                
                
                if(x - 1 >=0 && y - 1 >= 0)
                    w2 = dist(OA(x-1,y-1)-Axy) ;
                else
                    w2 = 0 ;
                
                
                if(x + 1 < nx && y + 1 < ny)
                    w3 = dist(OA(x+1,y+1)-Axy) ;
                else
                    w3 = 0 ;
                
                
                if(x + 1 < nx && y - 1 >= 0)
                    w4 = dist(OA(x+1,y-1)-Axy) ;
                else
                    w4 = 0 ;
                
                sw = UPDT / (w1 + w2 + w3 + w4) ;
                
                w1 *= sw ;
                w2 *= sw ;
                w3 *= sw ;
                w4 *= sw ;
                
                if(x - 1 >=0 && y + 1 < ny) {
                    W[i++] = w1 ;
                    A(x,y) += w1 * A(x-1,y+1) ;
                }
                
                if(x - 1 >=0 && y - 1 >= 0) {
                    W[i++] = w2 ;
                    A(x,y) += w2 * A(x-1,y-1) ;
                }
                
                if(x + 1 < nx && y + 1 < ny) {
                    W[i++] = w3 ;
                    A(x,y) += w3 * A(x+1,y+1) ;
                }
                
                if(x + 1 < nx && y - 1 >= 0) {
                    W[i++] = w4 ;
                    A(x,y) += w4 * A(x+1,y-1) ;
                }
            }
        }
    }
}




/***************************************************/
/*             Red-Black Wavelets                  */
/***************************************************/



void iWRB(Grid& OA, Grid& A, Grid& W) {
    
    A = OA ;
    
    int nx = A.len_x ;
    int ny = A.len_y ;
    
    double w1, w2, w3, w4 ;
    double A1=0, A2=0, A3=0, A4=0 ;
    
    int nxm = nx - 1 ;
    int nym = ny - 1 ;
    
    
    int i = 6 * nx * ny ;
    
    // UPDATE II
    
    for(int y = 0 ; y < ny ; y+=2) {
        for(int x = 0 ; x < nx ; x+=2) {
            
            if(x > 0 && y > 0 && x < nxm && y < nym) {
                
                w1 = W[i++] ;
                w2 = W[i++] ;
                w3 = W[i++] ;
                w4 = W[i++] ;
                
                A1 = A(x-1,y+1) ;
                A2 = A(x-1,y-1) ;
                A3 = A(x+1,y+1) ;
                A4 = A(x+1,y-1) ;
                
                A(x,y) -= (w1 * A1 + w2 * A2 + w3 * A3 + w4 * A4) ;
            }
            else {
                
                if(x - 1 >=0 && y + 1 < ny) {
                    w1 = W[i++] ;
                    A(x,y) -= w1 * A(x-1,y+1) ;
                }
                
                if(x - 1 >=0 && y - 1 >= 0) {
                    w2 = W[i++] ;
                    A(x,y) -= w2 * A(x-1,y-1) ;
                }
                
                if(x + 1 < nx && y + 1 < ny) {
                    w3 = W[i++] ;
                    A(x,y) -= w3 * A(x+1,y+1) ;
                }
                
                if(x + 1 < nx && y - 1 >= 0) {
                    w4 = W[i++] ;
                    A(x,y) -= w4 * A(x+1,y-1) ;
                }
            }
        }
    }
    
    
    
    i = 4 * nx * ny ;
    
    
    // PREDICT II
    
    for(int y = 1 ; y < ny ; y+=2) {
        for(int x = 1 ; x < nx ; x+=2) {
            
            if(x > 0 && y > 0 && x < nxm && y < nym) {
                
                w1 = W[i++] ;
                w2 = W[i++] ;
                w3 = W[i++] ;
                w4 = W[i++] ;
                
                A1 = A(x-1,y+1) ;
                A2 = A(x-1,y-1) ;
                A3 = A(x+1,y+1) ;
                A4 = A(x+1,y-1) ;
                
                A(x,y) += (w1 * A1 + w2 * A2 + w3 * A3 + w4 * A4) ;
            }
            else {
                
                if(x - 1 >=0 && y + 1 < ny) {
                    w1 = W[i++] ;
                    A(x,y) += w1 * A(x-1,y+1) ;
                }
                
                if(x - 1 >=0 && y - 1 >= 0) {
                    w2 = W[i++] ;
                    A(x,y) += w2 * A(x-1,y-1) ;
                }
                
                if(x + 1 < nx && y + 1 < ny) {
                    w3 = W[i++] ;
                    A(x,y) += w3 * A(x+1,y+1) ;
                }
                
                if(x + 1 < nx && y - 1 >= 0) {
                    w4 = W[i++] ;
                    A(x,y) += w4 * A(x+1,y-1) ;
                }
            }
        }
    }
    
    
    i = 2 * nx * ny ;
    
    
    
    // UPDATE I
    
    for(int y = 0 ; y < ny ; y++) {
        for(int x = 0 ; x < nx ; x++) {
            if((x+y) % 2 == 1)
                continue ;
            
            if(x > 0 && y > 0 && x < nxm && y < nym) {
                
                w1 = W[i++] ;
                w2 = W[i++] ;
                w3 = W[i++] ;
                w4 = W[i++] ;
                
                A1 = A(x+1,y) ;
                A2 = A(x-1,y) ;
                A3 = A(x,y+1) ;
                A4 = A(x,y-1) ;
                
                A(x,y) -= (w1 * A1 + w2 * A2 + w3 * A3 + w4 * A4) ;
            }
            else {
                
                if(x + 1 < nx) {
                    w1 = W[i++] ;
                    A(x,y) -= w1 * A(x+1,y) ;
                }
                
                if(x - 1 >= 0) {
                    w2 = W[i++] ;
                    A(x,y) -= w2 * A(x-1,y) ;
                }
                
                if(y + 1 < ny) {
                    w3 = W[i++] ;
                    A(x,y) -= w3 * A(x,y+1) ;
                }
                
                if(y - 1 >= 0) {
                    w4 = W[i++] ;
                    A(x,y) -= w4 * A(x,y-1) ;
                }
            }
        }
    }
    
    
    // PREDICT I
    
    
    i = 0 ;
    
    for(int y = 0 ; y < ny ; y++) {
        for(int x = 0 ; x < nx ; x++) {
            if((x+y) % 2 == 0)
                continue ;
            
            if(x > 0 && y > 0 && x < nxm && y < nym) {
                
                A1 = A(x+1,y) ;
                A2 = A(x-1,y) ;
                A3 = A(x,y+1) ;
                A4 = A(x,y-1) ;
                
                w1 = W[i++] ;
                w2 = W[i++] ;
                w3 = W[i++] ;
                w4 = W[i++] ;
                
                A(x,y) += (w1 * A1 + w2 * A2 + w3 * A3 + w4 * A4) ;
            }
            else {
                
                if(x + 1 < nx) {
                    w1 = W[i++] ;
                    A(x,y) += w1 * A(x+1,y) ;
                }
                
                if(x - 1 >= 0) {
                    w2 = W[i++] ;
                    A(x,y) += w2 * A(x-1,y) ;
                }
                
                if(y + 1 < ny) {
                    w3 = W[i++] ;
                    A(x,y) += w3 * A(x,y+1) ;
                }
                
                if(y - 1 >= 0) {
                    w4 = W[i++] ;
                    A(x,y) += w4 * A(x,y-1) ;
                }
            }
        }
    }
}



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////




/***************************************************/
/*             Guided Red-Black Wavelets           */
/*             for edge-preserving interpolation   */
/***************************************************/


void WRBg(Grid& OA, Grid& A, Grid& W) {
    
  double fac = UPDT ;
  double ofac = 1.0 - fac ;

    A = OA ;
    
    int nx = A.len_x ;
    int ny = A.len_y ;
    
    double w1, w2, w3, w4, sw ;
    double A1=0, A2=0, A3=0, A4=0 ;
    double Axy ;
    
    int nxm = nx - 1 ;
    int nym = ny - 1 ;
    
    int i = 0 ;
    
    // PREDICT I
        
    i = 2 * nx * ny ;
          
    // UPDATE I
    
    for(int y = 0 ; y < ny ; y++) {
        for(int x = 0 ; x < nx ; x++) {
            if((x+y) % 2 == 1)
                continue ;
            
            if(x > 0 && y > 0 && x < nxm && y < nym) {
                
	        A1 = A(x+1,y) ;
                A2 = A(x-1,y) ;
                A3 = A(x,y+1) ;
                A4 = A(x,y-1) ;

		w1 = W[i++] ;
		w2 = W[i++] ;
		w3 = W[i++] ;
		w4 = W[i++] ;

                A(x,y) = fac * A(x,y) + ofac * (w1 * A1 + w2 * A2 + w3 * A3 + w4 * A4) / UPDT ;
                
            }
            else {
                
                Axy = fac * A(x,y) ;
               
                if(x + 1 < nx) {
                    A1 = OA(x+1,y) ;
		    w1 = W[i++] / UPDT ;
		    Axy += w1 * ofac ;
                }
		
                if(x - 1 >= 0) {
                    A2 = OA(x-1,y) ;
                    w2 = W[i++] / UPDT ;
		    Axy += w2 * ofac ;
                }
                else
		  w2=0;
                
                if(y + 1 < ny) {
                    A3 = OA(x,y+1) ;
                    w3 = W[i++] / UPDT ;
		    Axy += w3 * ofac ;
                }
                else
		  w3=0;
                
                
                if(y - 1 >= 0) {
                    A4 = OA(x,y-1) ;
		    w4 = W[i++] / UPDT ;
		    Axy += w4 * ofac ;
		}
else
		  w4=0;
                                
          
	    }
	}
    }
    
    
    i = 4 * nx * ny ;
    
    
    // PREDICT II
    
    i = 6 * nx * ny ;
        
    // UPDATE II
    
    for(int y = 0 ; y < ny ; y+=2) {
        for(int x = 0 ; x < nx ; x+=2) {
            
            if(x > 0 && y > 0 && x < nxm && y < nym) {
                 
                w1 = W[i++] ;
                w2 = W[i++] ;
                w3 = W[i++] ;
                w4 = W[i++] ;
                
                A1 = A(x-1,y+1) ;
                A2 = A(x-1,y-1) ;
                A3 = A(x+1,y+1) ;
                A4 = A(x+1,y-1) ;
                
		A(x,y) = fac * A(x,y) + ofac * (w1 * A1 + w2 * A2 + w3 * A3 + w4 * A4) / UPDT ;
	    }
            else {
                
                Axy = fac * A(x,y) ;
                
                if(x - 1 >=0 && y + 1 < ny) {
                    w1 = W[i++] / UPDT ;
                    Axy += ofac * w1 * A(x-1,y+1) ;
                }
		else 
		  w1 = 0 ;
                
                if(x - 1 >=0 && y - 1 >= 0) {
                    w2 = W[i++] / UPDT ;
                    Axy += ofac * w2 * A(x-1,y-1) ;
                }
                else 
		  w2 = 0 ;
                
                if(x + 1 < nx && y + 1 < ny) {
                    w3 = W[i++] / UPDT ;
                    Axy += ofac * w3 * A(x+1,y+1) ;
                }
                else 
		  w3 = 0 ;
                
                if(x + 1 < nx && y - 1 >= 0) {
                    w4 = W[i++] / UPDT ;
                    Axy += ofac * w4 * A(x+1,y-1) ;
                }
		else 
		  w4 = 0 ;
                
		A(x,y) = Axy ;
            }
        }
    }
}



/***************************************************/
/*             Inverse Guided Red-Black Wavelets   */
/*             for edge-preserving interpolation   */
/***************************************************/



void iWRBg(Grid& OA, Grid& A, Grid& W) {
    
    A = OA ;
    
    int nx = A.len_x ;
    int ny = A.len_y ;
    
    double w1, w2, w3, w4 ;
    double A1=0, A2=0, A3=0, A4=0 ;
    double Axy ;

    int nxm = nx - 1 ;
    int nym = ny - 1 ;
    
    
    int i = 6 * nx * ny ;
    
    // UPDATE II
    
    
    i = 4 * nx * ny ;
    
    
    // PREDICT II
    
    for(int y = 1 ; y < ny ; y+=2) {
        for(int x = 1 ; x < nx ; x+=2) {
            
            if(x > 0 && y > 0 && x < nxm && y < nym) {
                
                w1 = W[i++] ;
                w2 = W[i++] ;
                w3 = W[i++] ;
                w4 = W[i++] ;
                
                A1 = A(x-1,y+1) ;
                A2 = A(x-1,y-1) ;
                A3 = A(x+1,y+1) ;
                A4 = A(x+1,y-1) ;
                
                A(x,y) = (w1 * A1 + w2 * A2 + w3 * A3 + w4 * A4) ;
            }
            else {
                
	      Axy = 0 ;
                if(x - 1 >=0 && y + 1 < ny) {
                    w1 = W[i++] ;
                    Axy += w1 * A(x-1,y+1) ;
                }
                
                if(x - 1 >=0 && y - 1 >= 0) {
                    w2 = W[i++] ;
                    Axy += w2 * A(x-1,y-1) ;
                }
                
                if(x + 1 < nx && y + 1 < ny) {
                    w3 = W[i++] ;
                    Axy += w3 * A(x+1,y+1) ;
                }
                
                if(x + 1 < nx && y - 1 >= 0) {
                    w4 = W[i++] ;
                    Axy += w4 * A(x+1,y-1) ;
                }
		A(x,y) = Axy ;
            }
        }
    }
    
    
    i = 2 * nx * ny ;
        
    // PREDICT I
    
    
    i = 0 ;
    
    for(int y = 0 ; y < ny ; y++) {
        for(int x = 0 ; x < nx ; x++) {
            if((x+y) % 2 == 0)
                continue ;
            
            if(x > 0 && y > 0 && x < nxm && y < nym) {
                
                A1 = A(x+1,y) ;
                A2 = A(x-1,y) ;
                A3 = A(x,y+1) ;
                A4 = A(x,y-1) ;
                
                w1 = W[i++] ;
                w2 = W[i++] ;
                w3 = W[i++] ;
                w4 = W[i++] ;
                
                A(x,y) = (w1 * A1 + w2 * A2 + w3 * A3 + w4 * A4) ;
            }
            else {
	      Axy = 0 ;
                if(x + 1 < nx) {
                    w1 = W[i++] ;
                    Axy += w1 * A(x+1,y) ;
                }
                
                if(x - 1 >= 0) {
                    w2 = W[i++] ;
                    Axy += w2 * A(x-1,y) ;
                }
                
                if(y + 1 < ny) {
                    w3 = W[i++] ;
                    Axy += w3 * A(x,y+1) ;
                }
                
                if(y - 1 >= 0) {
                    w4 = W[i++] ;
                    Axy += w4 * A(x,y-1) ;
                }
		A(x,y) = Axy ;
            }
        }
    }
}



///////////////////////////////////////////////////////////////////



#endif
