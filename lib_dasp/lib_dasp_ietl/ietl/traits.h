/***************************************************************************
 * $Id: traits.h,v 1.2 2004/02/15 23:30:42 troyer Exp $
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

#ifndef IETL_VECTORSPACETRAITS__H
#define IETL_VECTORSPACETRAITS__H

#include <complex>

namespace ietl {
  template <class T> 
    struct number_traits {
      typedef T magnitude_type;
    };
    
  template <class T>
    struct number_traits<std::complex<T> > {
      typedef T magnitude_type;
    };

  template <class VS>
    struct vectorspace_traits {
      typedef typename VS::vector_type vector_type;
      typedef typename VS::size_type size_type;
      typedef typename VS::scalar_type scalar_type;
	  typedef typename number_traits<scalar_type>::magnitude_type magnitude_type;
    };
    
}
#endif
