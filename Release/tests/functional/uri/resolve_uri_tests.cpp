#include "stdafx.h"

using namespace web;
using namespace utility;

namespace tests
{
namespace functional
{
namespace uri_tests
{
// testing resolution against examples from Section 5.4 https://tools.ietf.org/html/rfc3986#section-5.4
SUITE(resolve_uri_tests)
{
    // 5.4.1. Normal Examples https://tools.ietf.org/html/rfc3986#section-5.4.1
    TEST(resolve_uri_normal)
    {
        const uri baseUri = _XPLATSTR("http://a/b/c/d;p?q");

        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g:h")), _XPLATSTR("g:h"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g")), _XPLATSTR("http://a/b/c/g"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("./g")), _XPLATSTR("http://a/b/c/g"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g/")), _XPLATSTR("http://a/b/c/g/"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("/g")), _XPLATSTR("http://a/g"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("//g")), _XPLATSTR("http://g"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("?y")), _XPLATSTR("http://a/b/c/d;p?y"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g?y")), _XPLATSTR("http://a/b/c/g?y"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("#s")), _XPLATSTR("http://a/b/c/d;p?q#s"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g#s")), _XPLATSTR("http://a/b/c/g#s"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g?y#s")), _XPLATSTR("http://a/b/c/g?y#s"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR(";x")), _XPLATSTR("http://a/b/c/;x"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g;x")), _XPLATSTR("http://a/b/c/g;x"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g;x?y#s")), _XPLATSTR("http://a/b/c/g;x?y#s"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("")), _XPLATSTR("http://a/b/c/d;p?q"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR(".")), _XPLATSTR("http://a/b/c/"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("./")), _XPLATSTR("http://a/b/c/"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("..")), _XPLATSTR("http://a/b/"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("../")), _XPLATSTR("http://a/b/"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("../g")), _XPLATSTR("http://a/b/g"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("../..")), _XPLATSTR("http://a/"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("../../")), _XPLATSTR("http://a/"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("../../g")), _XPLATSTR("http://a/g"));
    }
    // 5.4.2. Abnormal Examples https://tools.ietf.org/html/rfc3986#section-5.4.2
    TEST(resolve_uri_abnormal)
    {
        const uri baseUri = _XPLATSTR("http://a/b/c/d;p?q");

        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("../../../g")), _XPLATSTR("http://a/g"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("../../../../g")), _XPLATSTR("http://a/g"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("/./g")), _XPLATSTR("http://a/g"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("/../g")), _XPLATSTR("http://a/g"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g.")), _XPLATSTR("http://a/b/c/g."));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR(".g")), _XPLATSTR("http://a/b/c/.g"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g..")), _XPLATSTR("http://a/b/c/g.."));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("..g")), _XPLATSTR("http://a/b/c/..g"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("./../g")), _XPLATSTR("http://a/b/g"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("./g/.")), _XPLATSTR("http://a/b/c/g/"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g/./h")), _XPLATSTR("http://a/b/c/g/h"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g/../h")), _XPLATSTR("http://a/b/c/h"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g;x=1/./y")), _XPLATSTR("http://a/b/c/g;x=1/y"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g;x=1/../y")), _XPLATSTR("http://a/b/c/y"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g?y/./x")), _XPLATSTR("http://a/b/c/g?y/./x"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g?y/../x")), _XPLATSTR("http://a/b/c/g?y/../x"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g#s/./x")), _XPLATSTR("http://a/b/c/g#s/./x"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("g#s/../x")), _XPLATSTR("http://a/b/c/g#s/../x"));
        VERIFY_ARE_EQUAL(baseUri.resolve_uri(_XPLATSTR("http:g")), _XPLATSTR("http:g"));
    }

} // SUITE(resolve_uri_tests)

} // namespace uri_tests
} // namespace functional
} // namespace tests
