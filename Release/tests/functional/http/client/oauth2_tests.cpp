/***
 * Copyright (C) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 *
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * Test cases for oauth2.
 *
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 ****/

#include "stdafx.h"

#include "cpprest/details/http_helpers.h"

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::http::details;
using namespace web::http::oauth2::experimental;
using namespace utility;
using namespace concurrency;

using namespace tests::functional::http::utilities;
extern utility::string_t _to_base64(const unsigned char* ptr, size_t size);

namespace tests
{
namespace functional
{
namespace http
{
namespace client
{
static std::vector<unsigned char> to_body_data(utility::string_t str)
{
    const std::string utf8(conversions::to_utf8string(std::move(str)));
    return std::vector<unsigned char>(utf8.data(), utf8.data() + utf8.size());
}

static bool is_application_x_www_form_urlencoded(test_request* request)
{
    const auto content_type(request->m_headers[header_names::content_type]);
    return (0 == content_type.find(mime_types::application_x_www_form_urlencoded));
}

static utility::string_t get_request_user_agent(test_request* request)
{
    if (request->m_headers.find(header_names::user_agent) != request->m_headers.end())
    {
        return request->m_headers[header_names::user_agent];
    }

    return utility::string_t();
}

SUITE(oauth2_tests)
{
    struct oauth2_test_setup
    {
        oauth2_test_setup()
            : m_uri(_XPLATSTR("http://localhost:16743/"))
            , m_oauth2_config(_XPLATSTR("123ABC"), _XPLATSTR("456DEF"), _XPLATSTR("https://test1"), m_uri.to_string(), _XPLATSTR("https://bar"))
            , m_scoped(m_uri)
        {
        }

        web::http::uri m_uri;
        oauth2_config m_oauth2_config;
        test_http_server::scoped_server m_scoped;
    };

#define TEST_ACCESSOR(value_, name_)                                                                                   \
    t.set_##name_(value_);                                                                                             \
    VERIFY_ARE_EQUAL(value_, t.name_());

    TEST(oauth2_token_accessors)
    {
        oauth2_token t;
        TEST_ACCESSOR(_XPLATSTR("b%20456"), access_token)
        TEST_ACCESSOR(_XPLATSTR("a%123"), refresh_token)
        TEST_ACCESSOR(_XPLATSTR("b%20456"), token_type)
        TEST_ACCESSOR(_XPLATSTR("ad.ww xyz"), scope)
        TEST_ACCESSOR(0, expires_in)
        TEST_ACCESSOR(123, expires_in)
    }

    TEST(oauth2_config_accessors)
    {
        oauth2_config t(_XPLATSTR(""), _XPLATSTR(""), _XPLATSTR(""), _XPLATSTR(""), _XPLATSTR(""));
        TEST_ACCESSOR(_XPLATSTR("ABC123abc"), client_key)
        TEST_ACCESSOR(_XPLATSTR("123abcABC"), client_secret)
        TEST_ACCESSOR(_XPLATSTR("x:/t/a?q=c&a#3"), auth_endpoint)
        TEST_ACCESSOR(_XPLATSTR("y:///?a=21#1=2"), token_endpoint)
        TEST_ACCESSOR(_XPLATSTR("z://?=#"), redirect_uri)
        TEST_ACCESSOR(_XPLATSTR("xyzw=stuv"), scope)
        TEST_ACCESSOR(_XPLATSTR("1234567890"), state)
        TEST_ACCESSOR(_XPLATSTR("keyx"), access_token_key)
        TEST_ACCESSOR(true, implicit_grant)
        TEST_ACCESSOR(false, implicit_grant)
        TEST_ACCESSOR(true, bearer_auth)
        TEST_ACCESSOR(false, bearer_auth)
        TEST_ACCESSOR(true, http_basic_auth)
        TEST_ACCESSOR(false, http_basic_auth)
    }

#undef TEST_ACCESSOR

    TEST(oauth2_build_authorization_uri)
    {
        oauth2_config config(_XPLATSTR(""), _XPLATSTR(""), _XPLATSTR(""), _XPLATSTR(""), _XPLATSTR(""));
        config.set_state(_XPLATSTR("xyzzy"));
        config.set_implicit_grant(false);

        // Empty authorization URI.
        {
            VERIFY_ARE_EQUAL(_XPLATSTR("/?response_type=code&client_id=&redirect_uri=&state=xyzzy"),
                             config.build_authorization_uri(false));
        }

        // Authorization URI with scope parameter.
        {
            config.set_scope(_XPLATSTR("testing_123"));
            VERIFY_ARE_EQUAL(_XPLATSTR("/?response_type=code&client_id=&redirect_uri=&state=xyzzy&scope=testing_123"),
                             config.build_authorization_uri(false));
        }

        // Full authorization URI with scope.
        {
            config.set_client_key(_XPLATSTR("4567abcd"));
            config.set_auth_endpoint(_XPLATSTR("https://test1"));
            config.set_redirect_uri(_XPLATSTR("http://localhost:8080"));
            VERIFY_ARE_EQUAL(_XPLATSTR("https://test1/?response_type=code&client_id=4567abcd&redirect_uri=http://")
                                 _XPLATSTR("localhost:8080&state=xyzzy&scope=testing_123"),
                             config.build_authorization_uri(false));
        }

        // Verify again with implicit grant.
        {
            config.set_implicit_grant(true);
            VERIFY_ARE_EQUAL(_XPLATSTR("https://test1/?response_type=token&client_id=4567abcd&redirect_uri=http://")
                                 _XPLATSTR("localhost:8080&state=xyzzy&scope=testing_123"),
                             config.build_authorization_uri(false));
        }

        // Verify that a new state() will be generated.
        {
            const uri auth_uri(config.build_authorization_uri(true));
            auto params = uri::split_query(auth_uri.query());
            VERIFY_ARE_NOT_EQUAL(params[_XPLATSTR("state")], _XPLATSTR("xyzzy"));
        }
    }

    TEST_FIXTURE(oauth2_test_setup, oauth2_token_from_code)
    {
        VERIFY_IS_FALSE(m_oauth2_config.is_enabled());

        m_oauth2_config.set_user_agent(_XPLATSTR("test_user_agent"));

        // Fetch using HTTP Basic authentication.
        {
            m_scoped.server()->next_request().then([](test_request* request) {
                VERIFY_ARE_EQUAL(request->m_method, methods::POST);

                VERIFY_IS_TRUE(is_application_x_www_form_urlencoded(request));

                VERIFY_ARE_EQUAL(_XPLATSTR("Basic MTIzQUJDOjQ1NkRFRg=="), request->m_headers[header_names::authorization]);

                VERIFY_ARE_EQUAL(
                    to_body_data(_XPLATSTR("grant_type=authorization_code&code=789GHI&redirect_uri=https%3A%2F%2Fbar")),
                    request->m_body);

                VERIFY_ARE_EQUAL(_XPLATSTR("test_user_agent"), get_request_user_agent(request));

                std::map<utility::string_t, utility::string_t> headers;
                headers[header_names::content_type] = mime_types::application_json;
                request->reply(
                    status_codes::OK, _XPLATSTR(""), headers, "{\"access_token\":\"xyzzy123\",\"token_type\":\"bearer\"}");
            });

            m_oauth2_config.token_from_code(_XPLATSTR("789GHI")).wait();
            VERIFY_ARE_EQUAL(_XPLATSTR("xyzzy123"), m_oauth2_config.token().access_token());
            VERIFY_IS_TRUE(m_oauth2_config.is_enabled());
        }

        // Fetch using client key & secret in request body (x-www-form-urlencoded).
        {
            m_scoped.server()->next_request().then([](test_request* request) {
                VERIFY_IS_TRUE(is_application_x_www_form_urlencoded(request));

                VERIFY_ARE_EQUAL(_XPLATSTR(""), request->m_headers[header_names::authorization]);

                VERIFY_ARE_EQUAL(to_body_data(_XPLATSTR("grant_type=authorization_code&code=789GHI&redirect_uri=https%3A%2F%")
                                                  _XPLATSTR("2Fbar&client_id=123ABC&client_secret=456DEF")),
                                 request->m_body);

                VERIFY_ARE_EQUAL(_XPLATSTR("test_user_agent"), get_request_user_agent(request));

                std::map<utility::string_t, utility::string_t> headers;
                headers[header_names::content_type] = mime_types::application_json;
                request->reply(
                    status_codes::OK, _XPLATSTR(""), headers, "{\"access_token\":\"xyzzy123\",\"token_type\":\"bearer\"}");
            });

            m_oauth2_config.set_token(oauth2_token()); // Clear token.
            VERIFY_IS_FALSE(m_oauth2_config.is_enabled());

            m_oauth2_config.set_http_basic_auth(false);
            m_oauth2_config.token_from_code(_XPLATSTR("789GHI")).wait();

            VERIFY_ARE_EQUAL(_XPLATSTR("xyzzy123"), m_oauth2_config.token().access_token());
            VERIFY_IS_TRUE(m_oauth2_config.is_enabled());
        }
    }

    TEST_FIXTURE(oauth2_test_setup, oauth2_token_from_redirected_uri)
    {
        // Authorization code grant.
        {
            m_scoped.server()->next_request().then([](test_request* request) {
                std::map<utility::string_t, utility::string_t> headers;
                headers[header_names::content_type] = mime_types::application_json;
                request->reply(
                    status_codes::OK, _XPLATSTR(""), headers, "{\"access_token\":\"test1\",\"token_type\":\"bearer\"}");
            });

            m_oauth2_config.set_implicit_grant(false);
            m_oauth2_config.set_state(_XPLATSTR("xyzzy"));

            const web::http::uri redirected_uri(m_uri.to_string() + _XPLATSTR("?code=sesame&state=xyzzy"));
            m_oauth2_config.token_from_redirected_uri(redirected_uri).wait();

            VERIFY_IS_TRUE(m_oauth2_config.token().is_valid_access_token());
            VERIFY_ARE_EQUAL(m_oauth2_config.token().access_token(), _XPLATSTR("test1"));
        }

        // Implicit grant.
        {
            m_oauth2_config.set_implicit_grant(true);
            const web::http::uri redirected_uri(m_uri.to_string() + _XPLATSTR("#access_token=abcd1234&state=xyzzy"));
            m_oauth2_config.token_from_redirected_uri(redirected_uri).wait();

            VERIFY_IS_TRUE(m_oauth2_config.token().is_valid_access_token());
            VERIFY_ARE_EQUAL(m_oauth2_config.token().access_token(), _XPLATSTR("abcd1234"));
        }
    }

    TEST_FIXTURE(oauth2_test_setup, oauth2_token_from_refresh)
    {
        oauth2_token token(_XPLATSTR("accessing"));
        token.set_refresh_token(_XPLATSTR("refreshing"));
        m_oauth2_config.set_token(token);
        VERIFY_IS_TRUE(m_oauth2_config.is_enabled());

        // Verify token refresh without scope.
        m_scoped.server()->next_request().then([](test_request* request) {
            VERIFY_ARE_EQUAL(request->m_method, methods::POST);

            VERIFY_IS_TRUE(is_application_x_www_form_urlencoded(request));

            VERIFY_ARE_EQUAL(_XPLATSTR("Basic MTIzQUJDOjQ1NkRFRg=="), request->m_headers[header_names::authorization]);

            VERIFY_ARE_EQUAL(to_body_data(_XPLATSTR("grant_type=refresh_token&refresh_token=refreshing")), request->m_body);

            std::map<utility::string_t, utility::string_t> headers;
            headers[header_names::content_type] = mime_types::application_json;
            request->reply(status_codes::OK,
                           _XPLATSTR(""),
                           headers,
                           "{\"access_token\":\"ABBA\",\"refresh_token\":\"BAZ\",\"token_type\":\"bearer\"}");
        });

        m_oauth2_config.token_from_refresh().wait();
        VERIFY_ARE_EQUAL(_XPLATSTR("ABBA"), m_oauth2_config.token().access_token());
        VERIFY_ARE_EQUAL(_XPLATSTR("BAZ"), m_oauth2_config.token().refresh_token());

        // Verify chaining refresh tokens and refresh with scope.
        m_scoped.server()->next_request().then([](test_request* request) {
            VERIFY_IS_TRUE(is_application_x_www_form_urlencoded(request));

            VERIFY_ARE_EQUAL(to_body_data(_XPLATSTR("grant_type=refresh_token&refresh_token=BAZ&scope=xyzzy")),
                             request->m_body);

            std::map<utility::string_t, utility::string_t> headers;
            headers[header_names::content_type] = mime_types::application_json;
            request->reply(status_codes::OK, _XPLATSTR(""), headers, "{\"access_token\":\"done\",\"token_type\":\"bearer\"}");
        });

        m_oauth2_config.set_scope(_XPLATSTR("xyzzy"));
        m_oauth2_config.token_from_refresh().wait();
        VERIFY_ARE_EQUAL(_XPLATSTR("done"), m_oauth2_config.token().access_token());
    }

    TEST_FIXTURE(oauth2_test_setup, oauth2_bearer_token)
    {
        m_oauth2_config.set_token(oauth2_token(_XPLATSTR("12345678")));
        http_client_config config;

        // Default, bearer token in "Authorization" header (bearer_auth() == true)
        {
            config.set_oauth2(m_oauth2_config);

            http_client client(m_uri, config);
            m_scoped.server()->next_request().then([](test_request* request) {
                VERIFY_ARE_EQUAL(_XPLATSTR("Bearer 12345678"), request->m_headers[header_names::authorization]);
                VERIFY_ARE_EQUAL(_XPLATSTR("/"), request->m_path);
                request->reply(status_codes::OK);
            });

            http_response response = client.request(methods::GET).get();
            VERIFY_ARE_EQUAL(status_codes::OK, response.status_code());
        }

        // Bearer token in query, default access token key (bearer_auth() == false)
        {
            m_oauth2_config.set_bearer_auth(false);
            config.set_oauth2(m_oauth2_config);

            http_client client(m_uri, config);
            m_scoped.server()->next_request().then([](test_request* request) {
                VERIFY_ARE_EQUAL(_XPLATSTR(""), request->m_headers[header_names::authorization]);
                VERIFY_ARE_EQUAL(_XPLATSTR("/?access_token=12345678"), request->m_path);
                request->reply(status_codes::OK);
            });

            http_response response = client.request(methods::GET).get();
            VERIFY_ARE_EQUAL(status_codes::OK, response.status_code());
        }

        // Bearer token in query, updated token, custom access token key (bearer_auth() == false)
        {
            m_oauth2_config.set_bearer_auth(false);
            m_oauth2_config.set_access_token_key(_XPLATSTR("open"));
            m_oauth2_config.set_token(oauth2_token(_XPLATSTR("Sesame")));
            config.set_oauth2(m_oauth2_config);

            http_client client(m_uri, config);
            m_scoped.server()->next_request().then([](test_request* request) {
                VERIFY_ARE_EQUAL(_XPLATSTR(""), request->m_headers[header_names::authorization]);
                VERIFY_ARE_EQUAL(_XPLATSTR("/?open=Sesame"), request->m_path);
                request->reply(status_codes::OK);
            });

            http_response response = client.request(methods::GET).get();
            VERIFY_ARE_EQUAL(status_codes::OK, response.status_code());
        }
    }

    TEST_FIXTURE(oauth2_test_setup, oauth2_token_parsing)
    {
        VERIFY_IS_FALSE(m_oauth2_config.is_enabled());

        // Verify reply JSON 'access_token', 'refresh_token', 'expires_in' and 'scope'.
        {
            m_scoped.server()->next_request().then([](test_request* request) {
                std::map<utility::string_t, utility::string_t> headers;
                headers[header_names::content_type] = mime_types::application_json;
                request->reply(status_codes::OK,
                               _XPLATSTR(""),
                               headers,
                               "{\"access_token\":\"123\",\"refresh_token\":\"ABC\",\"token_type\":\"bearer\","
                               "\"expires_in\":12345678,\"scope\":\"baz\"}");
            });

            m_oauth2_config.token_from_code(_XPLATSTR("")).wait();
            VERIFY_ARE_EQUAL(_XPLATSTR("123"), m_oauth2_config.token().access_token());
            VERIFY_ARE_EQUAL(_XPLATSTR("ABC"), m_oauth2_config.token().refresh_token());
            VERIFY_ARE_EQUAL(12345678, m_oauth2_config.token().expires_in());
            VERIFY_ARE_EQUAL(_XPLATSTR("baz"), m_oauth2_config.token().scope());
            VERIFY_IS_TRUE(m_oauth2_config.is_enabled());
        }

        // Verify undefined 'expires_in' and 'scope'.
        {
            m_scoped.server()->next_request().then([](test_request* request) {
                std::map<utility::string_t, utility::string_t> headers;
                headers[header_names::content_type] = mime_types::application_json;
                request->reply(
                    status_codes::OK, _XPLATSTR(""), headers, "{\"access_token\":\"123\",\"token_type\":\"bearer\"}");
            });

            const utility::string_t test_scope(_XPLATSTR("wally world"));
            m_oauth2_config.set_scope(test_scope);

            m_oauth2_config.token_from_code(_XPLATSTR("")).wait();
            VERIFY_ARE_EQUAL(oauth2_token::undefined_expiration, m_oauth2_config.token().expires_in());
            VERIFY_ARE_EQUAL(test_scope, m_oauth2_config.token().scope());
        }
    }

} // SUITE(oauth2_tests)

} // namespace client
} // namespace http
} // namespace functional
} // namespace tests
