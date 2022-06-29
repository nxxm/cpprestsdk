/***
 * Copyright (C) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 *
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 * negative_parsing_tests.cpp
 *
 * Negative tests for JSON parsing.
 *
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 ****/

#include "cpprest/json.h"
#include "unittestpp.h"

using namespace web;
using namespace utility;

namespace tests
{
namespace functional
{
namespace json_tests
{
template<typename T>
void verify_json_throws(T& parseString)
{
    std::error_code ec;
    VERIFY_THROWS(json::value::parse(parseString), json::json_exception);
    auto value = json::value::parse(parseString, ec);
    VERIFY_IS_TRUE(ec.value() > 0);
    VERIFY_IS_TRUE(value.is_null());
}

SUITE(negative_parsing_tests)
{
    TEST(string_t)
    {
        verify_json_throws(_XPLATSTR("\"\\k\""));
        verify_json_throws(_XPLATSTR("\" \" \""));
        verify_json_throws(_XPLATSTR("\"\\u23A\""));
        verify_json_throws(_XPLATSTR("\"\\uXY1A\""));
        verify_json_throws(_XPLATSTR("\"asdf"));
        verify_json_throws(_XPLATSTR("\\asdf"));
        verify_json_throws(_XPLATSTR("\"\"\"\""));

        // '\', '"', and control characters must be escaped (0x1F and below).
        verify_json_throws(_XPLATSTR("\"\\\""));
        verify_json_throws(_XPLATSTR("\""));
        utility::string_t str(_XPLATSTR("\""));
        str.append(1, 0x1F);
        str.append(_XPLATSTR("\""));
        verify_json_throws(str);
    }

    TEST(numbers)
    {
        verify_json_throws(_XPLATSTR("-"));
        verify_json_throws(_XPLATSTR("-."));
        verify_json_throws(_XPLATSTR("-e1"));
        verify_json_throws(_XPLATSTR("-1e"));
        verify_json_throws(_XPLATSTR("+1.1"));
        verify_json_throws(_XPLATSTR("1.1 E"));
        verify_json_throws(_XPLATSTR("1.1E-"));
        verify_json_throws(_XPLATSTR("1.1E.1"));
        verify_json_throws(_XPLATSTR("1.1E1.1"));
        verify_json_throws(_XPLATSTR("001.1"));
        verify_json_throws(_XPLATSTR("-.100"));
        verify_json_throws(_XPLATSTR("-.001"));
        verify_json_throws(_XPLATSTR(".1"));
        verify_json_throws(_XPLATSTR("0.1.1"));
    }

    // TFS 535589
    void parse_help(utility::string_t str)
    {
        utility::stringstream_t ss1;
        ss1 << str;
        verify_json_throws(ss1);
    }

    TEST(objects)
    {
        verify_json_throws(_XPLATSTR("}"));
        parse_help(_XPLATSTR("{"));
        parse_help(_XPLATSTR("{ 1, 10 }"));
        parse_help(_XPLATSTR("{ : }"));
        parse_help(_XPLATSTR("{ \"}"));
        verify_json_throws(_XPLATSTR("{"));
        verify_json_throws(_XPLATSTR("{ 1"));
        verify_json_throws(_XPLATSTR("{ \"}"));
        verify_json_throws(_XPLATSTR("{\"2\":"));
        verify_json_throws(_XPLATSTR("{\"2\":}"));
        verify_json_throws(_XPLATSTR("{\"2\": true"));
        verify_json_throws(_XPLATSTR("{\"2\": true false"));
        verify_json_throws(_XPLATSTR("{\"2\": true :false"));
        verify_json_throws(_XPLATSTR("{\"2\": false,}"));
    }

    TEST(arrays)
    {
        verify_json_throws(_XPLATSTR("]"));
        verify_json_throws(_XPLATSTR("["));
        verify_json_throws(_XPLATSTR("[ 1"));
        verify_json_throws(_XPLATSTR("[ 1,"));
        verify_json_throws(_XPLATSTR("[ 1,]"));
        verify_json_throws(_XPLATSTR("[ 1 2]"));
        verify_json_throws(_XPLATSTR("[ \"1\" : 2]"));
        parse_help(_XPLATSTR("[,]"));
        parse_help(_XPLATSTR("[ \"]"));
        parse_help(_XPLATSTR("[\"2\", false,]"));
    }

    TEST(literals_not_lower_case)
    {
        verify_json_throws(_XPLATSTR("NULL"));
        verify_json_throws(_XPLATSTR("FAlse"));
        verify_json_throws(_XPLATSTR("TRue"));
    }

    TEST(incomplete_literals)
    {
        verify_json_throws(_XPLATSTR("nul"));
        verify_json_throws(_XPLATSTR("fal"));
        verify_json_throws(_XPLATSTR("tru"));
    }

    // TFS#501321
    TEST(exception_string)
    {
        utility::string_t json_ip_str = _XPLATSTR("");
        verify_json_throws(json_ip_str);
    }

    TEST(boundary_chars)
    {
        utility::string_t str(_XPLATSTR("\""));
        str.append(1, 0x1F);
        str.append(_XPLATSTR("\""));
        parse_help(str);
    }

    TEST(stream_left_over_chars)
    {
        std::stringbuf buf;
        buf.sputn("[false]false", 12);
        std::istream stream(&buf);
        verify_json_throws(stream);
    }

// Test using Windows only API.
#ifdef _WIN32
    TEST(wstream_left_over_chars)
    {
        std::wstringbuf buf;
        buf.sputn(L"[false]false", 12);
        std::wistream stream(&buf);
        verify_json_throws(stream);
    }
#endif

    void garbage_impl(wchar_t ch)
    {
        utility::string_t ss(_XPLATSTR("{\"a\" : 10, \"b\":"));

        std::random_device rd;
        std::mt19937 eng(rd());
        std::uniform_int_distribution<unsigned int> dist(0, ch);

        for (int i = 0; i < 2500; i++)
            ss.push_back(static_cast<char_t>(dist(eng)));

        verify_json_throws(ss);
    }

    TEST(garbage_1) { garbage_impl(0x7F); }

    TEST(garbage_2) { garbage_impl(0xFF); }

    TEST(garbage_3) { garbage_impl(0xFFFF); }
}

} // namespace json_tests
} // namespace functional
} // namespace tests
