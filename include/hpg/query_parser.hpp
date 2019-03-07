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

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <stdexcept>
#include <algorithm>
#include <string>
#include <vector>

namespace s1o_example {
namespace query {

enum query_type
{
    QUERY_TYPE_NONE,
    QUERY_TYPE_RANGE,
    QUERY_TYPE_NEAREST,
    QUERY_TYPE_EXACT,
};

class query_element
{
private:

    std::vector<std::string> _values;

public:

    query_element(
        const std::vector<std::string>& values
    ) :
        _values(values)
    {
    }

    size_t num_values() const
    {
        return _values.size();
    }

    bool is_value_empty(size_t i=0) const
    {
        if (i >= num_values())
            throw std::runtime_error("Sub-element index out of range!");

        return _values[i].size() == 0;
    }

    template <typename T>
    T get_value_as(size_t i=0) const
    {
        if (i >= num_values())
            throw std::runtime_error("Sub-element index out of range!");

        return boost::lexical_cast<T>(_values[i]);
    }
};

class query_parser
{
public:

    struct configuration
    {
        std::string field_sep_tokens;
        std::string range_sep_tokens;
        std::string range_token;
        std::string nearest_token;
        std::string exact_token;

        configuration() :
            field_sep_tokens(","),
            range_sep_tokens(":"),
            range_token("range"),
            nearest_token("nearest"),
            exact_token("at")
        {
        }
    };

private:

    typedef std::vector<std::string> tokens_t;
    typedef std::vector<query_element> elements_t;

    configuration _config;
    query_type _query_type;
    elements_t _elements;

    void parse_nearest_query(const tokens_t& tokens)
    {
        _query_type = QUERY_TYPE_NEAREST;
        parse_point_query(tokens);
    }

    void parse_exact_query(const tokens_t& tokens)
    {
        _query_type = QUERY_TYPE_EXACT;
        parse_point_query(tokens);
    }

    void parse_point_query(const tokens_t& tokens)
    {
        using namespace boost::algorithm;

        for (size_t i = 1; i < tokens.size(); i++)
        {
            tokens_t values;
            values.push_back(tokens[i]);

            _elements.push_back(query_element(values));
        }
    }

    void parse_range_query(const tokens_t& tokens)
    {
        _query_type = QUERY_TYPE_RANGE;

        using namespace boost::algorithm;

        for (size_t i = 1; i < tokens.size(); i++)
        {
            tokens_t values;
            split(values, tokens[i], is_any_of(_config.range_sep_tokens),
            token_compress_off);

            _elements.push_back(query_element(values));
        }
    }

public:

    query_parser(
        const std::string& query,
        size_t ndims,
        const configuration& config=configuration()
    ) :
        _config(config)
    {
        using namespace boost::algorithm;

        if (query.size() == 0)
        {
            _query_type = QUERY_TYPE_NONE;
            return;
        }

        tokens_t tokens;
        split(tokens, query, is_any_of(_config.field_sep_tokens),
            token_compress_off);

        if (tokens[0].compare(_config.range_token) == 0)
        {
            if (tokens.size() != ndims+1)
            {
                throw std::runtime_error(
                    "Invalid number of tokens for range query!");
            }

            parse_range_query(tokens);
        }
        else if (tokens[0].compare(_config.nearest_token) == 0)
        {
            if (tokens.size() != ndims+2)
            {
                throw std::runtime_error(
                    "Invalid number of tokens for nearest query!");
            }

            parse_nearest_query(tokens);
        }
        else if (tokens[0].compare(_config.exact_token) == 0)
        {
            if (tokens.size() != ndims+1)
            {
                throw std::runtime_error(
                    "Invalid number of tokens for exact query!");
            }

            parse_exact_query(tokens);
        }
        else
        {
            throw std::runtime_error("Unknown query" +
                tokens[0] + "!");
        }
    }

    query_type get_query_type() const
    {
        return _query_type;
    }

    size_t get_num_query_elements() const
    {
        return _elements.size();
    }

    const query_element& get_query_element(size_t i) const
    {
        if (i >= _elements.size())
            throw std::runtime_error("Element index out of range!");

        return _elements[i];
    }
};

}}
