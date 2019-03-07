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

#include "hpg/dataset_5d.hpp"
#include "hpg/su.hpp"

#include <boost/lexical_cast.hpp>

#include <stdexcept>
#include <iostream>
#include <vector>

#include <unistd.h>

// This program will unpack the headers and data of a single SU file
// stored inside the s1o dataset.

int main(int argc, const char* argv[])
{
    using namespace s1o_example::io;

    if (argc != 4)
    {
        std::cerr
            << "USAGE: PROGRAM s1ofile nslots slot > sufile"
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

    dataset_iterator begin = inds.begin_elements(slot);
    dataset_iterator end = inds.end_elements(slot);

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

        std::cout.write(data, inds.get_meta_adapter().
            get_data_size(header));

        // Ensure the trace was written to the output.

        std::cout.flush();
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
