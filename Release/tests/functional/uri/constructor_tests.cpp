/***
 * Copyright (C) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 *
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * constructor_tests.cpp
 *
 * Tests for constructors of the uri class.
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
SUITE(constructor_tests)
{
    TEST(parsing_constructor_char)
    {
        uri u(uri::encode_uri(_XPLATSTR("net.tcp://steve:@testname.com:81/bleh%?qstring#goo")));

        VERIFY_ARE_EQUAL(_XPLATSTR("net.tcp"), u.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR("steve:"), u.user_info());
        VERIFY_ARE_EQUAL(_XPLATSTR("testname.com"), u.host());
        VERIFY_ARE_EQUAL(81, u.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("/bleh%25"), u.path());
        VERIFY_ARE_EQUAL(_XPLATSTR("qstring"), u.query());
        VERIFY_ARE_EQUAL(_XPLATSTR("goo"), u.fragment());
    }

    TEST(parsing_constructor_encoded_string)
    {
        uri u(uri::encode_uri(_XPLATSTR("net.tcp://testname.com:81/bleh%?qstring#goo")));

        VERIFY_ARE_EQUAL(_XPLATSTR("net.tcp"), u.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR("testname.com"), u.host());
        VERIFY_ARE_EQUAL(81, u.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("/bleh%25"), u.path());
        VERIFY_ARE_EQUAL(_XPLATSTR("qstring"), u.query());
        VERIFY_ARE_EQUAL(_XPLATSTR("goo"), u.fragment());
    }

    TEST(parsing_constructor_string_string)
    {
        uri u(uri::encode_uri(_XPLATSTR("net.tcp://testname.com:81/bleh%?qstring#goo")));

        VERIFY_ARE_EQUAL(_XPLATSTR("net.tcp"), u.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR("testname.com"), u.host());
        VERIFY_ARE_EQUAL(81, u.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("/bleh%25"), u.path());
        VERIFY_ARE_EQUAL(_XPLATSTR("qstring"), u.query());
        VERIFY_ARE_EQUAL(_XPLATSTR("goo"), u.fragment());
    }

    TEST(empty_strings)
    {
        VERIFY_IS_TRUE(uri(_XPLATSTR("")).is_empty());
        VERIFY_IS_TRUE(uri(_XPLATSTR("")).is_empty());
        VERIFY_IS_TRUE(uri(uri::encode_uri(_XPLATSTR(""))).is_empty());
    }

    TEST(default_constructor) { VERIFY_IS_TRUE(uri().is_empty()); }

    TEST(relative_ref_string)
    {
        uri u(uri::encode_uri(_XPLATSTR("first/second#boff")));

        VERIFY_ARE_EQUAL(_XPLATSTR(""), u.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), u.host());
        VERIFY_ARE_EQUAL(0, u.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("first/second"), u.path());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), u.query());
        VERIFY_ARE_EQUAL(_XPLATSTR("boff"), u.fragment());
    }

    TEST(absolute_ref_string)
    {
        uri u(uri::encode_uri(_XPLATSTR("/first/second#boff")));

        VERIFY_ARE_EQUAL(_XPLATSTR(""), u.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), u.host());
        VERIFY_ARE_EQUAL(0, u.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("/first/second"), u.path());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), u.query());
        VERIFY_ARE_EQUAL(_XPLATSTR("boff"), u.fragment());
    }

    TEST(copy_constructor)
    {
        uri original(_XPLATSTR("http://st:pass@localhost:456/path1?qstring#goo"));
        uri new_uri(original);

        VERIFY_ARE_EQUAL(original, new_uri);
    }

    TEST(move_constructor)
    {
        const utility::string_t uri_str(_XPLATSTR("http://localhost:456/path1?qstring#goo"));
        uri original(uri_str);
        uri new_uri = std::move(original);

        VERIFY_ARE_EQUAL(uri_str, new_uri.to_string());
        VERIFY_ARE_EQUAL(uri(uri_str), new_uri);
    }

    TEST(assignment_operator)
    {
        uri original(_XPLATSTR("http://localhost:456/path?qstring#goo"));
        uri new_uri = original;

        VERIFY_ARE_EQUAL(original, new_uri);
    }

    // Tests invalid URI being passed in constructor.
    TEST(parsing_constructor_invalid)
    {
        VERIFY_THROWS(uri(_XPLATSTR("123http://localhost:345/")), uri_exception);
        VERIFY_THROWS(uri(_XPLATSTR("h*ttp://localhost:345/")), uri_exception);
        VERIFY_THROWS(uri(_XPLATSTR("http://localhost:345/\"")), uri_exception);
        VERIFY_THROWS(uri(_XPLATSTR("http://localhost:345/path?\"")), uri_exception);
        VERIFY_THROWS(uri(_XPLATSTR("http://local\"host:345/")), uri_exception);
    }

    // Tests a variety of different URIs using the examples in RFC 2732
    TEST(RFC_2732_examples_string)
    {
        // The URI parser will make characters lower case
        uri http1(_XPLATSTR("http://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]:80/index.html"));
        VERIFY_ARE_EQUAL(_XPLATSTR("http"), http1.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR("[fedc:ba98:7654:3210:fedc:ba98:7654:3210]"), http1.host());
        VERIFY_ARE_EQUAL(80, http1.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("/index.html"), http1.path());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), http1.query());

        uri http2(_XPLATSTR("http://[1080:0:0:0:8:800:200C:417A]/index.html"));
        VERIFY_ARE_EQUAL(_XPLATSTR("http"), http2.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR("[1080:0:0:0:8:800:200c:417a]"), http2.host());
        VERIFY_ARE_EQUAL(0, http2.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("/index.html"), http2.path());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), http2.query());

        uri http3(_XPLATSTR("https://[3ffe:2a00:100:7031::1]"));
        VERIFY_ARE_EQUAL(_XPLATSTR("https"), http3.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR("[3ffe:2a00:100:7031::1]"), http3.host());
        VERIFY_ARE_EQUAL(0, http3.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("/"), http3.path());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), http3.query());

        uri http4(_XPLATSTR("http://[::192.9.5.5]/ipng"));
        VERIFY_ARE_EQUAL(_XPLATSTR("http"), http4.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR("[::192.9.5.5]"), http4.host());
        VERIFY_ARE_EQUAL(0, http4.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("/ipng"), http4.path());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), http4.query());

        uri http5(_XPLATSTR("http://[1080::8:800:200C:417A]/foo"));
        VERIFY_ARE_EQUAL(_XPLATSTR("http"), http5.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR("[1080::8:800:200c:417a]"), http5.host());
        VERIFY_ARE_EQUAL(0, http5.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("/foo"), http5.path());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), http5.query());

        uri http6(_XPLATSTR("http://[::FFFF:129.144.52.38]:80/index.html"));
        VERIFY_ARE_EQUAL(_XPLATSTR("http"), http6.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR("[::ffff:129.144.52.38]"), http6.host());
        VERIFY_ARE_EQUAL(80, http6.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("/index.html"), http6.path());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), http6.query());

        uri http7(_XPLATSTR("http://[2010:836B:4179::836B:4179]"));
        VERIFY_ARE_EQUAL(_XPLATSTR("http"), http7.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR("[2010:836b:4179::836b:4179]"), http7.host());
        VERIFY_ARE_EQUAL(0, http7.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("/"), http7.path());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), http7.query());
    }

    // Tests a variety of different URIs using the examples in RFC 3986.
    TEST(RFC_3968_examples_string)
    {
        uri ftp(_XPLATSTR("ftp://ftp.is.co.za/rfc/rfc1808.txt"));
        VERIFY_ARE_EQUAL(_XPLATSTR("ftp"), ftp.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), ftp.user_info());
        VERIFY_ARE_EQUAL(_XPLATSTR("ftp.is.co.za"), ftp.host());
        VERIFY_ARE_EQUAL(0, ftp.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("/rfc/rfc1808.txt"), ftp.path());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), ftp.query());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), ftp.fragment());

        // TFS #371892
        // uri ldap(_XPLATSTR("ldap://[2001:db8::7]/?c=GB#objectClass?one"));
        // VERIFY_ARE_EQUAL(_XPLATSTR("ldap"), ldap.scheme());
        // VERIFY_ARE_EQUAL(_XPLATSTR(""), ldap.user_info());
        // VERIFY_ARE_EQUAL(_XPLATSTR("2001:db8::7"), ldap.host());
        // VERIFY_ARE_EQUAL(0, ldap.port());
        // VERIFY_ARE_EQUAL(_XPLATSTR("/"), ldap.path());
        // VERIFY_ARE_EQUAL(_XPLATSTR("c=GB"), ldap.query());
        // VERIFY_ARE_EQUAL(_XPLATSTR("objectClass?one"), ldap.fragment());

        // We don't support anything scheme specific like in C# so
        // these common ones don't have a great experience yet.
        uri mailto(_XPLATSTR("mailto:John.Doe@example.com"));
        VERIFY_ARE_EQUAL(_XPLATSTR("mailto"), mailto.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), mailto.user_info());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), mailto.host());
        VERIFY_ARE_EQUAL(0, mailto.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("John.Doe@example.com"), mailto.path());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), mailto.query());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), mailto.fragment());

        uri tel(_XPLATSTR("tel:+1-816-555-1212"));
        VERIFY_ARE_EQUAL(_XPLATSTR("tel"), tel.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), tel.user_info());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), tel.host());
        VERIFY_ARE_EQUAL(0, tel.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("+1-816-555-1212"), tel.path());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), tel.query());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), tel.fragment());

        uri telnet(_XPLATSTR("telnet://192.0.2.16:80/"));
        VERIFY_ARE_EQUAL(_XPLATSTR("telnet"), telnet.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), telnet.user_info());
        VERIFY_ARE_EQUAL(_XPLATSTR("192.0.2.16"), telnet.host());
        VERIFY_ARE_EQUAL(80, telnet.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("/"), telnet.path());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), telnet.query());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), telnet.fragment());
    }

    TEST(user_info_string)
    {
        uri ftp(_XPLATSTR("ftp://johndoe:testname@ftp.is.co.za/rfc/rfc1808.txt"));
        VERIFY_ARE_EQUAL(_XPLATSTR("ftp"), ftp.scheme());
        VERIFY_ARE_EQUAL(_XPLATSTR("johndoe:testname"), ftp.user_info());
        VERIFY_ARE_EQUAL(_XPLATSTR("ftp.is.co.za"), ftp.host());
        VERIFY_ARE_EQUAL(0, ftp.port());
        VERIFY_ARE_EQUAL(_XPLATSTR("/rfc/rfc1808.txt"), ftp.path());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), ftp.query());
        VERIFY_ARE_EQUAL(_XPLATSTR(""), ftp.fragment());
    }

    // Test query component can be separated with '&' or ';'.
    TEST(query_seperated_with_semi_colon)
    {
        uri u(_XPLATSTR("http://localhost/path1?key1=val1;key2=val2"));
        VERIFY_ARE_EQUAL(_XPLATSTR("key1=val1;key2=val2"), u.query());
    }

} // SUITE(constructor_tests)

} // namespace uri_tests
} // namespace functional
} // namespace tests
