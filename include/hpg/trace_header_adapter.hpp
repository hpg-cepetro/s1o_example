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

#include <s1o/types.hpp>
#include <s1o/metadata.hpp>

namespace s1o_example {
namespace io {

// Create the struct that adapts the trace_header to be used with s1o.
// This includes defining how the data will be organized in the file.

template <typename helper>
struct trace_header_adapter
{
    // Number of dimensions used to represent a spatial location.
    static const unsigned int num_spatial_dims = helper::num_spatial_dims;

    typedef float spatial_value_type;
    typedef trace_header metadata_type;

    const std::string check;
    const std::string meta_ext;
    const std::string data_ext;

    // Retrieve a location from the trace using the desired criteria.
    template <typename TPoint>
    void get_location(
        const metadata_type& meta,
        TPoint& point_out
    ) const
    {
        helper::get_location(meta, point_out);
    }

    s1o::uid_t get_uid(
        const metadata_type& meta
    ) const
    {
        return static_cast<s1o::uid_t>(meta.Id);
    }

    void set_uid(
        metadata_type& meta,
        const s1o::uid_t& uid
    ) const
    {
        meta.Id = static_cast<int>(uid);
    }

    size_t get_data_size(
        const metadata_type& meta
    ) const
    {
        return static_cast<size_t>(meta.Ns) * sizeof(sample_t);
    }

    s1o::c_meta_check_ptr_t get_meta_check_ptr() const
    {
        return check.c_str();
    }

    size_t get_meta_check_size() const
    {
        return check.size() + 1;
    }

    const char* get_meta_file_ext() const
    {
        return meta_ext.c_str();
    }

    const char* get_data_file_ext() const
    {
        return data_ext.c_str();
    }

    static std::string build_check()
    {
        std::stringstream ss;

        ss << helper::get_name();

        for (size_t i = 0; i < metadata_type::get_num_fields(); i++) {
            ss << "/";
            ss << metadata_type::get_field_name(i);
            ss << "@";
            ss << metadata_type::get_type_name(i);
        }

        return ss.str();
    }

    trace_header_adapter() :
        check(build_check()),
        meta_ext("meta"),
        data_ext("data")
    {
    }
};

}}
