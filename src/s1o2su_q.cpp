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

#include "hpg/query_to_point.hpp"
#include "hpg/query_to_range.hpp"
#include "hpg/query_parser.hpp"
#include "hpg/print_point.hpp"
#include "hpg/dataset_5d.hpp"
#include "hpg/su.hpp"

#include <s1o/traits/num_spatial_dims.hpp>

#include <boost/lexical_cast.hpp>

#include <stdexcept>
#include <iostream>
#include <iterator>
#include <vector>

#include <unistd.h>

// Copy traces to stdout from element iterators

template <typename IT, typename MA>
size_t copy_traces(
    IT begin,
    IT end,
    size_t ndelta,
    const MA& meta_adapter
)
{
    using namespace s1o_example::io;

    size_t n;

    for (n = 0; begin != end; begin++, n++)
    {
        if (((n + 1) % ndelta) == 0) {
            std::cerr
                << ".";
        }

        const trace_header& header = *begin->first;
        const char* data = begin->second;

        // Write raw trace data to stdout.

        su_dataset::write_header(header, std::cout);

        std::cout.write(data, meta_adapter. get_data_size(header));

        // Ensure the trace was written to the output.

        std::cout.flush();
    }

    return n;
}

// Copy the entire file without any query.

size_t copy_traces_no_query(
    const s1o_example::io::dataset_5d& inds,
    size_t slot,
    size_t ndelta
)
{
    using namespace s1o_example::io;

    typedef dataset_5d::elem_l_iterator_slot dataset_iterator;

    // Get the iterators to all elements in the dataset,
    // at the specific slot.

    dataset_iterator begin = inds.begin_elements(slot);
    dataset_iterator end = inds.end_elements(slot);

    return copy_traces(begin, end, ndelta, inds.get_meta_adapter());
}

// Copy the file with a range query.

size_t copy_traces_range(
    const s1o_example::io::dataset_5d& inds,
    size_t slot,
    size_t ndelta,
    const s1o_example::query::query_parser& query
)
{
    using namespace s1o_example::io;
    using namespace s1o_example::misc;
    using namespace s1o_example::query;

    typedef dataset_5d::elem_q_iterator_slot dataset_iterator;
    typedef dataset_5d::spatial_point_type point;

    // Parse and print the query points.

    point p1, p2;
    query_to_range(query, p1, p2);

    std::cerr
        << "Selecting points in range:"
        << std::endl;

    std::cerr << "  from: ";
    print_point(p1, std::cerr);
    std::cerr << "..." << std::endl;
    std::cerr << "  to:   ";
    print_point(p2, std::cerr);
    std::cerr << "..." << std::endl;

    // Get the iterators to the range query at the specific slot.

    dataset_iterator begin = inds.begin_query_elements(p1, p2, slot);
    dataset_iterator end = inds.end_query_elements(slot);

    return copy_traces(begin, end, ndelta, inds.get_meta_adapter());
}

// Copy the file with a k-nearest neighbors query.

size_t copy_traces_nearest(
    const s1o_example::io::dataset_5d& inds,
    size_t slot,
    size_t ndelta,
    const s1o_example::query::query_parser& query
)
{
    using namespace s1o_example::io;
    using namespace s1o_example::misc;
    using namespace s1o_example::query;

    typedef dataset_5d::elem_q_iterator_slot dataset_iterator;
    typedef dataset_5d::spatial_point_type point;

    // Parse the query point.

    point p;
    query_to_point(query, p);

    const query_element& elem_nearest = query.get_query_element(
        query.get_num_query_elements() - 1);

    if (elem_nearest.is_value_empty())
        throw std::runtime_error("Missing number of nearest points!");

    if (elem_nearest.num_values() != 1)
        throw std::runtime_error("Expect nearest as a single value!");

    // Parse the number of nearest neighbors to search.

    size_t nearest = elem_nearest.get_value_as<size_t>();

    // Print the query parameters.

    std::cerr
        << "Selecting "
        << nearest
        << " nearest points to:"
        << std::endl;

    std::cerr << "  point: ";
    print_point(p, std::cerr);
    std::cerr << "..." << std::endl;

    // Get the iterators to the KNN query at the specific slot.

    dataset_iterator begin = inds.begin_query_elements(p, nearest, slot);
    dataset_iterator end = inds.end_query_elements(slot);

    return copy_traces(begin, end, ndelta, inds.get_meta_adapter());
}

