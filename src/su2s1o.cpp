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
#include <algorithm>
#include <iostream>
#include <vector>

#include <stdint.h>

// Ensure two trace headers are equal.

void assert_same_header(
    const s1o_example::io::trace_header& a,
    const s1o_example::io::trace_header& b
);

// This program will pack several SU files into a single s1o dataset as
// long as they have the same headers in the same order.

int main(int argc, const char* argv[])
{
    using namespace s1o_example::io;

    if (argc < 3)
    {
        std::cerr
            << "USAGE: PROGRAM sufile0 [sufile1] ... [sufileN] s1ofile"
            << std::endl;
        return 1;
    }

    std::vector<trace_header> headers;
    std::vector<uint64_t> fofs;

    // Load the headers from the first SU file and convert them to the
    // trace_header format. These will be the reference headers.

    {
        std::string infile(argv[1]);

        std::cerr
            << "Reading trace headers from "
            << infile
            << "..."
            << std::endl;

        su_dataset inds(infile);

        trace_header header;
        uint64_t id; // s1o objects must have an id sequenced from 1 to N.

        for (id = 1; inds.read_trace_header(header); id++)
        {
            // Keep the original file offset.

            fofs.push_back(header.Id);

            // Change the id of the trace header to be a sequential
            // value starting in 1 and store the header.

            header.Id = id;
            headers.push_back(header);
        }

        if (headers.size() == 0)
            throw std::runtime_error("The input file has no headers!");

        std::cerr
            << "Read "
            << headers.size()
            << " headers."
            << std::endl;
    }

    // Create the s1o dataset from the sequence of headers. This will already
    // set the ordering of the data inside the data file to follow the
    // ordering of the headers in the search tree.

    std::string outfile = argv[argc - 1];
    size_t slots = argc - 2;

    std::cerr
        << "Initializing output dataset "
        << outfile
        << " with "
        << slots
        << " slots..."
        << std::endl;

    dataset_5d outds(outfile, 0, slots, headers.begin(), headers.end());

    // Ensure everything was written to the file.

    outds.sync_metadata();

    std::cerr
        << "Output dataset initialized."
        << std::endl;

    // Copy the data from the SU files to the new dataset.

    std::vector<sample_t> insamples;

    typedef dataset_5d::element_pair dataset_element_pair;

    // Show 1% of the progress at a time

    size_t ndelta = headers.size() / 100;
    ndelta = ndelta != 0 ? ndelta : 1;

    std::cerr
        << "Copying traces..."
        << std::endl;

    size_t n = 0;

    for (size_t slot = 0; slot < slots; slot++)
    {
        size_t arg = slot + 1;
        std::string infile = argv[arg];

        std::cerr
            << infile;

        su_dataset inds(infile);

        s1o::uid_t uid;
        trace_header inheader;
        trace_header* p_outheader;
        char* p_outdata;

        for (uid = 1; inds.read_trace(inheader, insamples); uid++, n++)
        {
            if ((uid % ndelta) == 0)
            {
                std::cerr
                    << ".";
            }

            outds.get_element(uid, slot, p_outheader, p_outdata);

            // Headers must match.

            assert_same_header(inheader, *p_outheader);

            // Copy the samples.

            sample_t* outsamples = reinterpret_cast<sample_t*>(p_outdata);

            std::copy(insamples.begin(), insamples.end(), outsamples);
        }

        std::cerr
            << std::endl;

        // The number of traces must match.

        if (uid-1 != headers.size())
        {
            throw std::runtime_error(
                std::string("The number of traces in file ") +
                infile + " differ from dataset: " +
                boost::lexical_cast<std::string>(uid-1) + " vs " +
                boost::lexical_cast<std::string>(headers.size()) + "!");
        }
    }

    // Ensure everything was written to the file.

    std::cerr
        << "Synchronizing dataset..."
        << std::endl;

    outds.sync_data();

    std::cerr
        << "Copied " << n << " traces."
        << std::endl;

    std::cerr
        << "Done."
        << std::endl;

    return 0;
}

