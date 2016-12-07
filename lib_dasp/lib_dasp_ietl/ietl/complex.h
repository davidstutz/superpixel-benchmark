/***************************************************************************
 * $Id: complex.h,v 1.5 2003/09/13 10:30:24 troyer Exp $
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

#ifndef IETL_COMPLEX_H
#define IETL_COMPLEX_H 

#include <complex>

namespace ietl {
  template <class T>
  struct real_type {
    typedef T type;
  };
  
  template <class T>
  struct real_type<std::complex<T> > {
    typedef T type;
  };
  
  template <class T> T real (T x) { return x;}
  template <class T> T real (std::complex<T> x) { return x.real();}
  template <class T> T conj (T x) { return x;}
  template <class T> T conj (std::complex<T> x) { return std::conj(x);}
}
#endif

