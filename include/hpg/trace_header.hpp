/*
 * Copyright (C) 2019 Caian Benedicto <caianbene@gmail.com>
 *
 * This file is part of s1o_example.
 *
 * s1o_example is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * s1o_example is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Make; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#pragma once

#include <f1d/fields.hpp>

#include <stdint.h>

namespace s1o_example {
namespace io {

// Create a simple struct to store the essential
// fields of the trace header.

F1D_TRAITS_MAKE()
F1D_STRUCT_MAKE_NT(trace_header,
    10, (
        (Id    , uint64_t ), // required by s1o
        (CDP   , int32_t  ),
        (Offset, double   ),
        (SrcX  , double   ),
        (SrcY  , double   ),
        (RcvX  , double   ),
        (RcvY  , double   ),
        (Delrt , double   ),
        (Ns    , uint32_t ),
        (Dt    , double   )
    )
) // trace_header

// The type of the seismic sample

typedef float sample_t;

}}