// Ensure two trace headers are equal.

void assert_same_header(
    const s1o_example::io::trace_header& a,
    const s1o_example::io::trace_header& b
)
{
    if (a.CDP != b.CDP)
    {
        throw std::runtime_error(
            std::string("The field CDP of the trace ") +
            boost::lexical_cast<std::string>(a.Id) + " at byte " +
            boost::lexical_cast<std::string>(b.Id) + " differ: " +
            boost::lexical_cast<std::string>(a.CDP) + " vs " +
            boost::lexical_cast<std::string>(b.CDP) + "!");
    }

    if (a.Offset != b.Offset)
    {
        throw std::runtime_error(
            std::string("The field Offset of the trace ") +
            boost::lexical_cast<std::string>(a.Id) + " at byte " +
            boost::lexical_cast<std::string>(b.Id) + " differ: " +
            boost::lexical_cast<std::string>(a.Offset) + " vs " +
            boost::lexical_cast<std::string>(b.Offset) + "!");
    }

    if (a.SrcX != b.SrcX)
    {
        throw std::runtime_error(
            std::string("The field SrcX of the trace ") +
            boost::lexical_cast<std::string>(a.Id) + " at byte " +
            boost::lexical_cast<std::string>(b.Id) + " differ: " +
            boost::lexical_cast<std::string>(a.SrcX) + " vs " +
            boost::lexical_cast<std::string>(b.SrcX) + "!");
    }

    if (a.SrcY != b.SrcY)
    {
        throw std::runtime_error(
            std::string("The field SrcY of the trace ") +
            boost::lexical_cast<std::string>(a.Id) + " at byte " +
            boost::lexical_cast<std::string>(b.Id) + " differ: " +
            boost::lexical_cast<std::string>(a.SrcY) + " vs " +
            boost::lexical_cast<std::string>(b.SrcY) + "!");
    }

    if (a.RcvX != b.RcvX)
    {
        throw std::runtime_error(
            std::string("The field RcvX of the trace ") +
            boost::lexical_cast<std::string>(a.Id) + " at byte " +
            boost::lexical_cast<std::string>(b.Id) + " differ: " +
            boost::lexical_cast<std::string>(a.RcvX) + " vs " +
            boost::lexical_cast<std::string>(b.RcvX) + "!");
    }

    if (a.RcvY != b.RcvY)
    {
        throw std::runtime_error(
            std::string("The field RcvY of the trace ") +
            boost::lexical_cast<std::string>(a.Id) + " at byte " +
            boost::lexical_cast<std::string>(b.Id) + " differ: " +
            boost::lexical_cast<std::string>(a.RcvY) + " vs " +
            boost::lexical_cast<std::string>(b.RcvY) + "!");
    }

    if (a.Delrt != b.Delrt)
    {
        throw std::runtime_error(
            std::string("The field Delrt of the trace ") +
            boost::lexical_cast<std::string>(a.Id) + " at byte " +
            boost::lexical_cast<std::string>(b.Id) + " differ: " +
            boost::lexical_cast<std::string>(a.Delrt) + " vs " +
            boost::lexical_cast<std::string>(b.Delrt) + "!");
    }

    if (a.Ns != b.Ns)
    {
        throw std::runtime_error(
            std::string("The field Ns of the trace ") +
            boost::lexical_cast<std::string>(a.Id) + " at byte " +
            boost::lexical_cast<std::string>(b.Id) + " differ: " +
            boost::lexical_cast<std::string>(a.Ns) + " vs " +
            boost::lexical_cast<std::string>(b.Ns) + "!");
    }

    if (a.Dt != b.Dt)
    {
        throw std::runtime_error(
            std::string("The field Dt of the trace ") +
            boost::lexical_cast<std::string>(a.Id) + " at byte " +
            boost::lexical_cast<std::string>(b.Id) + " differ: " +
            boost::lexical_cast<std::string>(a.Dt) + " vs " +
            boost::lexical_cast<std::string>(b.Dt) + "!");
    }
}
