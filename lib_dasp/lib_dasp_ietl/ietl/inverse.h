/***************************************************************************
 * $Id: inverse.h,v 1.9 2004/02/15 23:30:42 troyer Exp $
 *
 * Copyright (C) 2001-2002 by Rene Villiger <rvilliger@smile.ch>
 *                            Prakash Dayal <prakash@comp-phys.org>
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

#ifndef IETL_INVERSE_H
#define IETL_INVERSE_H

#include <ietl/traits.h>
#include <ietl/complex.h>

// REQUIRES class SOLVER with interface
//
//    void operator()(MATRIX matrix, MAGNITUDE_TYPE sigma, VECTOR_TYPE v, VECTOR_TYPE y)
// 
// Solves the equation
//    y = (A - sigma*I)^{-1} * v
//

namespace ietl
{
   template <class MATRIX, class GEN, class SOLVER, class ITER, class VS>
   std::pair<typename vectorspace_traits<VS>::magnitude_type,
             typename vectorspace_traits<VS>::vector_type>
      inverse(const MATRIX& matrix, GEN& gen,const SOLVER& solver, ITER& iter,
			  typename vectorspace_traits<VS>::magnitude_type sigma,
              const VS& vec)
      {
         typedef typename vectorspace_traits<VS>::vector_type vector_type;
         typedef typename vectorspace_traits<VS>::scalar_type scalar_type;
         typedef typename vectorspace_traits<VS>::magnitude_type magnitude_type;
         
         vector_type y   = new_vector(vec);
         vector_type v   = new_vector(vec);
         magnitude_type theta;
         magnitude_type residual;

         // Start with vector y=z, the initial guess
         ietl::generate(y,gen);
         ietl::project(y,vec);
         
         // start iteration loop
         do {
            v=(1./ietl::two_norm(y)) * y;
            try {
			  solver(matrix, sigma, v, y);  // y = (A-\sigma*I)^{-1} v
            }
            catch (...) {
              break; // done with iteration
            }  
			
            theta = ietl::real(ietl::dot(v,y));
            v = y-theta*v;  // residual = | y - \theta*v |_2
            residual = ietl::two_norm(v);
            ++iter;
            
         // check for convergence
         } while(!iter.finished(residual, theta));

         // accept \lambda = \sigma + 1/\theta  and  x = y/\theta 
         y/=theta;
         theta = sigma + 1./theta;
         
         return std::make_pair(theta, y);
      }
}

#endif