// Copy the trace at the exact position specified in the query.

size_t copy_traces_exact(
    const s1o_example::io::dataset_5d& inds,
    size_t slot,
    size_t ndelta,
    const s1o_example::query::query_parser& query
)
{
    using namespace s1o_example::io;
    using namespace s1o_example::misc;
    using namespace s1o_example::query;

    typedef dataset_5d::element_pair dataset_element_pair;
    typedef dataset_5d::spatial_point_type point;

    // Parse and print the query point.

    point p;
    query_to_point(query, p);

    std::cerr
        << "Selecting exactly the point:"
        << std::endl;

    std::cerr << "  at: ";
    print_point(p, std::cerr);
    std::cerr << "..." << std::endl;

    // Get the exact element at the specific slot.

    dataset_element_pair trace = inds.find_element(p, slot);

    return copy_traces(&trace, &trace, ndelta, inds.get_meta_adapter());
}

// This program will unpack the headers and data of a single SU file
// stored inside the s1o dataset.

int main(int argc, const char* argv[])
{
    using namespace s1o::traits;
    using namespace s1o_example::io;
    using namespace s1o_example::query;

    if (argc < 4)
    {
        std::cerr
            << "USAGE: PROGRAM s1ofile nslots slot [query] > sufile"
            << std::endl
            << "QUERY FORMAT:"
            << std::endl
            << "  range(R0,R1,RN)"
            << std::endl
            << "  nearest(c0,c1,cN,k)"
            << std::endl
            << "  at(c0,c1,cN)"
            << std::endl
            << "R (RANGE SPEC) FORMAT:"
            << "  ci-cf"
            << std::endl
            << "  ci-"
            << std::endl
            << "  -cf"
            << std::endl
            << "  -"
            << std::endl;
        return 1;
    }

    // Ensure binary data does not output to terminal.

    if (isatty(fileno(stdout)))
    {
        std::cerr
            << "Error: stdout must be a file or a pipe, not TTY!"
            << std::endl;
        return 1;
    }

    std::string infile(argv[1]);
    size_t slots = boost::lexical_cast<size_t>(argv[2]);
    size_t slot = boost::lexical_cast<size_t>(argv[3]);

    // Assemble the query back in case it was split by the terminal.

    std::string querystr;

    for (size_t i = 4; i < argc; i++)
        querystr += argv[i];

    query_parser query(querystr, num_spatial_dims<dataset_5d>::value);

    // Open the s1o dataset allowing unsorted data (this program is not
    // performance critical) and not performing any data checks (increase
    // memory usage).

    std::cerr
        << "Opening dataset "
        << infile
        << "..."
        << std::endl;

    dataset_5d inds(infile, 0, s1o::S1O_FLAGS_ALLOW_UNSORTED |
        s1o::S1O_FLAGS_NO_DATA_CHECK, slots);

    // Extract the selected slot.

    std::cerr
        << "Dataset open."
        << std::endl;

    // Copy the data from the s1o dataset to stdout.

    typedef dataset_5d::elem_l_iterator_slot dataset_iterator;

    // Show 1% of the progress at a time

    size_t ndelta = inds.get_max_elements() / 100;
    ndelta = ndelta != 0 ? ndelta : 1;

    std::cerr
        << "Copying traces..."
        << std::endl;

    size_t n;

    // Select the right method from the query type (if specified).

    switch(query.get_query_type())
    {
    case QUERY_TYPE_NONE:
        n = copy_traces_no_query(inds, slot, ndelta);
        break;
    case QUERY_TYPE_RANGE:
        n = copy_traces_range(inds, slot, ndelta, query);
        break;
    case QUERY_TYPE_NEAREST:
        n = copy_traces_nearest(inds, slot, ndelta, query);
        break;
    case QUERY_TYPE_EXACT:
        n = copy_traces_exact(inds, slot, ndelta, query);
        break;
    default:
        throw std::runtime_error("Unknown query type!");
    }

    std::cerr
        << std::endl;

    std::cerr
        << "Copied " << n << " traces."
        << std::endl;

    std::cerr
        << "Done."
        << std::endl;

    return 0;
}
