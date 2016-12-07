/***************************************************************************
 * $Id: blitz.h,v 1.10 2003/09/05 08:12:38 troyer Exp $
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
 *
 **************************************************************************/

#ifndef IETL_INTERFACE_BLITZ_H
#define IETL_INTERFACE_BLITZ_H

#include <ietl/complex.h>
#include <blitz/array.h>
#include <blitz/vecglobs.h>
#include <ietl/traits.h>


namespace ietl {
  
  template < class Cont, class Gen> 
    void generate(Cont& c, Gen& gen) {
    std::generate(c.begin(),c.end(),gen);
  }

  template <class T, int D>
  typename number_traits<T>::magnitude_type two_norm(const blitz::Array<T,D>& v) {
    return std::sqrt(ietl::real(dot(v,v)));
  }

  template <class T, int D>
  T dot(const blitz::Array<T,D>& x, const blitz::Array<T,D>& y) {
    return blitz::sum(x*y);
  }

  template <class T, int D>
  T dot(const blitz::Array<std::complex<T>,D>& x, const blitz::Array<std::complex<T>,D>& y) {
    
    return blitz::sum(blitz::conj(x)*y);
  }

  template <class T, int D>
  void copy(const blitz::Array<T,D>& x, blitz::Array<T,D>& y) {
    y=x;
    y.makeUnique();
  }
}

namespace std {  
  template <class T, int D>
  void swap(blitz::Array<T,D>& x, blitz::Array<T,D>& y) {
    blitz::cycleArrays(x,y);
  }

}

#endif // IETL_INTERFACE_BLITZ_H

