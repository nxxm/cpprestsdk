/***
 * Copyright (C) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 *
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * Tests cases for http_headers.
 *
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 ****/

#include "stdafx.h"

#include "cpprest/details/http_helpers.h"

using namespace web::http;
using namespace web::http::client;

using namespace tests::functional::http::utilities;

namespace tests
{
namespace functional
{
namespace http
{
namespace client
{
SUITE(outside_tests)
{
    TEST_FIXTURE(uri_address, request_headers)
    {
        test_http_server::scoped_server scoped(m_uri);
        test_http_server* p_server = scoped.server();
        http_client client(m_uri);

        http_request msg(methods::POST);

#ifndef __cplusplus_winrt
        // The WinRT-based HTTP stack does not support headers that have no
        // value, which means that there is no point in making this particular
        // header test, it is an unsupported feature on WinRT.
        msg.headers().add(_XPLATSTR("HEHE"), _XPLATSTR(""));
#endif

        msg.headers().add(_XPLATSTR("MyHeader"), _XPLATSTR("hehe;blach"));
        msg.headers().add(_XPLATSTR("Yo1"), _XPLATSTR("You, Too"));
        msg.headers().add(_XPLATSTR("Yo2"), _XPLATSTR("You2"));
        msg.headers().add(_XPLATSTR("Yo3"), _XPLATSTR("You3"));
        msg.headers().add(_XPLATSTR("Yo4"), _XPLATSTR("You4"));
        msg.headers().add(_XPLATSTR("Yo5"), _XPLATSTR("You5"));
        msg.headers().add(_XPLATSTR("Yo6"), _XPLATSTR("You6"));
        msg.headers().add(_XPLATSTR("Yo7"), _XPLATSTR("You7"));
        msg.headers().add(_XPLATSTR("Yo8"), _XPLATSTR("You8"));
        msg.headers().add(_XPLATSTR("Yo9"), _XPLATSTR("You9"));
        msg.headers().add(_XPLATSTR("Yo10"), _XPLATSTR("You10"));
        msg.headers().add(_XPLATSTR("Yo11"), _XPLATSTR("You11"));
        msg.headers().add(_XPLATSTR("Accept"), _XPLATSTR("text/plain"));
        VERIFY_ARE_EQUAL(_XPLATSTR("You5"), msg.headers()[_XPLATSTR("Yo5")]);
        p_server->next_request().then([&](test_request* p_request) {
            http_asserts::assert_test_request_equals(p_request, methods::POST, _XPLATSTR("/"));
            http_asserts::assert_test_request_contains_headers(p_request, msg.headers());
            p_request->reply(200);
        });
        http_asserts::assert_response_equals(client.request(msg).get(), status_codes::OK);
    }

    TEST_FIXTURE(uri_address, field_name_casing)
    {
        test_http_server::scoped_server scoped(m_uri);
        http_client client(m_uri);
        const method mtd = methods::GET;
        const utility::string_t field_name1 = _XPLATSTR("CustomHeader");
        const utility::string_t field_name2 = _XPLATSTR("CUSTOMHEADER");
        const utility::string_t field_name3 = _XPLATSTR("CuSTomHEAdeR");
        const utility::string_t value1 = _XPLATSTR("value1");
        const utility::string_t value2 = _XPLATSTR("value2");
        const utility::string_t value3 = _XPLATSTR("value3");

        http_request msg(mtd);
        msg.headers()[field_name1] = value1;
        msg.headers()[field_name2].append(_XPLATSTR(", ") + value2);
        msg.headers()[field_name3].append(_XPLATSTR(", ") + value3);
        scoped.server()->next_request().then([&](test_request* p_request) {
            http_asserts::assert_test_request_equals(p_request, mtd, _XPLATSTR("/"));
            std::map<utility::string_t, utility::string_t> expected_headers;
            expected_headers[field_name1] = value1 + _XPLATSTR(", ") + value2 + _XPLATSTR(", ") + value3;
            http_asserts::assert_test_request_contains_headers(p_request, expected_headers);
            p_request->reply(200);
        });
        http_asserts::assert_response_equals(client.request(msg).get(), status_codes::OK);
    }

    TEST_FIXTURE(uri_address, field_name_duplicate)
    {
        test_http_server::scoped_server scoped(m_uri);
        http_client client(m_uri);
        const method mtd = methods::GET;
        const utility::string_t field_name1 = _XPLATSTR("CUSTOMHEADER");
        const utility::string_t value1 = _XPLATSTR("value1");
        const utility::string_t value2 = _XPLATSTR("value2");

        http_request msg(mtd);
        msg.headers().add(field_name1, value1);
        msg.headers().add(field_name1, value2);
        scoped.server()->next_request().then([&](test_request* p_request) {
            http_asserts::assert_test_request_equals(p_request, mtd, _XPLATSTR("/"));
            std::map<utility::string_t, utility::string_t> expected_headers;
            expected_headers[field_name1] = value1 + _XPLATSTR(", ") + value2;
            http_asserts::assert_test_request_contains_headers(p_request, expected_headers);
            p_request->reply(200);
        });
        http_asserts::assert_response_equals(client.request(msg).get(), status_codes::OK);
    }

    TEST_FIXTURE(uri_address, field_name_no_multivalue_allowed)
    {
        test_http_server::scoped_server scoped(m_uri);
        http_client client(m_uri);
        const method mtd = methods::GET;

        http_request msg(mtd);

        msg.headers().set_content_type(web::http::details::mime_types::text_plain);
        msg.headers().set_content_type(web::http::details::mime_types::application_json);

        scoped.server()->next_request().then([&](test_request* p_request) {
            http_asserts::assert_test_request_equals(p_request, mtd, _XPLATSTR("/"));
            std::map<utility::string_t, utility::string_t> expected_headers;
            expected_headers[_XPLATSTR("Content-Type")] = web::http::details::mime_types::application_json;
            http_asserts::assert_test_request_contains_headers(p_request, expected_headers);
            p_request->reply(200);
        });
        http_asserts::assert_response_equals(client.request(msg).get(), status_codes::OK);
    }
    TEST_FIXTURE(uri_address, copy_move)
    {
        // copy constructor
        http_headers h1;
        h1.add(_XPLATSTR("key1"), _XPLATSTR("key2"));
        http_headers h2(h1);
        http_asserts::assert_http_headers_equals(h1, h2);

        // move constructor
        http_headers h3(std::move(h1));
        VERIFY_ARE_EQUAL(1u, h3.size());
        VERIFY_ARE_EQUAL(_XPLATSTR("key2"), h3[_XPLATSTR("key1")]);

        // assignment operator
        h1 = h3;
        VERIFY_ARE_EQUAL(1u, h1.size());
        VERIFY_ARE_EQUAL(_XPLATSTR("key2"), h1[_XPLATSTR("key1")]);
        http_asserts::assert_http_headers_equals(h1, h3);

        // move assignment operator
        h1 = http_headers();
        h1 = std::move(h2);
        VERIFY_ARE_EQUAL(1u, h1.size());
        VERIFY_ARE_EQUAL(_XPLATSTR("key2"), h1[_XPLATSTR("key1")]);
    }

    TEST_FIXTURE(uri_address, match_types)
    {
        // wchar
        http_headers h1;
        h1[_XPLATSTR("key1")] = _XPLATSTR("string");
        utility::char_t buf[12];
        VERIFY_IS_TRUE(h1.match(_XPLATSTR("key1"), buf));
        VERIFY_ARE_EQUAL(_XPLATSTR("string"), utility::string_t(buf));

        // utility::string_t
        utility::string_t wstr;
        VERIFY_IS_TRUE(h1.match(_XPLATSTR("key1"), wstr));
        VERIFY_ARE_EQUAL(_XPLATSTR("string"), wstr);

        // int
        h1[_XPLATSTR("key2")] = _XPLATSTR("22");
        int i;
        VERIFY_IS_TRUE(h1.match(_XPLATSTR("key2"), i));
        VERIFY_ARE_EQUAL(22, i);

        // unsigned long
        unsigned long l;
        VERIFY_IS_TRUE(h1.match(_XPLATSTR("key2"), l));
        VERIFY_ARE_EQUAL(22ul, l);
    }

    TEST_FIXTURE(uri_address, match_edge_cases)
    {
        // match with empty string
        http_headers h;
        h[_XPLATSTR("here")] = _XPLATSTR("");
        utility::string_t value(_XPLATSTR("k"));
        VERIFY_IS_TRUE(h.match(_XPLATSTR("HeRE"), value));
        VERIFY_ARE_EQUAL(_XPLATSTR(""), value);

        // match with string containing spaces
        h.add(_XPLATSTR("blah"), _XPLATSTR("spaces ss"));
        VERIFY_IS_TRUE(h.match(_XPLATSTR("blah"), value));
        VERIFY_ARE_EQUAL(_XPLATSTR("spaces ss"), value);

        // match failing
        value = utility::string_t();
        VERIFY_IS_FALSE(h.match(_XPLATSTR("hahah"), value));
        VERIFY_ARE_EQUAL(_XPLATSTR(""), value);
    }

    TEST_FIXTURE(uri_address, headers_find)
    {
        // Find when empty.
        http_headers h;
        VERIFY_ARE_EQUAL(h.end(), h.find(_XPLATSTR("key1")));

        // Find that exists.
        h[_XPLATSTR("key1")] = _XPLATSTR("yes");
        VERIFY_ARE_EQUAL(_XPLATSTR("yes"), h.find(_XPLATSTR("key1"))->second);

        // Find that doesn't exist.
        VERIFY_ARE_EQUAL(h.end(), h.find(_XPLATSTR("key2")));
    }

    TEST_FIXTURE(uri_address, headers_add)
    {
        // Add multiple
        http_headers h;
        h.add(_XPLATSTR("key1"), 22);
        h.add(_XPLATSTR("key2"), _XPLATSTR("str2"));
        VERIFY_ARE_EQUAL(_XPLATSTR("22"), h[_XPLATSTR("key1")]);
        VERIFY_ARE_EQUAL(_XPLATSTR("str2"), h[_XPLATSTR("key2")]);

        // Add one that already exists
        h.add(_XPLATSTR("key2"), _XPLATSTR("str3"));
        VERIFY_ARE_EQUAL(_XPLATSTR("str2, str3"), h[_XPLATSTR("key2")]);

        // Add with different case
        h.add(_XPLATSTR("KEY2"), _XPLATSTR("str4"));
        VERIFY_ARE_EQUAL(_XPLATSTR("str2, str3, str4"), h[_XPLATSTR("keY2")]);

        // Add with spaces in string
        h.add(_XPLATSTR("key3"), _XPLATSTR("value with spaces"));
        VERIFY_ARE_EQUAL(_XPLATSTR("value with spaces"), h[_XPLATSTR("key3")]);
    }

    TEST_FIXTURE(uri_address, headers_iterators)
    {
        // begin when empty
        http_headers h;
        VERIFY_ARE_EQUAL(h.begin(), h.end());

        // with some values.
        h.add(_XPLATSTR("key1"), _XPLATSTR("value1"));
        h.add(_XPLATSTR("key2"), _XPLATSTR("value2"));
        h.add(_XPLATSTR("key3"), _XPLATSTR("value3"));
        http_headers::const_iterator iter = h.begin();
        VERIFY_ARE_EQUAL(_XPLATSTR("value1"), iter->second);
        ++iter;
        VERIFY_ARE_EQUAL(_XPLATSTR("value2"), iter->second);
        ++iter;
        VERIFY_ARE_EQUAL(_XPLATSTR("value3"), iter->second);
        ++iter;
        VERIFY_ARE_EQUAL(h.end(), iter);
    }

    TEST_FIXTURE(uri_address, headers_foreach)
    {
        // begin when empty
        http_headers h;
        VERIFY_ARE_EQUAL(h.begin(), h.end());

        // with some values.
        h.add(_XPLATSTR("key1"), _XPLATSTR("value"));
        h.add(_XPLATSTR("key2"), _XPLATSTR("value"));
        h.add(_XPLATSTR("key3"), _XPLATSTR("value"));

        std::for_each(std::begin(h), std::end(h), [=](http_headers::const_reference kv) {
            VERIFY_ARE_EQUAL(_XPLATSTR("value"), kv.second);
        });

        std::for_each(
            std::begin(h), std::end(h), [=](http_headers::reference kv) { VERIFY_ARE_EQUAL(_XPLATSTR("value"), kv.second); });
    }

    TEST_FIXTURE(uri_address, response_headers)
    {
        test_http_server::scoped_server scoped(m_uri);
        http_client client(m_uri);

        std::map<utility::string_t, utility::string_t> headers;
        headers[_XPLATSTR("H1")] = _XPLATSTR("");
        headers[_XPLATSTR("H2")] = _XPLATSTR("hah");
        headers[_XPLATSTR("H3")] = _XPLATSTR("es");
        headers[_XPLATSTR("H4")] = _XPLATSTR("es;kjr");
        headers[_XPLATSTR("H5")] = _XPLATSTR("asb");
        headers[_XPLATSTR("H6")] = _XPLATSTR("abc");
        headers[_XPLATSTR("H7")] = _XPLATSTR("eds");
        headers[_XPLATSTR("H8")] = _XPLATSTR("blue");
        headers[_XPLATSTR("H9")] = _XPLATSTR("sd");
        headers[_XPLATSTR("H10")] = _XPLATSTR("res");
        test_server_utilities::verify_request(
            &client, methods::GET, _XPLATSTR("/"), scoped.server(), status_codes::OK, headers);
    }

    TEST_FIXTURE(uri_address, cache_control_header)
    {
        http_headers headers;
        VERIFY_ARE_EQUAL(headers.cache_control(), _XPLATSTR(""));
        const utility::string_t value(_XPLATSTR("custom value"));
        headers.set_cache_control(value);
        VERIFY_ARE_EQUAL(headers.cache_control(), value);
        utility::string_t foundValue;
        VERIFY_IS_TRUE(headers.match(header_names::cache_control, foundValue));
        VERIFY_ARE_EQUAL(value, foundValue);
    }

    TEST_FIXTURE(uri_address, content_length_header)
    {
        http_headers headers;
        VERIFY_ARE_EQUAL(headers.content_length(), 0);
        const size_t value = 44;
        headers.set_content_length(value);
        VERIFY_ARE_EQUAL(headers.content_length(), value);
        size_t foundValue;
        VERIFY_IS_TRUE(headers.match(header_names::content_length, foundValue));
        VERIFY_ARE_EQUAL(value, foundValue);
    }

    TEST_FIXTURE(uri_address, date_header)
    {
        http_headers headers;
        VERIFY_ARE_EQUAL(headers.date(), _XPLATSTR(""));
        const utility::datetime value(utility::datetime::utc_now());
        headers.set_date(value);
        VERIFY_ARE_EQUAL(headers.date(), value.to_string());
        utility::string_t foundValue;
        VERIFY_IS_TRUE(headers.match(header_names::date, foundValue));
        VERIFY_ARE_EQUAL(value.to_string(), foundValue);
    }

    TEST_FIXTURE(uri_address, parsing_content_type_redundantsemicolon_json)
    {
        test_http_server::scoped_server scoped(m_uri);
        web::json::value body = web::json::value::string(_XPLATSTR("Json body"));

        scoped.server()->next_request().then([&](test_request* p_request) {
            std::map<utility::string_t, utility::string_t> headers;
            headers[header_names::content_type] = _XPLATSTR("application/json; charset=utf-8;;;;");
            p_request->reply(200, _XPLATSTR("OK"), headers, utility::conversions::to_utf8string(body.serialize()));
        });

        http_client client(m_uri);
        auto resp = client.request(methods::GET).get();
        VERIFY_ARE_EQUAL(resp.extract_json().get().serialize(), body.serialize());
    }

    TEST_FIXTURE(uri_address, parsing_content_type_redundantsemicolon_string)
    {
        test_http_server::scoped_server scoped(m_uri);
        std::string body("Body");
        scoped.server()->next_request().then([&](test_request* p_request) {
            std::map<utility::string_t, utility::string_t> headers;
            headers[header_names::content_type] = _XPLATSTR("text/plain; charset  =  UTF-8;;;; ");
            p_request->reply(200, _XPLATSTR("OK"), headers, body);
        });

        http_client client(m_uri);
        auto resp = client.request(methods::GET).get();
        VERIFY_ARE_EQUAL(resp.extract_string().get(), utility::conversions::to_string_t(body));
    }

    TEST_FIXTURE(uri_address, overwrite_http_header)
    {
        test_http_server::scoped_server scoped(m_uri);
        http_client client(m_uri);

        // Test default case of cpprestsdk setting host header as host:port
        auto& host = m_uri.host();
        int port = m_uri.port();
        utility::string_t expected_default_header = host + _XPLATSTR(":") + utility::conversions::details::to_string_t(port);
        http_request default_host_headers_request(methods::GET);
        scoped.server()->next_request().then([&](test_request* p_request) {
            auto headers = p_request->m_headers;
            VERIFY_ARE_EQUAL(expected_default_header, headers[header_names::host]);
            p_request->reply(200);
        });

        client.request(default_host_headers_request).get();

#ifndef __cplusplus_winrt
        // Test case where we overwrite the host header
        http_request overwritten_host_headers_request(methods::GET);
        overwritten_host_headers_request.headers().add(_XPLATSTR("Host"), host);
        scoped.server()->next_request().then([&](test_request* p_request) {
            auto headers = p_request->m_headers;
            VERIFY_ARE_EQUAL(host, headers[header_names::host]);
            p_request->reply(200);
        });
        client.request(overwritten_host_headers_request).get();
#endif
    }
} // SUITE(header_tests)

} // namespace client
} // namespace http
} // namespace functional
} // namespace tests
