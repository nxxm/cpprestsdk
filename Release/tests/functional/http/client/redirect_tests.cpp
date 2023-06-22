/***
 * Copyright (C) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 *
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * Tests cases for multiple requests and responses from an http_client.
 *
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 ****/

#include "stdafx.h"
#ifdef _WIN32
#include <Windows.h>
#include <VersionHelpers.h>
#endif // _WIN32

using namespace web::http;
using namespace web::http::client;

using namespace tests::functional::http::utilities;

#if defined(_WIN32) && !defined(CPPREST_FORCE_HTTP_CLIENT_ASIO)
#define USING_WINHTTP 1
#else
#define USING_WINHTTP 0
#endif

namespace tests
{
namespace functional
{
namespace http
{
namespace client
{
pplx::task<void> next_reply_assert(
    test_http_server* p_server,
    const method& method,
    const utility::string_t& path,
    status_code code = status_codes::OK,
    const utility::string_t& location = _XPLATSTR(""))
{
    return p_server->next_request().then([=](test_request* p_request) {
        http_asserts::assert_test_request_equals(p_request, method, path);
        size_t content_length;
        VERIFY_ARE_EQUAL(methods::POST == method,
            p_request->match_header(header_names::content_length, content_length));

        std::map<utility::string_t, utility::string_t> headers;
        if (!location.empty())
        {
            headers[header_names::location] = location;
        }

        // web::http::details::get_default_reason_phrase is internal :-/
        p_request->reply(code, {}, headers);
    });
}

pplx::task<void> next_reply_assert(
    test_http_server* p_server,
    const utility::string_t& path,
    status_code code = status_codes::OK,
    const utility::string_t& location = _XPLATSTR(""))
{
    return next_reply_assert(p_server, methods::GET, path, code, location);
}

SUITE(redirect_tests)
{
    TEST_FIXTURE(uri_address, follows_multiple_redirects_by_default)
    {
        test_http_server::scoped_server scoped(m_uri);
        auto p_server = scoped.server();

        std::vector<pplx::task<void>> replies;
        replies.push_back(next_reply_assert(p_server, _XPLATSTR("/"), status_codes::MovedPermanently, _XPLATSTR("/moved-here")));
        replies.push_back(next_reply_assert(p_server, _XPLATSTR("/moved-here"), status_codes::TemporaryRedirect, _XPLATSTR("/moved-there")));
        replies.push_back(next_reply_assert(p_server, _XPLATSTR("/moved-there"), status_codes::Found, _XPLATSTR("/found-elsewhere")));
        replies.push_back(next_reply_assert(p_server, _XPLATSTR("/found-elsewhere")));

        http_client_config config;
        http_client client(m_uri, config);
        VERIFY_NO_THROWS(
            http_asserts::assert_response_equals(client.request(methods::GET).get(), status_codes::OK)
        );
        p_server->close();
        for (auto& reply : replies)
        {
            VERIFY_NO_THROWS(reply.get());
        }
    }

    TEST_FIXTURE(uri_address, follows_retrieval_redirect)
    {
        test_http_server::scoped_server scoped(m_uri);
        auto p_server = scoped.server();

        std::vector<pplx::task<void>> replies;
        replies.push_back(next_reply_assert(p_server, methods::POST, _XPLATSTR("/"), status_codes::SeeOther, _XPLATSTR("/see-here")));
        replies.push_back(next_reply_assert(p_server, methods::GET, _XPLATSTR("/see-here")));

        http_client_config config;
        http_client client(m_uri, config);

        VERIFY_NO_THROWS(
            http_asserts::assert_response_equals(client.request(methods::POST, _XPLATSTR(""), _XPLATSTR("body")).get(), status_codes::OK);
        );
        p_server->close();
        for (auto& reply : replies)
        {
            VERIFY_NO_THROWS(reply.get());
        }
    }

    TEST_FIXTURE(uri_address, obeys_max_redirects)
    {
        test_http_server::scoped_server scoped(m_uri);
        auto p_server = scoped.server();

        std::vector<pplx::task<void>> replies;
        replies.push_back(next_reply_assert(p_server, _XPLATSTR("/"), status_codes::MovedPermanently, _XPLATSTR("/moved-here")));
        replies.push_back(next_reply_assert(p_server, _XPLATSTR("/moved-here"), status_codes::TemporaryRedirect, _XPLATSTR("/moved-there")));
        replies.push_back(next_reply_assert(p_server, _XPLATSTR("/moved-there"), status_codes::Found, _XPLATSTR("/found-elsewhere")));

        http_client_config config;
        config.set_max_redirects(2);
        http_client client(m_uri, config);

        VERIFY_NO_THROWS(
            http_asserts::assert_response_equals(client.request(methods::GET).get(), status_codes::Found)
        );
        p_server->close();
        for (auto& reply : replies)
        {
            VERIFY_NO_THROWS(reply.get());
        }
    }

    TEST_FIXTURE(uri_address, can_disable_redirects)
    {
        test_http_server::scoped_server scoped(m_uri);
        auto p_server = scoped.server();

        std::vector<pplx::task<void>> replies;
        replies.push_back(next_reply_assert(p_server, _XPLATSTR("/"), status_codes::MovedPermanently, _XPLATSTR("/moved-here")));

        http_client_config config;
        config.set_max_redirects(0);
        http_client client(m_uri, config);

        VERIFY_NO_THROWS(
            http_asserts::assert_response_equals(client.request(methods::GET).get(), status_codes::MovedPermanently)
        );
        p_server->close();
        for (auto& reply : replies)
        {
            VERIFY_NO_THROWS(reply.get());
        }
    }

    TEST(does_not_follow_https_to_http_by_default)
    {
        handle_timeout([] {
            http_client_config config;
            http_client client(_XPLATSTR("https://http.badssl.com/"), config);
            VERIFY_NO_THROWS(
                http_asserts::assert_response_equals(client.request(methods::GET).get(), status_codes::MovedPermanently)
            );
        });
    }

    TEST(can_follow_https_to_http)
    {
        handle_timeout([] {
            http_client_config config;
            config.set_https_to_http_redirects(true);
            http_client client(_XPLATSTR("https://http.badssl.com/"), config);
            VERIFY_NO_THROWS(
                http_asserts::assert_response_equals(client.request(methods::GET).get(), status_codes::OK)
            );
        });
    }

    TEST_FIXTURE(uri_address, follows_permanent_redirect)
    {
#if USING_WINHTTP
        // note that 308 Permanent Redirect is only supported by WinHTTP from Windows 10
        if (!IsWindows10OrGreater()) {
            return;
        }
#endif // USING_WINHTTP
        test_http_server::scoped_server scoped(m_uri);
        auto p_server = scoped.server();

        std::vector<pplx::task<void>> replies;
        replies.push_back(next_reply_assert(p_server, _XPLATSTR("/"), status_codes::PermanentRedirect, _XPLATSTR("/moved-here")));
        replies.push_back(next_reply_assert(p_server, _XPLATSTR("/moved-here")));

        http_client_config config;
        http_client client(m_uri, config);

        VERIFY_NO_THROWS(
            http_asserts::assert_response_equals(client.request(methods::GET).get(), status_codes::OK)
        );
        p_server->close();
        for (auto& reply : replies)
        {
            VERIFY_NO_THROWS(reply.get());
        }
    }

    TEST_FIXTURE(uri_address, may_throw_if_no_location)
    {
        test_http_server::scoped_server scoped(m_uri);
        auto p_server = scoped.server();

        std::vector<pplx::task<void>> replies;
        replies.push_back(next_reply_assert(p_server, _XPLATSTR("/"), status_codes::MovedPermanently));

        http_client_config config;
        http_client client(m_uri, config);

        // implementation-specific behaviour
#if USING_WINHTTP
        VERIFY_THROWS(
            client.request(methods::GET).get(),
            http_exception
        );
#else
        VERIFY_NO_THROWS(
            http_asserts::assert_response_equals(client.request(methods::GET).get(), status_codes::MovedPermanently)
        );
#endif
        p_server->close();
        for (auto& reply : replies)
        {
            VERIFY_NO_THROWS(reply.get());
        }
    }

    TEST_FIXTURE(uri_address, should_not_follow_cyclic_redirect)
    {
        test_http_server::scoped_server scoped(m_uri);
        auto p_server = scoped.server();

        std::vector<pplx::task<void>> replies;
        replies.push_back(next_reply_assert(p_server, _XPLATSTR("/"), status_codes::TemporaryRedirect, _XPLATSTR("/briefly-here")));
        replies.push_back(next_reply_assert(p_server, _XPLATSTR("/briefly-here"), status_codes::MovedPermanently, _XPLATSTR("/")));
#if USING_WINHTTP
        replies.push_back(next_reply_assert(p_server, _XPLATSTR("/"), status_codes::NotFound));
#endif

        http_client_config config;
        http_client client(m_uri, config);

        // implementation-specific behaviour
#if USING_WINHTTP
        VERIFY_NO_THROWS(
            http_asserts::assert_response_equals(client.request(methods::GET).get(), status_codes::NotFound)
        );
#else // ^^^ USING_WINHTTP / !USING_WINHTTP vvv
        VERIFY_NO_THROWS(
            http_asserts::assert_response_equals(client.request(methods::GET).get(), status_codes::MovedPermanently)
        );
#endif // USING_WINHTTP
        p_server->close();
        for (auto& reply : replies)
        {
            VERIFY_NO_THROWS(reply.get());
        }
    }

    TEST_FIXTURE(uri_address, may_follow_unchanged_redirect)
    {
        test_http_server::scoped_server scoped(m_uri);
        auto p_server = scoped.server();

        std::vector<pplx::task<void>> replies;
        replies.push_back(next_reply_assert(p_server, methods::POST, _XPLATSTR("/"), status_codes::TemporaryRedirect, _XPLATSTR("/retry-here")));
#if USING_WINHTTP
        replies.push_back(next_reply_assert(p_server, methods::POST, _XPLATSTR("/retry-here")));
#endif

        http_client_config config;
        http_client client(m_uri, config);

        // implementation-specific behaviour
#if USING_WINHTTP
        VERIFY_NO_THROWS(
            http_asserts::assert_response_equals(client.request(methods::POST, _XPLATSTR(""), _XPLATSTR("body")).get(), status_codes::OK)
        );
#else // ^^^ USING_WINHTTP / !USING_WINHTTP vvv
        VERIFY_NO_THROWS(
            http_asserts::assert_response_equals(client.request(methods::POST, _XPLATSTR(""), _XPLATSTR("body")).get(), status_codes::TemporaryRedirect)
        );
#endif // USING_WINHTTP
        p_server->close();
        for (auto& reply : replies)
        {
            VERIFY_NO_THROWS(reply.get());
        }
    }

    TEST_FIXTURE(uri_address, may_not_follow_manual_redirect)
    {
        test_http_server::scoped_server scoped(m_uri);
        auto p_server = scoped.server();

        std::vector<pplx::task<void>> replies;
        replies.push_back(next_reply_assert(p_server, _XPLATSTR("/"), status_codes::MultipleChoices, _XPLATSTR("/prefer-here")));
#if USING_WINHTTP
        replies.push_back(next_reply_assert(p_server, _XPLATSTR("/prefer-here")));
#endif

        http_client_config config;
        http_client client(m_uri, config);

        // implementation-specific behaviour
#if USING_WINHTTP
        VERIFY_NO_THROWS(
            http_asserts::assert_response_equals(client.request(methods::GET).get(), status_codes::OK)
        );
#else // ^^^ USING_WINHTTP / !USING_WINHTTP vvv
        VERIFY_NO_THROWS(
            http_asserts::assert_response_equals(client.request(methods::GET).get(), status_codes::MultipleChoices)
        );
#endif // USING_WINHTTP
        p_server->close();
        for (auto& reply : replies)
        {
            VERIFY_NO_THROWS(reply.get());
        }
    }

} // SUITE(redirect_tests)

} // namespace client
} // namespace http
} // namespace functional
} // namespace tests
