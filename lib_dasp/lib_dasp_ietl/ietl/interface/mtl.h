/***************************************************************************
 * $Id: mtl.h,v 1.10 2004/02/11 08:10:09 troyer Exp $
 *
 * Copyright (C) 2001-2004 by Prakash Dayal <prakash@comp-phys.org>
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

#ifndef IETL_MTL_H
#define IETL_MTL_H

#error The MTL-2 interface is not yet implemented

namespace ietl {
  // generate function.     
  template < class Cont, class Gen> 
    void generate(Cont& c, Gen& gen) {
    std::generate(c.begin(),c.end(),gen);
  }  

}
#endif
