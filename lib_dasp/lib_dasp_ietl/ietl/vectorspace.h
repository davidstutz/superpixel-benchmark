/***************************************************************************
 * $Id: vectorspace.h,v 1.13 2004/06/29 08:31:02 troyer Exp $
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

#ifndef IETL_VECTORSPACE__H
#define IETL_VECTORSPACE__H
#include <ietl/traits.h>
#include <boost/smart_ptr.hpp>

namespace ietl {
  template<class V>
    class vectorspace {
    public:
    typedef V vector_type;
    typedef typename V::value_type scalar_type;
    typedef typename V::size_type size_type;
    
    vectorspace(size_type n):n_(n){}
    
    inline size_type vec_dimension() const {
      return n_;
    }
    vector_type new_vector() const {
      return vector_type(n_);
    }
    
    void project(vector_type&) const {
    }
    
    private:
    size_type n_;
  };
  
  template <class V, class S> class scaled_vector_wrapper;
  
  template <class V>
    class vector_wrapper : public boost::shared_ptr<V> {
    typedef boost::shared_ptr<V> super_type;
    public:
    vector_wrapper(V* p) : boost::shared_ptr<V>(p) {}
    operator V& () { return *super_type::get();}
    operator const V& () const { return *super_type::get();}
    const vector_wrapper operator += (const vector_wrapper& x) { *super_type::get() += *x.get(); return *this;}
    const vector_wrapper operator -= (const vector_wrapper& x) { *super_type::get() -= *x.get(); return *this;}
    template <class T> const vector_wrapper& operator *= (T x) { *super_type::get() *= x; return *this;}
    template <class T> const vector_wrapper& operator /= (T x) { *super_type::get() /= x; return *this;}
    template <class S>
    const vector_wrapper& operator += (const scaled_vector_wrapper<V,S>& x) 
    { *super_type::get() += x.scalar()*x.vector(); return *this;}
    template <class S>
    const vector_wrapper& operator -= (const scaled_vector_wrapper<V,S>& x) 
    { *super_type::get() -= x.scalar()*x.vector(); return *this;}
    template <class S>
    const vector_wrapper& operator = (const scaled_vector_wrapper<V,S>& x) 
    { *super_type::get() = x.scalar()*x.vector(); return *this;}
      };
  
  template<class VS>
    void project(typename ietl::vectorspace_traits<VS>::vector_type& v, const VS& vs) {
    vs.project(v);
  }
  
  template<class V>
    class wrapper_vectorspace {
    public:
    typedef vector_wrapper<V> vector_type;
    typedef typename V::value_type scalar_type;
    typedef typename V::size_type size_type;
    
    wrapper_vectorspace(size_type n):n_(n){}
    
    inline size_type vec_dimension() const{
      return n_;
    }
    vector_type new_vector() const {
      return vector_wrapper<V>(new V(n_));
    }
    
    void project(vector_type& src) const {
    }  
    private:
    size_type n_;
  };
  
template <class VS>
  typename ietl::vectorspace_traits<VS>::vector_type new_vector(const VS& vs) {
  return vs.new_vector();
}

 template <class VS>
   typename ietl::vectorspace_traits<VS>::size_type vec_dimension(const VS& vs) {
   return vs.vec_dimension();
 } 

 template <class V, class VS>
   void project(ietl::vector_wrapper<V>& v, const VS& vs) {
   vs.project(v);
 }
 
 template <class V, class S>
   class scaled_vector_wrapper {
   public:
   scaled_vector_wrapper(const ietl::vector_wrapper<V>& v, S s)
     : v_(v), s_(s)
     {}
   
   const V& vector() const { return *v_.get();}
   S scalar() const { return s_;}
   private:
   const ietl::vector_wrapper<V>& v_;
   S s_;
 }; 
} // matches namespace ietl

// wrapper forwarders

namespace ietl {
  template <class V>
    void copy(const ietl::vector_wrapper<V>& src, ietl::vector_wrapper<V>& dst) {
    ietl::copy(*src.get(),*dst.get());
  } 
  

  template <class V>
    typename V::value_type two_norm(const ietl::vector_wrapper<V>& src) {
    return ietl::two_norm(*src.get());
  }
  
 template <class V>
   typename V::value_type dot(const ietl::vector_wrapper<V>& src1, const ietl::vector_wrapper<V>& src2) {
   return ietl::dot(*src1.get(),*src2.get());
 }
 
 template <class A, class V>
   void mult(A a, const ietl::vector_wrapper<V>& src, ietl::vector_wrapper<V>& dst) {
   ietl::mult(a,*src.get(),*dst.get());
 } 

  template <class V, class GEN>
   void generate(ietl::vector_wrapper<V>& src, GEN& gen) {
   ietl::generate(*src.get(),gen);
 }
 
 template <class V, class S>
   ietl::scaled_vector_wrapper<V,S> operator*(const ietl::vector_wrapper<V>& v, S s) {
   return ietl::scaled_vector_wrapper<V,S>(v,s);
 }

 template <class V, class S>
   ietl::scaled_vector_wrapper<V,S> operator*(S s, const ietl::vector_wrapper<V>& v) {
   return ietl::scaled_vector_wrapper<V,S>(v,s);
 }

 template <class V, class S>
   ietl::scaled_vector_wrapper<V,S> operator/(const ietl::vector_wrapper<V>& v, S s) {
   return ietl::scaled_vector_wrapper<V,S>(v,1./s);
 }
 
} // end of namespace ietl.
#endif
