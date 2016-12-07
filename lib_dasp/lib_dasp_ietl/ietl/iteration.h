/***************************************************************************
 * $Id: iteration.h,v 1.11 2004/06/29 09:27:48 troyer Exp $
 *
 * Copyright (C) 2001-2003 by Prakash Dayal <prakash@comp-phys.org>
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
 **************************************************************************/

#ifndef IETL_ITERATION_H
#define IETL_ITERATION_H

#include <iostream>
#include <complex>
#include <string>
#include <vector>

namespace ietl {
  template <class T>
  class basic_iteration {
    public:       
    basic_iteration(unsigned int max_iter, T reltol = 0., T abstol = 0.)
      : error(0), i(0), max_iter_(max_iter), rtol_(reltol), atol_(abstol) { }        
    bool finished(T r,T lambda) {
      if (converged(r,lambda))
	return true;
      else if (i < max_iter_)
	return false;
      else {
	fail(1,"maximum number of iterations exceeded");
	return true;
      }
    }
    
    inline bool converged(T r, T lambda) {
      return (r <= rtol_ * std::fabs(lambda) || r < atol_); // relative or absolute tolerance.
    }    
    
    inline void operator++() { ++i; }  
    inline bool first() { return i == 0; }    
    inline int error_code() { return error; }    
    inline unsigned int iterations() { return i; }    
    inline T relative_tolerance() { return rtol_; }
    inline T absolute_tolerance() { return atol_; }
    inline unsigned int max_iterations() { return max_iter_; }    
    inline void fail(int err_code) { error = err_code; }  
    inline void fail(int err_code, const std::string& msg)
      { error = err_code; err_msg = msg; }
    
    protected:
    int error;
    unsigned int i;    
    unsigned int max_iter_;
    T rtol_;
    T atol_;
    std::string err_msg;
  };
  
  template <class T, class Derived>
    class basic_lanczos_iteration {
    public:         
    basic_lanczos_iteration(unsigned int max_iter, T r = 0., T a = 0.)
      : error(0), i(0), 
      max_iter_(max_iter), rtol_(r), atol_(a) { }   
    
    template <class Tmatrix>
      bool finished(const Tmatrix& tmatrix) {
      if (static_cast<const Derived&>(*this).converged(tmatrix))
	return true;
      else if (i < max_iter_)
	return false;
      else {
	fail (1, "maximum number of iterations exceeded");
	return true;
      }
    }
    
	bool converged() const { return false;}
	void operator++() { ++i; }    
	bool first() const { return i == 0; }
	int error_code() const { return error; }  
	unsigned int iterations() const { return i; }  
    inline unsigned int max_iterations() { return max_iter_; }    
	T relative_tolerance() const { return rtol_; }
	T absolute_tolerance() const { return atol_; }   
    inline void fail(int err_code){ error = err_code; }  
    inline void fail(int err_code, const std::string& msg)
      { error = err_code; err_msg = msg; }
    
  protected:
    int error;
    unsigned int i;    
    unsigned int max_iter_;
    T rtol_;
    T atol_;
    std::string err_msg;
  };
  
  template <class T>
  class lanczos_iteration_nlowest : public basic_lanczos_iteration<T,lanczos_iteration_nlowest<T> > {
    typedef basic_lanczos_iteration<T,lanczos_iteration_nlowest<T> > super_type;
    public:         
    lanczos_iteration_nlowest(unsigned int max_iter, unsigned int n= 1, 
	    T r = 100.*std::numeric_limits<T>::epsilon(), 
	    T a = 100.*std::numeric_limits<T>::epsilon())
      : basic_lanczos_iteration<T,lanczos_iteration_nlowest<T> >(max_iter,r,a), n_(n) { }   
        
    template <class Tmatrix>
	bool converged(const Tmatrix& tmatrix) const { 
      if(super_type::iterations()>1) {
        const std::vector<T>& errs = tmatrix.errors();
        const std::vector<T>& vals = tmatrix.eigenvalues();      
        if(vals.size()<n_)
          return false;
        else { 
          for(unsigned int i = 0; i < n_; i++)
            if (errs[i] > std::max(super_type::absolute_tolerance(),super_type::relative_tolerance()*std::abs(vals[i])))
	          return false;
	      return true;
	    }
      }
      return false;
    }    
    
  private:
    unsigned int n_; 
  };
  

