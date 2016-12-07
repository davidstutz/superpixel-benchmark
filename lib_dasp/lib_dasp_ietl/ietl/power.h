/***************************************************************************
 * $Id: power.h,v 1.12 2004/02/15 23:30:42 troyer Exp $
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

#ifndef IETL_POWER_H
#define IETL_POWER_H

#include <ietl/traits.h>

namespace ietl {
  template <class MATRIX, class GEN, class IT, class VS>
    std::pair<typename vectorspace_traits<VS>::scalar_type, typename vectorspace_traits<VS>::vector_type>
    power(const MATRIX& m, GEN& start, IT& iter, const VS& vec) {      

    typedef typename vectorspace_traits<VS>::vector_type vector_type;
    typedef typename vectorspace_traits<VS>::scalar_type scalar_type; 
    typedef typename vectorspace_traits<VS>::magnitude_type magnitude_type; 
    
    vector_type vec1 = new_vector(vec);
    ietl::generate(vec1,start);
    project(vec1,vec);
    vector_type vec2 = new_vector(vec);     
    scalar_type lambda;
    magnitude_type residual; 

    do {
      ietl::mult(m,vec1,vec2);
      lambda = ietl::dot(vec1,vec2);
      vec1 *= -lambda;
      vec1+=vec2;
      residual = ietl::two_norm(vec1); 
      vec1=(1./ietl::two_norm(vec2))*vec2;  
      ++iter;
    } while(!iter.finished(residual,lambda));    
    return std::make_pair(lambda,vec1);
  }	 
}

#endif
