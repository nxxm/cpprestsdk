/***
 * Copyright (C) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 *
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * combining_tests.cpp
 *
 * Tests for appending/combining features of the http::uri class.
 *
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 ****/

#include "stdafx.h"

using namespace web;
using namespace utility;

namespace tests
{
namespace functional
{
namespace uri_tests
{
SUITE(combining_tests)
{
    TEST(append_path)
    {
        utility::string_t uri_str = _XPLATSTR("http://testname.com/path?baz");
        uri_builder ub(uri_str);
        uri combined = ub.append_path(_XPLATSTR("/baz")).to_uri();

        VERIFY_ARE_EQUAL(uri(_XPLATSTR("http://testname.com/path/baz?baz")), combined);
    }

    TEST(append_empty_path)
    {
        utility::string_t uri_str(_XPLATSTR("http://fakeuri.net"));
        uri u = uri_str;
        uri_builder ub(u);
        uri combined = ub.append_path(_XPLATSTR("")).to_uri();

        VERIFY_ARE_EQUAL(u, combined);
    }

    TEST(append_query)
    {
        utility::string_t uri_str(_XPLATSTR("http://testname.com/path1?key1=value2"));
        uri_builder ub(uri_str);
        uri combined = ub.append_query(uri(_XPLATSTR("http://testname2.com/path2?key2=value3")).query()).to_uri();

        VERIFY_ARE_EQUAL(_XPLATSTR("http://testname.com/path1?key1=value2&key2=value3"), combined.to_string());
    }

    TEST(append_empty_query)
    {
        utility::string_t uri_str(_XPLATSTR("http://fakeuri.org/?key=value"));
        uri u(uri_str);
        uri_builder ub(u);
        uri combined = ub.append_query(_XPLATSTR("")).to_uri();

        VERIFY_ARE_EQUAL(u, combined);
    }

    TEST(append)
    {
        utility::string_t uri_str(_XPLATSTR("http://testname.com/path1?key1=value2"));
        uri_builder ub(uri_str);
        uri combined = ub.append(_XPLATSTR("http://testname2.com/path2?key2=value3")).to_uri();

        VERIFY_ARE_EQUAL(_XPLATSTR("http://testname.com/path1/path2?key1=value2&key2=value3"), combined.to_string());
        VERIFY_ARE_EQUAL(_XPLATSTR("/path1/path2?key1=value2&key2=value3"), combined.resource().to_string());
    }

    TEST(append_empty)
    {
        utility::string_t uri_str(_XPLATSTR("http://myhost.com"));
        uri u(uri_str);
        uri_builder ub(u);
        uri combined = ub.append(_XPLATSTR("")).to_uri();

        VERIFY_ARE_EQUAL(u, combined);
    }

} // SUITE(combining_tests)

} // namespace uri_tests
} // namespace functional
} // namespace tests
