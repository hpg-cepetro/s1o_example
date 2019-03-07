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

#include "query_parser.hpp"

#include <boost/geometry/geometries/point.hpp>

namespace s1o_example {
namespace query {

namespace detail {

template <typename T, unsigned int I>
struct query_to_point_impl
{
    static const unsigned int N = boost::geometry::
        traits::dimension<T>::value;
    static const unsigned int J = N-I;

    typedef typename boost::geometry::traits::
        coordinate_type<T>::type value_type;

    typedef query_to_point_impl<T, I-1> Next;

    static void call(const query_parser& query, T& out)
    {
        const query_element& elem = query.get_query_element(J);

        if (elem.num_values() != 1)
        {
            throw std::runtime_error(
                "Expected coordinate as a single value!");
        }

        if (elem.is_value_empty(0))
            throw std::runtime_error("Coordinate value is empty!");

        out.template set<J>(elem.get_value_as<value_type>(0));

        Next::call(query, out);
    }
};

template <typename T>
struct query_to_point_impl<T, 0>
{
    static void call(const query_parser& query, T& out)
    {
        (void)query;
        (void)out;
    }
};

}

template <typename T>
void query_to_point(const query_parser& query, T& out)
{
    detail::query_to_point_impl<
        T,
        boost::geometry::traits::dimension<T>::value
        >::call(query, out);
}

template <typename T>
T query_to_point(const query_parser& query)
{
    T out;
    query_to_point(query, out);
    return out;
}

}}
