/***
 * ==++==
 *
 * Copyright (c) Microsoft Corporation.  All rights reserved.
 *
 * ==--==
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * header_tests.cpp
 *
 * Tests cases for using HTTP requests/response headers with http_listener.
 *
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 ****/

#include "stdafx.h"

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
SUITE(header_tests)
{
    TEST_FIXTURE(uri_address, request_headers)
    {
        http_listener listener(m_uri);
        listener.open().wait();
        test_http_client::scoped_client client(m_uri);
        test_http_client* p_client = client.client();
        const utility::string_t mtd = methods::GET;
        std::map<utility::string_t, utility::string_t> headers;

        // single header value.
        headers[_XPLATSTR("Header1")] = _XPLATSTR("Value1");
        listener.support([&](http_request request) {
            http_asserts::assert_request_equals(request, mtd, _XPLATSTR("/"), headers);
            request.reply(status_codes::OK).wait();
        });
        VERIFY_ARE_EQUAL(0, p_client->request(mtd, _XPLATSTR(""), headers));
        p_client->next_response()
            .then([](test_response* p_response) {
                http_asserts::assert_test_response_equals(p_response, status_codes::OK);
            })
            .wait();

        // empty header value.
        headers.clear();
        headers[_XPLATSTR("Key1")] = _XPLATSTR("");
        listener.support([&](http_request request) {
            http_asserts::assert_request_equals(request, mtd, _XPLATSTR("/"), headers);
            request.reply(status_codes::OK).wait();
        });
        VERIFY_ARE_EQUAL(0, p_client->request(mtd, _XPLATSTR(""), headers));
        p_client->next_response()
            .then([](test_response* p_response) {
                http_asserts::assert_test_response_equals(p_response, status_codes::OK);
            })
            .wait();

        // 10 headers.
        headers.clear();
        headers[_XPLATSTR("MyHeader")] = _XPLATSTR("hehe;blach");
        headers[_XPLATSTR("Yo1")] = _XPLATSTR("You, Too");
        headers[_XPLATSTR("Yo2")] = _XPLATSTR("You2");
        headers[_XPLATSTR("Yo3")] = _XPLATSTR("You3");
        headers[_XPLATSTR("Yo4")] = _XPLATSTR("You4");
        headers[_XPLATSTR("Yo5")] = _XPLATSTR("You5");
        headers[_XPLATSTR("Yo6")] = _XPLATSTR("You6");
        headers[_XPLATSTR("Yo7")] = _XPLATSTR("You7");
        headers[_XPLATSTR("Yo8")] = _XPLATSTR("You8");
        headers[_XPLATSTR("Yo9")] = _XPLATSTR("You9");
        headers[_XPLATSTR("Yo10")] = _XPLATSTR("You10");
        headers[_XPLATSTR("Yo11")] = _XPLATSTR("You11");
        headers[_XPLATSTR("Accept")] = _XPLATSTR("text/plain");
        listener.support([&](http_request request) {
            http_asserts::assert_request_equals(request, mtd, _XPLATSTR("/"), headers);
            request.reply(status_codes::OK).wait();
        });
        VERIFY_ARE_EQUAL(0, p_client->request(mtd, _XPLATSTR(""), headers));
        p_client->next_response()
            .then([](test_response* p_response) {
                http_asserts::assert_test_response_equals(p_response, status_codes::OK);
            })
            .wait();

        // several headers different casings.
        headers.clear();
        headers[_XPLATSTR("CUSTOMHEADER")] = _XPLATSTR("value1");
        headers[_XPLATSTR("customHEADER")] = _XPLATSTR("value2");
        headers[_XPLATSTR("CUSTOMheaDER")] = _XPLATSTR("value3");
        listener.support([&](http_request request) {
            std::map<utility::string_t, utility::string_t> h;
            h[_XPLATSTR("CUSTOMHEADER")] = _XPLATSTR("value1, value3, value2");
            http_asserts::assert_request_equals(request, mtd, _XPLATSTR("/"), h);
            request.reply(status_codes::OK).wait();
        });
        VERIFY_ARE_EQUAL(0, p_client->request(mtd, _XPLATSTR(""), headers));
        p_client->next_response()
            .then([](test_response* p_response) {
                http_asserts::assert_test_response_equals(p_response, status_codes::OK);
            })
            .wait();
        listener.close().wait();
    }

    TEST_FIXTURE(uri_address, request_known_headers)
    {
        http_listener listener(m_uri);
        listener.open().wait();
        test_http_client::scoped_client client(m_uri);
        test_http_client* p_client = client.client();
        const utility::string_t mtd = methods::GET;
        std::map<utility::string_t, utility::string_t> headers;

        // "Date" was being incorrectly mapped to "Data"
        // see https://github.com/microsoft/cpprestsdk/issues/1208
        headers[_XPLATSTR("Date")] = _XPLATSTR("Mon, 29 Jul 2019 12:32:57 GMT");
        listener.support([&](http_request request) {
            http_asserts::assert_request_equals(request, mtd, _XPLATSTR("/"), headers);
            request.reply(status_codes::OK).wait();
        });
        VERIFY_ARE_EQUAL(0, p_client->request(mtd, _XPLATSTR(""), headers));
        p_client->next_response()
            .then([](test_response* p_response) {
                http_asserts::assert_test_response_equals(p_response, status_codes::OK);
            })
            .wait();
        listener.close().wait();
    }

    TEST_FIXTURE(uri_address, response_headers)
    {
        http_listener listener(m_uri);
        listener.open().wait();
        test_http_client::scoped_client client(m_uri);
        test_http_client* p_client = client.client();

        // No http_request/response classes can be around for close to complete.
        {
            // header with empty value
            http_response response(status_codes::OK);
            response.headers()[_XPLATSTR("Key1")] = _XPLATSTR("");
            listener.support([&](http_request request) {
                http_asserts::assert_request_equals(request, methods::POST, _XPLATSTR("/"));
                request.reply(response).wait();
            });
            VERIFY_ARE_EQUAL(0u, p_client->request(methods::POST, _XPLATSTR("")));
            p_client->next_response()
                .then([&](test_response* p_response) {
                    http_asserts::assert_test_response_equals(p_response, status_codes::OK, response.headers());
                })
                .wait();

            // 10 headers
            response = http_response(status_codes::Accepted);
            response.headers()[_XPLATSTR("MyHeader")] = _XPLATSTR("hehe;blach");
            response.headers()[_XPLATSTR("Yo1")] = _XPLATSTR("You, Too");
            response.headers()[_XPLATSTR("Yo2")] = _XPLATSTR("You2");
            response.headers()[_XPLATSTR("Yo3")] = _XPLATSTR("You3");
            response.headers()[_XPLATSTR("Yo4")] = _XPLATSTR("You4");
            response.headers()[_XPLATSTR("Yo5")] = _XPLATSTR("You5");
            response.headers()[_XPLATSTR("Yo6")] = _XPLATSTR("You6");
            response.headers()[_XPLATSTR("Yo7")] = _XPLATSTR("You7");
            response.headers()[_XPLATSTR("Yo8")] = _XPLATSTR("You8");
            response.headers()[_XPLATSTR("Yo9")] = _XPLATSTR("You9");
            response.headers()[_XPLATSTR("Yo10")] = _XPLATSTR("You10");
            response.headers()[_XPLATSTR("Yo11")] = _XPLATSTR("You11");
            response.headers()[_XPLATSTR("Accept")] = _XPLATSTR("text/plain");
            listener.support([&](http_request request) {
                http_asserts::assert_request_equals(request, methods::POST, _XPLATSTR("/"));
                request.reply(response).wait();
            });
            VERIFY_ARE_EQUAL(0u, p_client->request(methods::POST, _XPLATSTR("")));
            p_client->next_response()
                .then([&](test_response* p_response) {
                    http_asserts::assert_test_response_equals(p_response, status_codes::Accepted, response.headers());
                })
                .wait();

            // several headers in different casings
            response = http_response(status_codes::BadGateway);
            response.headers().add(_XPLATSTR("Key1"), _XPLATSTR("value1"));
            response.headers()[_XPLATSTR("KEY1")] += _XPLATSTR("value2");
            listener.support([&](http_request request) {
                http_asserts::assert_request_equals(request, methods::POST, _XPLATSTR("/"));
                request.reply(response).wait();
            });
            VERIFY_ARE_EQUAL(0u, p_client->request(methods::POST, _XPLATSTR("")));
            p_client->next_response()
                .then([&](test_response* p_response) {
                    http_asserts::assert_test_response_equals(p_response, status_codes::BadGateway, response.headers());
                })
                .wait();

            // duplicate headers fields
            response = http_response(status_codes::BadGateway);
            response.headers().add(_XPLATSTR("Key1"), _XPLATSTR("value1"));
            response.headers().add(_XPLATSTR("Key1"), _XPLATSTR("value2"));
            listener.support([&](http_request request) {
                http_asserts::assert_request_equals(request, methods::POST, _XPLATSTR("/"));
                request.reply(response).wait();
            });
            VERIFY_ARE_EQUAL(0u, p_client->request(methods::POST, _XPLATSTR("")));
            p_client->next_response()
                .then([&](test_response* p_response) {
                    http_asserts::assert_test_response_equals(p_response, status_codes::BadGateway, response.headers());
                })
                .wait();
        }

        listener.close().wait();
    }
}

} // namespace listener
} // namespace http
} // namespace functional
} // namespace tests
