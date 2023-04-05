/***
 * Copyright (C) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 *
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * constructor_string_tests.cpp
 *
 * Tests for constructors of the uri class
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
SUITE(accessor_tests)
{
    TEST(authority_string)
    {
        uri u(_XPLATSTR("http://testname.com:81/path?baz"));
        uri a = u.authority();

        VERIFY_ARE_EQUAL(_XPLATSTR("/path"), u.path());
        VERIFY_ARE_EQUAL(_XPLATSTR("http"), a.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR("testname.com"), a.host());
        VERIFY_ARE_EQUAL(81, a.port());
        VERIFY_ARE_EQUAL(uri(_XPLATSTR("http://testname.com:81")), a);
    }

    TEST(authority_wstring)
    {
        uri u(_XPLATSTR("http://testname.com:81/path?baz"));
        uri a = u.authority();

        VERIFY_ARE_EQUAL(_XPLATSTR("/path"), u.path());
        VERIFY_ARE_EQUAL(_XPLATSTR("http"), a.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR("testname.com"), a.host());
        VERIFY_ARE_EQUAL(81, a.port());
        VERIFY_ARE_EQUAL(uri(_XPLATSTR("http://testname.com:81")), a);
    }

} // SUITE(accessor_tests)

} // namespace uri_tests
} // namespace functional
} // namespace tests
