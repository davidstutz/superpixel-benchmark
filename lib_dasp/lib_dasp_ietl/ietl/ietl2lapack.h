/***************************************************************************
 * $Id: ietl2lapack.h,v 1.9 2003/12/05 09:24:01 tprosser Exp $
 *
 * Copyright (C) 2001-2002 by Prakash Dayal <prakash@comp-phys.org>
 *                            Matthias Troyer <troyer@comp-phys.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 **************************************************************************/

#ifndef IETL_LAPACK_H
#define IETL_LAPACK_H

#include <ietl/ietl2lapack_interface.h>
#include <complex>
#include <cstdlib>
#include <vector>
#include <stdexcept>
#include <cassert>

#define MTL_FCALL(x) x##_
#undef minor

namespace ietl {
  template <class T>
    T* get_data(const std::vector<T>& v) {
    return const_cast<T*>(&v[0]);
  }
}
 
namespace ietl_lapack_dispatch {  

  inline void stev(const char& jobz, const int& n, float sd[],float se[],
		   float sz[],const int& ldz,int& info) {
    float* swork = new float[2*n -2];
    MTL_FCALL(sstev)(jobz,n,sd,se,sz,ldz,swork,info); 
    delete[] swork;
    if (info)
      throw std::runtime_error("Error return from sstev");
  }  

  inline void stev(const char& jobz, const int& n, double dd[],double de[],
		   double dz[],const int& ldz,int& info) {
    double* dwork = new double[2*n -2];
    MTL_FCALL(dstev)(jobz,n,dd,de,dz,ldz,dwork,info);
    delete[] dwork;
    if (info)
      throw std::runtime_error("Error return from dstev");
  }

  inline void stev(const char& jobz, const int& n, float sd[],float se[],
		   std::complex<float> sz[],const int& ldz,int& info) {
    float* swork = new float[2*n -2];
    MTL_FCALL(csteqr)(jobz,n,sd,se,sz,ldz,swork,info); 
    delete[] swork;
    if (info)
      throw std::runtime_error("Error return from csteqr");
  }  

  inline void stev(const char& jobz, const int& n, double dd[],double de[],
		   std::complex<double> dz[],const int& ldz,int& info) {
    double* dwork = new double[2*n -2];
    MTL_FCALL(zsteqr)(jobz,n,dd,de,dz,ldz,dwork,info);
    delete[] dwork;
    if (info)
      throw std::runtime_error("Error return from zsteqr");
  }
  
} // ietl2lapack_dispatch ends here.

namespace ietl2lapack {

  inline void syev(int dim, double a[], double w[], char jobz='V', char uplo='U') 
  {
    int lwork=4*dim;
    int info;
    double* work = new double[lwork];
    dsyev_(jobz, uplo, dim, a, dim, w, work, lwork, info);
    delete[] work;
    if (info)
      throw std::runtime_error("Error return from dsyev");
  }
      
  inline void syev(int dim, float a[], float w[], char jobz='V', char uplo='U')
  {
    int lwork=4*dim;
    int info;
    float* work = new float[lwork];
    ssyev_(jobz, uplo, dim, a, dim, w, work, lwork, info);
    delete[] work;
    if (info)
      throw std::runtime_error("Error return from ssyev");
  }
      
  inline void heev(int dim, std::complex<double> a[], double w[], char jobz='V', char uplo='U')
  {
    int lwork=4*dim;
    int info;
    std::complex<double>* work = new std::complex<double>[lwork];
    double* rwork = new double[lwork];
    zheev_(jobz, uplo, dim, a, dim, w, work, lwork, rwork, info);
    delete[] work;
    delete[] rwork;
    if (info)
      throw std::runtime_error("Error return from zheev");
    
  }

  inline void syev(int dim, std::complex<double> a[], double w[], char jobz='V', char uplo='U')
  {
    heev(dim,a,w,jobz,uplo);
  }
  
      
  inline void heev(int dim, std::complex<float> a[], float w[], char jobz='V', char uplo='U')
  {
    int lwork=4*dim;
    int info;
    std::complex<float>* work = new std::complex<float>[lwork];
    float* rwork = new float[lwork];
    cheev_(jobz, uplo, dim, a, dim, w, work, lwork, rwork, info);
    delete[] work;
    delete[] rwork;
    if (info)
      throw std::runtime_error("Error return from cheev");
  }

  inline void syev(int dim, std::complex<float> a[], float w[], char jobz='V', char uplo='U')
  {
    heev(dim,a,w,jobz,uplo);
  }


  template<class Vector>
    int stev(const Vector& alpha, const Vector& beta, Vector& eval, unsigned int n) {  
    if (n==0) n = alpha.size();
    std::copy(alpha.begin(),alpha.begin() + n, eval.begin()); 
    assert(eval.size() >= n);
    assert(alpha.size() >= n);
    assert(beta.size() >= n);
    Vector beta_tmp(n);
    std::copy(beta.begin(),beta.begin() + n, beta_tmp.begin()); 
    Vector z; // not referenced
    char _jobz = 'N';
    int _info;    
    int _ldz = 1; 
    ietl_lapack_dispatch::stev(_jobz, n, ietl::get_data(eval), ietl::get_data(beta_tmp), ietl::get_data(z),_ldz, _info);
    return _info;
  }
  
  template<class Vector, class FortranMatrix>
    int stev(const Vector& alpha, const Vector& beta, Vector& eval, FortranMatrix& z, unsigned int n) {  
    if (n==0) n = alpha.size();
    std::copy(alpha.begin(),alpha.begin() + n, eval.begin()); 
    assert(eval.size()>=n);
    assert(alpha.size()>=n);
    assert(beta.size()>=n); 
    Vector beta_tmp(n);
    std::copy(beta.begin(),beta.begin() + n, beta_tmp.begin()); 
    char _jobz;
    int _info;
    _jobz = 'V';
    int _ldz = z.minor(); 
    ietl_lapack_dispatch::stev(_jobz, n, ietl::get_data(eval), ietl::get_data(beta_tmp),z.data(),_ldz, _info);
    return _info;
  }
  
  
} 
#endif
