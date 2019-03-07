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

#include "custom_limits.hpp"
#include "query_parser.hpp"

#include <boost/geometry/geometries/point.hpp>

#include <algorithm>

namespace s1o_example {
namespace query {

namespace detail {

template <typename T, unsigned int I>
struct query_to_range_impl
{
    static const unsigned int N = boost::geometry::
        traits::dimension<T>::value;
    static const unsigned int J = N-I;

    typedef typename boost::geometry::traits::
        coordinate_type<T>::type value_type;

    typedef query_to_range_impl<T, I-1> Next;

    static void call(const query_parser& query, T& out_min, T& out_max)
    {
        typedef misc::custom_limits<value_type> nl;

        const value_type min_value = nl::lowest();
        const value_type max_value = nl::highest();

        const query_element& elem = query.get_query_element(J);

        if (elem.num_values() != 2)
        {
            throw std::runtime_error(
                "Expected coordinate range as two values!");
        }

        value_type first = elem.is_value_empty(0) ? min_value :
            elem.get_value_as<value_type>(0);

        value_type second = elem.is_value_empty(1) ? max_value :
            elem.get_value_as<value_type>(1);

        out_min.template set<J>(std::min(first, second));
        out_max.template set<J>(std::max(first, second));

        Next::call(query, out_min, out_max);
    }
};

template <typename T>
struct query_to_range_impl<T, 0>
{
    static void call(const query_parser& query, T& out_min, T& out_max)
    {
        (void)query;
        (void)out_min;
        (void)out_max;
    }
};

}

template <typename T>
void query_to_range(const query_parser& query, T& out_min, T& out_max)
{
    detail::query_to_range_impl<
        T,
        boost::geometry::traits::dimension<T>::value
        >::call(query, out_min, out_max);
}

template <typename T>
std::pair<T, T> query_to_range(const query_parser& query)
{
    T out_min, out_max;
    query_to_range(query, out_min, out_max);
    return std::make_pair<out_min, out_max>;
}

}}
