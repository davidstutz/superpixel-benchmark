/***************************************************************************
 * $Id: ietl2lapack_interface.h,v 1.5 2003/09/05 08:12:38 troyer Exp $
 *
 * Copyright (C) 2001-2003 by Prakash Dayal <prakash@comp-phys.org>
 *                            Matthias Troyer <troyer@comp-phys.org>
 *                            Rene Villiger <rvilliger@smile.ch>
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

/**************************************************************************
 * This header file is included in ietl2lapack.h file. The functions
 * in this file are external and call the LAPACK subroutines for 
 * implementation.
 **************************************************************************/

#ifndef IETL_LAPACK_INTERFACE_H
#define IETL_LAPACK_INTERFACE_H
#include <complex>

extern "C" {
void sstev_(const char& jobz, const int& n, float sd[],
	    float se[], float sz[], const int& ldz, 
	    float swork[], int& info);

void dstev_(const char& jobz, const int& n, double dd[],
	    double de[], double dz[], const int& ldz, 
	    double swork[], int& info);
  
void csteqr_(const char& jobz, const int& n, float sd[],
	    float se[], std::complex<float> sz[], const int& ldz, 
	    float swork[], int& info);

void zsteqr_(const char& jobz, const int& n, double dd[],
	    double de[], std::complex<double> dz[], const int& ldz, 
	    double dwork[], int& info);  

void dsyevx_(const char& jobz,            const char& range,         const char& uplo,
                             const int& n,                double da[],               const int& lda,
                             const double& vl,            const double& vu,          const int& il,
                             const int& iu,               const double& abstol,      int& m,
                             double w[],                  double z[],                const int& ldz,
                             double work[],               const int& lwork,          int iwork[],
                             int ifail[],                 int& info);                                             

void ssyevx_(const char& jobz,            const char& range,         const char& uplo,
                             const int& n,                float da[],                const int& lda,
                             const float& vl,             const float& vu,           const int& il,
                             const int& iu,               const float& abstol,       int& m,
                             float w[],                   float z[],                 const int& ldz,
                             float work[],                const int& lwork,          int iwork[],
                             int ifail[],                 int& info);                                             
                             
void zheevx_(const char& jobz,            const char& range,         const char& uplo,
                             const int& n,                std::complex<double> da[], const int& lda,
                             const double& vl,            const double& vu,          const int& il,
                             const int& iu,               const double& abstol,      int& m,
                             double w[],                  std::complex<double> z[],  const int& ldz,
                             std::complex<double> work[], const int& lwork,          double rwork[],
                             int iwork[],                 int ifail[],               int& info);                  

void cheevx_(const char& jobz,            const char& range,         const char& uplo,
                             const int& n,                std::complex<float> da[],  const int& lda,
                             const float& vl,             const float& vu,           const int& il,
                             const int& iu,               const float& abstol,       int& m,
                             float w[],                   std::complex<float> z[],   const int& ldz,
                             std::complex<float> work[],  const int& lwork,          float rwork[],
                             int iwork[],                 int ifail[],               int& info);                  
                             
void dsysv_(const char& uplo,             const int& n,              const int& nrhs,
                            double a[],                   const int& lda,            int ipiv[],
                            double b[],                   const int& ldb,            double work[],
                            const int& lwork,             int& info);                                             
                            
void ssysv_(const char& uplo,             const int& n,              const int& nrhs,
                            float a[],                    const int& lda,            int ipiv[],
                            float b[],                    const int& ldb,            float work[],
                            const int& lwork,             int& info);                                             
                        
void chesv_(const char& uplo,             const int& n,              const int& nrhs,
                            std::complex<float> a[],      const int& lda,            int ipiv[],
                            std::complex<float> b[],      const int& ldb,            std::complex<float> work[],
                            const int& lwork,             int& info);                                             
                            
void zhesv_(const char& uplo,             const int& n,              const int& nrhs,
                            std::complex<double> a[],     const int& lda,            int ipiv[],
                            std::complex<double> b[],     const int& ldb,            std::complex<double> work[],
                            const int& lwork,             int& info);                                             

void dsyev_(const char& jobz,            const char& uplo, const int& n,
                            double da[],                 const int& lda,   double w[],
                            double work[],               const int& lwork, int& info);                 

void ssyev_(const char& jobz,            const char& uplo, const int& n,
                            float da[],                  const int& lda,   float w[],
                            float work[],                const int& lwork, int& info);                  

void zheev_(const char& jobz,            const char& uplo, const int& n,
                            std::complex<double> da[],   const int& lda,   double w[],
                            std::complex<double> work[], const int& lwork, double rwork[], int& info); 

void cheev_(const char& jobz,            const char& uplo, const int& n,
                            std::complex<float> da[],    const int& lda,   float w[],
                            std::complex<float> work[],  const int& lwork, float rwork[],  int& info);

}
#endif
