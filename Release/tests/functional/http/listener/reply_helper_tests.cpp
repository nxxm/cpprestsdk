/***
 * ==++==
 *
 * Copyright (c) Microsoft Corporation.  All rights reserved.
 *
 * ==--==
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * reply_helper_tests.cpp
 *
 * Tests cases covering the reply helper functions on HTTP response.
 *
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 ****/

#include "stdafx.h"

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

using namespace tests::functional::http::utilities;

namespace tests
{
namespace functional
{
namespace http
{
namespace listener
{
SUITE(reply_helper_tests)
{
    TEST_FIXTURE(uri_address, json)
    {
        http_listener listener(m_uri);
        listener.open().wait();
        test_http_client::scoped_client client(m_uri);
        test_http_client* p_client = client.client();

        listener.support(
            [](http_request request) { request.reply(status_codes::OK, json::value::parse(_XPLATSTR("true"))).wait(); });
        VERIFY_ARE_EQUAL(0, p_client->request(methods::POST, _XPLATSTR("")));
        p_client->next_response()
            .then([&](test_response* p_response) {
                http_asserts::assert_test_response_equals(
                    p_response, status_codes::OK, _XPLATSTR("application/json"), _XPLATSTR("true"));
            })
            .wait();

        listener.close().wait();
    }

    TEST_FIXTURE(uri_address, string)
    {
        http_listener listener(m_uri);
        listener.open().wait();
        test_http_client::scoped_client client(m_uri);
        test_http_client* p_client = client.client();

        listener.support([](http_request request) {
            std::string body("test str");
            request.reply(status_codes::OK, body).wait();
        });
        VERIFY_ARE_EQUAL(0, p_client->request(methods::POST, _XPLATSTR("")));
        p_client->next_response()
            .then([&](test_response* p_response) {
                http_asserts::assert_test_response_equals(
                    p_response, status_codes::OK, _XPLATSTR("text/plain; charset=utf-8"), _XPLATSTR("test str"));
            })
            .wait();

        // content type and string body
        listener.support([](http_request request) {
            utility::string_t s(_XPLATSTR("test str"));
            request.reply(status_codes::OK, s, _XPLATSTR("custom content")).wait();
        });
        VERIFY_ARE_EQUAL(0, p_client->request(methods::POST, _XPLATSTR("")));
        p_client->next_response()
            .then([&](test_response* p_response) {
                http_asserts::assert_test_response_equals(
                    p_response, status_codes::OK, _XPLATSTR("custom content"), _XPLATSTR("test str"));
            })
            .wait();

        // content type and rvalue reference string body
        listener.support(
            [](http_request request) { request.reply(status_codes::OK, "test str", "text/plain").wait(); });
        VERIFY_ARE_EQUAL(0, p_client->request(methods::POST, _XPLATSTR("")));
        p_client->next_response()
            .then([&](test_response* p_response) {
                http_asserts::assert_test_response_equals(p_response, status_codes::OK, _XPLATSTR("text/plain"), _XPLATSTR("test str"));
            })
            .wait();

        listener.close().wait();
    }
}

} // namespace listener
} // namespace http
} // namespace functional
} // namespace tests
