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

#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <limits>
#include <vector>

#include <stdint.h>

namespace s1o_example {
namespace io {

class su_dataset
{
private:

    std::string filename;
    std::fstream file;

    template <typename T>
    static T get(const char* p)
    {
        return reinterpret_cast<const T*>(p)[0];
    }

    template <typename T>
    static void set(char* p, const T& val)
    {
        reinterpret_cast<T*>(p)[0] = val;
    }

    void read_samples(unsigned int ns, std::vector<sample_t>& samples)
    {
        samples.resize(ns);

        char* buffer = reinterpret_cast<char*>(&samples[0]);

        if (!this->file.read(buffer, sizeof(sample_t) * ns))
        {
            throw std::runtime_error(
                std::string("Failed to read samples from ") +
                filename + "!");
        }
    }

    void skip_samples(unsigned int ns)
    {
        if (!this->file.seekg(sizeof(sample_t) * ns, std::ios_base::cur))
        {
            throw std::runtime_error(
                std::string("Failed to seek file ") +
                filename + "!");
        }
    }

    bool read_header(trace_header& header)
    {
        char data[su_header_size];

        // Use the position in the file as the id of the trace.
        uint64_t id = this->file.tellg();

        if (!this->file.read(data, su_header_size))
            return false;

        // Process the data in the header.

        int cdp = get<int>(data + 20);

        double offset = static_cast<double>(get<int>(data + 36));

        double scaling = static_cast<double>(get<short>(data + 70));

        if (scaling < 0)
            scaling = -1.0 / scaling;

        double srcx = static_cast<double>(get<int>(data + 72)) * scaling;
        double srcy = static_cast<double>(get<int>(data + 76)) * scaling;
        double rcvx = static_cast<double>(get<int>(data + 80)) * scaling;
        double rcvy = static_cast<double>(get<int>(data + 84)) * scaling;

        double delrt = static_cast<double>(get<unsigned short>(data + 108)) / 1.0e3;
        double dt = static_cast<double>(get<unsigned short>(data + 116)) / 1.0e6;

        unsigned int ns = get<unsigned short>(data + 114);

        // Use the factory created by f1d to ensure no field is left unset.

        trace_header_factory thf;
        thf.begin();
        thf.set_Id(id);
        thf.set_CDP(cdp);
        thf.set_Offset(offset);
        thf.set_SrcX(srcx);
        thf.set_SrcY(srcy);
        thf.set_RcvX(rcvx);
        thf.set_RcvY(rcvy);
        thf.set_Delrt(delrt);
        thf.set_Ns(ns);
        thf.set_Dt(dt);
        thf.end();

        header = thf.get();

        return true;
    }

public:

    static const unsigned int su_header_size = 240;

    su_dataset(const std::string& filename) :
        filename(filename), file()
    {
        this->file.open(filename.c_str(), std::fstream::in |
                std::fstream::binary);

        if (!this->file.is_open()) {
            throw std::runtime_error("Failed to open " + filename + "!");
        }
    }

    ~su_dataset()
    {
        file.close();
    }

    bool read_trace_header(trace_header& header)
    {
        if (!read_header(header))
            return false;

        skip_samples(header.Ns);

        return true;
    }

    bool read_trace(trace_header& header, std::vector<sample_t>& samples)
    {
        if (!read_header(header))
            return false;

        read_samples(header.Ns, samples);

        return true;
    }

    static void write_header(const trace_header& header, std::ostream& stream)
    {
        char data[su_header_size];

        std::fill(data, data + su_header_size, 0);

        // Convert the data in the header back to binary.

        set<int>(data + 20, header.CDP);

        set<int>(data + 36, static_cast<int>(header.Offset));

        short scalco = 1;

        while (true)
        {
            double maxp = std::max(std::max(std::max(header.SrcX,
                header.SrcY), header.RcvX), header.RcvY);

            const double limp = static_cast<double>(
                std::numeric_limits<int>::max());

            if (maxp < limp)
                break;

            int iscalco = scalco * 10;

            const int lims = std::numeric_limits<short>::max();

            if (iscalco > lims)
                throw std::runtime_error("Coordinate scaling error!");

            scalco = static_cast<short>(iscalco);
        }

        int srcx = static_cast<int>(header.SrcX / scalco);
        int srcy = static_cast<int>(header.SrcY / scalco);
        int rcvx = static_cast<int>(header.RcvX / scalco);
        int rcvy = static_cast<int>(header.RcvY / scalco);

        set<short>(data + 70, scalco);

        set<int>(data + 72, srcx);
        set<int>(data + 76, srcy);
        set<int>(data + 80, rcvx);
        set<int>(data + 84, rcvy);

        set<unsigned short>(data + 108, static_cast<unsigned short>(header.Delrt * 1.0e3));
        set<unsigned short>(data + 116, static_cast<unsigned short>(header.Dt * 1.0e6));

        set<unsigned short>(data + 114, static_cast<unsigned short>(header.Ns));

        stream.write(data, su_header_size);
    }
};

}}