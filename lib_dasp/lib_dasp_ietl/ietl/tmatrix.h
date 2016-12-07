/***************************************************************************
 * $Id: tmatrix.h,v 1.12.2.1 2006/01/02 03:37:45 prakash Exp $
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

#ifndef IETL_TMATRIX_H
#define IETL_TMATRIX_H
#include <ietl/vectorspace.h>
#include <stdexcept>

namespace ietl {
  template <class VS>
    class Tmatrix {
    public:
    typedef typename vectorspace_traits<VS>::scalar_type scalar_type; 
    typedef typename vectorspace_traits<VS>::vector_type vector_type;
    typedef typename vectorspace_traits<VS>::magnitude_type magnitude_type;
    typedef typename vectorspace_traits<VS>::size_type size_type;
    
    Tmatrix() : error_tol(pow(std::numeric_limits<magnitude_type>::epsilon(),2./3.)) {}
    
    inline void push_back(magnitude_type a, magnitude_type b);
    inline void push_back(std::pair<magnitude_type,magnitude_type> a_and_b)
    { push_back(a_and_b.first,a_and_b.second);}
    
    const std::vector<magnitude_type>& eigenvalues(bool discard_ghosts=true) const {
      if(!computed) compute();
      if (discard_ghosts)
        return eigval_distinct_noghost;
      else
        return eigval_distinct;
    }
    
    const std::vector<magnitude_type>& errors(bool discard_ghosts=true) const {
      if(!computed) compute();
      if (discard_ghosts)
        return err_noghost;
      else
	return err;
    }
    
    const std::vector<int>& multiplicities(bool discard_ghosts=true) const {
      if(!computed) compute();
      if (discard_ghosts)
        return multiplicty_noghost;
      else
	return multiplicty;
    }
    
    protected:
    std::vector<magnitude_type> alpha;
    std::vector<magnitude_type> beta;
    magnitude_type error_tol;
    mutable magnitude_type thold; 
    
    private:
    mutable bool computed;
    void compute() const;
    mutable magnitude_type multol; 
    mutable std::vector<magnitude_type> err; 
    mutable std::vector<magnitude_type> err_noghost;
    mutable std::vector<magnitude_type> eigval_distinct; // distinct eigen values.
    mutable std::vector<magnitude_type> eigval_distinct_noghost; // distinct eigen values.  
    mutable std::vector<int> multiplicty; 
    mutable std::vector<int> multiplicty_noghost;
    magnitude_type alpha_max;
    magnitude_type beta_max;
    magnitude_type beta_min;     
  }; // end of class Tmatrix.
  //-----------------------------------------------------------------------

  // implementation of member functions start:
  template <class VS>
    void Tmatrix<VS>::push_back(magnitude_type a, magnitude_type b) {
    computed = false;
    alpha.push_back(a);
    beta.push_back(b);
    if(alpha.size() == 1) {
      alpha_max = a;
      beta_min = beta_max = b;
    }
    else {
      if(a > alpha_max)
      	alpha_max = a;
      if(b > beta_max) beta_max = b; 
      if(b < beta_min) beta_min = b;
    } 
  }
  
  //-----------------------------------------------------------------------
  
  template <class VS>
    void Tmatrix<VS>::compute() const {
    err.resize(0,0);
    eigval_distinct.resize(0,0);
    multiplicty.resize(0,0);
    
    err_noghost.resize(0,0);
    eigval_distinct_noghost.resize(0,0);
    multiplicty_noghost.resize(0,0);
    
    computed = true;
    int info,n;
    std::vector<magnitude_type> eval(alpha.size()); 
    // on return from stev function, eval contains the eigen values.
    n = alpha.size();
    ietl::FortranMatrix<magnitude_type> z2(n,n);
    
    info = ietl2lapack::stev(alpha, beta, eval, z2, n);
    if (info > 0)
      throw std::runtime_error("LAPACK error, stev function failed.");
    
    // tolerance values:
    multol = std::max(alpha_max,beta_max) * 2 * std::numeric_limits<magnitude_type>::epsilon() * (1000 + n); 
    thold = std::max(eval[0],eval[n-1]);
    thold = std::max(error_tol * thold, 5 * multol);
    
    // error estimates of eigen values starts:    
    // the unique eigen values selection, their multiplicities and corresponding errors calculation follows:
    
    magnitude_type temp = eval[0];
    eigval_distinct.push_back(eval[0]);
    int multiple = 1;
    
    for(int i = 1; i < n ; i++) {
      if((eval[i]- temp) > thold) {
	eigval_distinct.push_back(eval[i]);
	temp = eval[i];
	multiplicty.push_back(multiple);
	if(multiple > 1) err.push_back(0.);
	else
	  err.push_back(fabs(*beta.rbegin() * z2(n-1,i-1))); // *beta.rbegin() = betaMplusOne.
	multiple = 1;
      }
      else
	multiple++;
    }
    
    // for last eigen value.
    multiplicty.push_back(multiple);
    if(multiple > 1) err.push_back(0); 
    else
      err.push_back(fabs(*beta.rbegin() * z2(n-1,n-1))); // *beta.rbegin() = betaMplusOne.
    
    // the unique eigen values selection, their multiplicities and corresponding errors calculation ends.
    
    // ghosts calculations starts:
    std::vector<magnitude_type> beta_g(alpha.size() - 1);
    std::vector<magnitude_type> alpha_g(alpha.size() - 1);
    
    std::copy(alpha.begin() + 1, alpha.end(), alpha_g.begin());
    std::copy(beta.begin() + 1, beta.end(), beta_g.begin());
    
    std::vector<magnitude_type> eval_g(alpha_g.size()); 
    info = ietl2lapack::stev(alpha_g, beta_g, eval_g, n-1);
    if (info > 0)
      throw std::runtime_error("LAPACK error, stev function failed.");
    
    typename std::vector<magnitude_type>::iterator k;
    int i = 0, t2 = 0;
    for(k = eigval_distinct.begin(); k != eigval_distinct.end(); k++,i++) { 
      if(multiplicty[i] == 1) { // test of spuriousness for the eigenvalues whose multiplicity is one.
	for(int j = t2; j < n-1; j++,t2++) { // since size of reduced matrix is n-1
	  if((eval_g[j] - *k) >= multol) break;
	  
	  if(fabs(*k - eval_g[j]) < multol) {
	    multiplicty[i] = 0;
	    err[i] = 0; // if eigen value is a ghost => error calculation not required, 0=> ignore error.
	    t2++;
	    break;
	  }	  
	}
      }
    } // end of outer for.
    
    i = 0;
    for(k = eigval_distinct.begin(); k != eigval_distinct.end(); k++,i++) {
      if(multiplicty[i] != 0) {
	eigval_distinct_noghost.push_back(*k);
	multiplicty_noghost.push_back(multiplicty[i]);
	err_noghost.push_back(err[i]);
      }
    }
  } // end of compute.
}  //----------------------------------------------------------------------
#endif
