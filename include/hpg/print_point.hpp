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

#include <boost/geometry/geometries/point.hpp>

#include <string>

namespace s1o_example {
namespace misc {

namespace detail {

template <typename T, unsigned int I>
struct print_point_impl
{
    static const unsigned int N = boost::geometry::
        traits::dimension<T>::value;
    static const unsigned int J = N-I;

    typedef typename boost::geometry::traits::
        coordinate_type<T>::type value_type;

    typedef print_point_impl<T, I-1> Next;

    template <typename S>
    static void call(
        const T& point,
        S& stream,
        const std::string& pre,
        const std::string& sep,
        const std::string& post
    )
    {
        stream
            << (J == 0 ? pre : sep)
            << point.template get<J>();

        Next::call(point, stream, pre, sep, post);
    }
};

template <typename T>
struct print_point_impl<T, 0>
{
    template <typename S>
    static void call(
        const T& point,
        S& stream,
        const std::string& pre,
        const std::string& sep,
        const std::string& post
    )
    {
        (void)pre;
        (void)sep;

        stream << post;
    }
};

}

template <typename T, typename S>
void print_point(
    const T& point,
    S& stream,
    const std::string& pre="(",
    const std::string& sep=", ",
    const std::string& post=")"
)
{
    detail::print_point_impl<
        T,
        boost::geometry::traits::dimension<T>::value
        >::call(point, stream, pre, sep, post);
}

}}
