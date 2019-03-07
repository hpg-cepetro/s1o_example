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

#include <limits>

namespace s1o_example {
namespace misc {

namespace detail {

template <typename T>
struct custom_limits_f
{
    static T highest()
    {
        return std::numeric_limits<T>::max();
    }

    static T lowest()
    {
        return -std::numeric_limits<T>::max();
    }
};

}

template <typename T>
struct custom_limits
{
    static T highest()
    {
        return std::numeric_limits<T>::max();
    }

    static T lowest()
    {
        return std::numeric_limits<T>::min();
    }
};

template <>
struct custom_limits<float> :
    detail::custom_limits_f<float>
{
};

template <>
struct custom_limits<double> :
    detail::custom_limits_f<double>
{
};

}}
