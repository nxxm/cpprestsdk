/***
 * Copyright (C) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 *
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * diagnostic_tests.cpp
 *
 * Tests for diagnostic functions like is_host_loopback of the uri class.
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
SUITE(diagnostic_tests)
{
    TEST(empty_components)
    {
        VERIFY_IS_TRUE(uri().is_empty());

        VERIFY_IS_FALSE(uri().is_authority());

        VERIFY_IS_FALSE(uri().is_host_loopback());
        VERIFY_IS_FALSE(uri().is_host_wildcard());
        VERIFY_IS_FALSE(uri().is_host_portable());

        VERIFY_IS_FALSE(uri().is_port_default());
    }

    TEST(is_authority)
    {
        VERIFY_IS_TRUE(uri(_XPLATSTR("http://first.second/")).is_authority());
        VERIFY_IS_TRUE(uri(_XPLATSTR("http://first.second")).is_authority());

        VERIFY_IS_FALSE(uri(_XPLATSTR("http://first.second/b")).is_authority());
        VERIFY_IS_FALSE(uri(_XPLATSTR("http://first.second?qstring")).is_authority());
        VERIFY_IS_FALSE(uri(_XPLATSTR("http://first.second#third")).is_authority());
    }

    TEST(has_same_authority)
    {
        VERIFY_IS_TRUE(uri(_XPLATSTR("http://first.second/")).has_same_authority(uri(_XPLATSTR("http://first.second/path"))));
        VERIFY_IS_TRUE(uri(_XPLATSTR("http://first.second:83/")).has_same_authority(uri(_XPLATSTR("http://first.second:83/path:83"))));

        VERIFY_IS_FALSE(uri(_XPLATSTR("http://first.second:82/")).has_same_authority(uri(_XPLATSTR("http://first.second/path"))));
        VERIFY_IS_FALSE(uri(_XPLATSTR("tcp://first.second:82/")).has_same_authority(uri(_XPLATSTR("http://first.second/path"))));
        VERIFY_IS_FALSE(uri(_XPLATSTR("http://path.:82/")).has_same_authority(uri(_XPLATSTR("http://first.second/path"))));
    }

    TEST(has_same_authority_empty)
    {
        VERIFY_IS_FALSE(uri().has_same_authority(uri()));
        VERIFY_IS_FALSE(uri(_XPLATSTR("http://first.second/")).has_same_authority(uri()));
        VERIFY_IS_FALSE(uri().has_same_authority(uri(_XPLATSTR("http://first.second/"))));
    }

    TEST(is_host_wildcard)
    {
        VERIFY_IS_TRUE(uri(_XPLATSTR("http://*/")).is_host_wildcard());
        VERIFY_IS_TRUE(uri(_XPLATSTR("http://+/?qstring")).is_host_wildcard());

        VERIFY_IS_FALSE(uri(_XPLATSTR("http://bleh/?qstring")).is_host_wildcard());
        VERIFY_IS_FALSE(uri(_XPLATSTR("http://+*/?qstring")).is_host_wildcard());
    }

    TEST(is_host_loopback)
    {
        VERIFY_IS_TRUE(uri(_XPLATSTR("http://localhost/")).is_host_loopback());
        VERIFY_IS_TRUE(uri(_XPLATSTR("http://LoCALHoST/")).is_host_loopback());

        VERIFY_IS_FALSE(uri(_XPLATSTR("http://127")).is_host_loopback());
        VERIFY_IS_FALSE(uri(_XPLATSTR("http://bleh/?qstring")).is_host_loopback());
        VERIFY_IS_FALSE(uri(_XPLATSTR("http://+*/?qstring")).is_host_loopback());
        VERIFY_IS_TRUE(uri(_XPLATSTR("http://127.0.0.1/")).is_host_loopback());
        VERIFY_IS_TRUE(uri(_XPLATSTR("http://127.155.0.1/")).is_host_loopback());
        VERIFY_IS_FALSE(uri(_XPLATSTR("http://128.0.0.1/")).is_host_loopback());
    }

    TEST(is_host_portable)
    {
        VERIFY_IS_TRUE(uri(_XPLATSTR("http://bleh/?qstring")).is_host_portable());

        VERIFY_IS_FALSE(uri(_XPLATSTR("http://localhost/")).is_host_portable());
        VERIFY_IS_FALSE(uri(_XPLATSTR("http://+/?qstring")).is_host_portable());
    }

    TEST(is_port_default)
    {
        VERIFY_IS_TRUE(uri(_XPLATSTR("http://bleh/?qstring")).is_port_default());
        VERIFY_IS_TRUE(uri(_XPLATSTR("http://localhost:0/")).is_port_default());

        VERIFY_IS_FALSE(uri(_XPLATSTR("http://+:85/?qstring")).is_port_default());
    }

    TEST(is_path_empty)
    {
        VERIFY_IS_TRUE(uri(_XPLATSTR("http://bleh/?qstring")).is_path_empty());
        VERIFY_IS_TRUE(uri(_XPLATSTR("http://localhost:0")).is_path_empty());

        VERIFY_IS_FALSE(uri(_XPLATSTR("http://+:85/path/?qstring")).is_path_empty());
    }

} // SUITE(diagnostic_tests)

} // namespace uri_tests
} // namespace functional
} // namespace tests