  template <class T>
    class lanczos_iteration_nhighest : public basic_lanczos_iteration<T,lanczos_iteration_nhighest<T> > {
    typedef basic_lanczos_iteration<T,lanczos_iteration_nhighest<T> > super_type;
    public:     
    
    lanczos_iteration_nhighest(unsigned int max_iter, unsigned int n= 1,
	    T r = 100.*std::numeric_limits<T>::epsilon(), 
	    T a = 100.*std::numeric_limits<T>::epsilon())
      : basic_lanczos_iteration<T,lanczos_iteration_nhighest<T> >(max_iter,r,a), n_(n){}
    
    template <class Tmatrix>
	bool converged(const Tmatrix& tmatrix) const {
      if(super_type::iterations()>1) { 
        const std::vector<T>& errs = tmatrix.errors();
        const std::vector<T>& vals = tmatrix.eigenvalues();
	
        if(errs.size()<n_)
          return false;
        else { 
          for(int i = 0; i < n_; i++)
            if (errs[errs.size()-i - 1] > std::max(super_type::absolute_tolerance(),
                 super_type::relative_tolerance()*std::abs(vals[vals.size()-i-1])))
	          return false;
          return true;
        }
      } 
      return false;
    }   
        
  private:
    unsigned int n_; 
  };
  
  
  template <class T>
  class fixed_lanczos_iteration : public basic_lanczos_iteration<T,fixed_lanczos_iteration<T> > {
    public:         
    fixed_lanczos_iteration(unsigned int max_iter)
      : basic_lanczos_iteration<T,fixed_lanczos_iteration<T> >(max_iter,0.,0.) { }   
        
    template <class Tmatrix>
	bool converged(const Tmatrix& ) const { return false;}    
  };


  template <class T>
class bandlanczos_iteration_nlowest {
 public:
  bandlanczos_iteration_nlowest(unsigned int max_iter,T def_tol, 
				T dep_tol,T ghost_tol,
				bool ghost_discarding,unsigned int evs)
    : max_iter_(max_iter), def_tol_(def_tol),
    dep_tol_(dep_tol), ghost_tol_(ghost_tol),
    ghost_discarding_(ghost_discarding), evs_(evs) {
    i=0;
  };
  bool finished() const {
    if ( i < max_iter_ )
      return false;
    else
      return true;
  }
  inline void operator++() { ++i; };
  inline void operator--() { --i; };
  inline bool first() { return i == 0; };
  inline unsigned int iterations() { return i; };
  inline unsigned int evs() { return evs_; };
  inline unsigned int max_iter() { return max_iter_; };
  inline T def_tol() { return def_tol_; };
  inline T dep_tol() { return dep_tol_; };
  inline T ghost_tol() { return ghost_tol_; };
  inline bool ghost_discarding() { return ghost_discarding_; };
  inline bool low() { return true; };
 private:
    unsigned int i;
    unsigned int max_iter_;
    unsigned int evs_;
    T def_tol_;
    T dep_tol_;
    T ghost_tol_;
    bool ghost_discarding_;
};

template <class T>
class bandlanczos_iteration_nhighest {
 public:
  bandlanczos_iteration_nhighest(unsigned int max_iter,T def_tol,
				 T dep_tol,T ghost_tol,
				 bool ghost_discarding, unsigned int evs)
    : max_iter_(max_iter), def_tol_(def_tol),
    dep_tol_(dep_tol), ghost_tol_(ghost_tol),
    ghost_discarding_(ghost_discarding), evs_(evs) {
    i=0;
  };
  bool finished() const {
    if ( i < max_iter_ )
      return false;
    else
      return true;
  }
  inline void operator++() { ++i; };
  inline void operator--() { --i; };
  inline bool first() { return i == 0; };
  inline unsigned int iterations() { return i; };
  inline unsigned int evs() { return evs_; };
  inline unsigned int max_iter() { return max_iter_; };
  inline T def_tol() { return def_tol_; };
  inline T dep_tol() { return dep_tol_; };
  inline T ghost_tol() { return ghost_tol_; };
  inline bool ghost_discarding() { return ghost_discarding_; };
  inline bool low() { return false; };
 private:
  unsigned int i;
  unsigned int max_iter_;
  unsigned int evs_;
  T def_tol_;
  T dep_tol_;
  T ghost_tol_;
  bool ghost_discarding_;
};

}

#endif
