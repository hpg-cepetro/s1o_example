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
#include "trace_header_adapter_mhxy.hpp"

#include <s1o/dataset.hpp>
#include <s1o/spatial_adapters/rtree_disk_slim.hpp>

namespace s1o_example {
namespace io {

namespace detail_dataset_5d {

typedef boost::geometry::index::quadratic<16> params_t;
typedef boost::geometry::cs::cartesian coord_sys_t;
typedef s1o::spatial_adapters::rtree_disk_slim<
    params_t,
    coord_sys_t
    > rtree;

}

typedef s1o::dataset<
    trace_header_adapter_mhxy,
    detail_dataset_5d::rtree
    > dataset_5d;

}}
