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

#include "trace_header.hpp"

namespace s1o_example {
namespace io {

// Create a helper struct to represent a trace as a 4D point organized
// by 2 dimensions of midpoints and 2 dimensions of half-offsets.

struct trace_header_helper_mhxy
{
    // Number of dimensions used to represent a spatial location.
    static const unsigned int num_spatial_dims = 4;

    // A name for the helper that will be included
    // in the check section of the dataset.
    static const char* get_name()
    {
        return "helper_mhxy";
    }

    // Retrieve a location from the trace using the desired criteria.
    template <typename TPoint>
    static void get_location(
        const trace_header& meta,
        TPoint& point_out
    )
    {
        double mx = (meta.RcvX + meta.SrcX) / 2.0;
        double my = (meta.RcvY + meta.SrcY) / 2.0;
        double hx = (meta.RcvX - meta.SrcX) / 2.0;
        double hy = (meta.RcvY - meta.SrcY) / 2.0;

        point_out.template set<0>(mx);
        point_out.template set<1>(my);
        point_out.template set<2>(hx);
        point_out.template set<3>(hy);
    }
};

}}
