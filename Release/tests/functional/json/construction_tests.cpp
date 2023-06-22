/***
 * Copyright (C) Microsoft. All rights reserved.
 * Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
 *
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * construction_tests.cpp
 *
 * Tests creating JSON values.
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
SUITE(construction_tests)
{
#if defined(__cplusplus_winrt)

    TEST(winrt_platform_string)
    {
        Platform::String ^ platformStr = "Hello!";
        json::value jstr = json::value::string(platformStr->Data());
        CHECK(jstr.is_string());
        CHECK_EQUAL(jstr.serialize(), _XPLATSTR("\"Hello!\""));
    }

#endif

    TEST(assignment_op)
    {
        json::value arr = json::value::array();
        arr[0] = json::value(true);

        json::value ass_copy = arr;
        VERIFY_IS_TRUE(ass_copy.is_array());
        VERIFY_ARE_EQUAL(_XPLATSTR("true"), ass_copy[0].serialize());
        ass_copy[1] = json::value(false);
        VERIFY_ARE_EQUAL(_XPLATSTR("false"), ass_copy[1].serialize());
        VERIFY_ARE_EQUAL(_XPLATSTR("null"), arr[1].serialize());
    }

    TEST(copy_ctor_array)
    {
        json::value arr = json::value::array();
        arr[0] = json::value(true);

        json::value copy(arr);
        VERIFY_IS_TRUE(copy.is_array());
        VERIFY_ARE_EQUAL(_XPLATSTR("true"), copy[0].serialize());
        copy[1] = json::value(false);
        VERIFY_ARE_EQUAL(_XPLATSTR("false"), copy[1].serialize());
        VERIFY_ARE_EQUAL(_XPLATSTR("null"), arr[1].serialize());
    }

    TEST(copy_ctor_object)
    {
        json::value obj = json::value::object();
        utility::string_t keyName(_XPLATSTR("key"));
        obj[keyName] = json::value(false);

        // Copy object that has values added.
        json::value copy(obj);
        VERIFY_IS_TRUE(copy.is_object());
        VERIFY_ARE_EQUAL(_XPLATSTR("false"), copy[keyName].serialize());
        obj[keyName] = json::value(true);
        VERIFY_ARE_EQUAL(_XPLATSTR("false"), copy[keyName].serialize());
        VERIFY_ARE_EQUAL(_XPLATSTR("true"), obj[keyName].serialize());

        // Copy object that parses with value, but none additional added.
        obj = json::value::parse(_XPLATSTR("{\"key\": true}"));
        json::value copy2(obj);
        VERIFY_IS_TRUE(copy2.is_object());
        obj[keyName] = json::value(false);
        VERIFY_IS_TRUE(copy2.size() == 1);
        VERIFY_ARE_EQUAL(_XPLATSTR("false"), obj[keyName].serialize());
        VERIFY_ARE_EQUAL(_XPLATSTR("true"), copy2[keyName].serialize());
    }

    TEST(copy_ctor_string)
    {
        utility::string_t strValue(_XPLATSTR("teststr"));
        json::value str = json::value::string(strValue);

        json::value copy(str);
        VERIFY_IS_TRUE(copy.is_string());
        VERIFY_ARE_EQUAL(strValue, copy.as_string());
        str = json::value::string(_XPLATSTR("teststr2"));
        VERIFY_ARE_EQUAL(strValue, copy.as_string());
        VERIFY_ARE_EQUAL(_XPLATSTR("teststr2"), str.as_string());
    }

    TEST(copy_ctor_with_escaped)
    {
        auto str = json::value::parse(_XPLATSTR("\"\\n\""));
        VERIFY_ARE_EQUAL(_XPLATSTR("\n"), str.as_string());

        auto copy = str;
        VERIFY_ARE_EQUAL(_XPLATSTR("\n"), copy.as_string());
    }

    TEST(move_ctor)
    {
        json::value obj;
        obj[_XPLATSTR("A")] = json::value(true);

        json::value moved(std::move(obj));
        VERIFY_IS_TRUE(moved.is_object());
        VERIFY_ARE_EQUAL(_XPLATSTR("true"), moved[_XPLATSTR("A")].serialize());
        moved[_XPLATSTR("B")] = json::value(false);
        VERIFY_ARE_EQUAL(_XPLATSTR("false"), moved[_XPLATSTR("B")].serialize());
    }

    TEST(move_assignment_op)
    {
        json::value obj;
        obj[_XPLATSTR("A")] = json::value(true);

        json::value moved;
        moved = std::move(obj);
        VERIFY_IS_TRUE(moved.is_object());
        VERIFY_ARE_EQUAL(_XPLATSTR("true"), moved[_XPLATSTR("A")].serialize());
        moved[_XPLATSTR("B")] = json::value(false);
        VERIFY_ARE_EQUAL(_XPLATSTR("false"), moved[_XPLATSTR("B")].serialize());
    }

    TEST(constructor_overloads)
    {
        json::value v0;
        json::value v1(17);
        json::value v2(3.1415);
        json::value v3(true);
        const utility::char_t* p4 = _XPLATSTR("Hello!");
        json::value v4(p4);

        json::value v5(_XPLATSTR("Hello Again!"));
        json::value v6(_XPLATSTR("YES YOU KNOW IT"));
        json::value v7(_XPLATSTR("HERE ID IS"));

        const utility::char_t* p9 = _XPLATSTR("Hello not-escaped!");
        json::value v8(p9, true);
        json::value v9(p9, false);

        VERIFY_ARE_EQUAL(v0.type(), json::value::Null);
        VERIFY_IS_TRUE(v0.is_null());
        VERIFY_ARE_EQUAL(v1.type(), json::value::Number);
        VERIFY_IS_TRUE(v1.is_number());
        VERIFY_IS_TRUE(v1.is_integer());
        VERIFY_IS_FALSE(v1.is_double());
        VERIFY_ARE_EQUAL(v2.type(), json::value::Number);
        VERIFY_IS_TRUE(v2.is_number());
        VERIFY_IS_TRUE(v2.is_double());
        VERIFY_IS_FALSE(v2.is_integer());
        VERIFY_ARE_EQUAL(v3.type(), json::value::Boolean);
        VERIFY_IS_TRUE(v3.is_boolean());
        VERIFY_ARE_EQUAL(v4.type(), json::value::String);
        VERIFY_IS_TRUE(v4.is_string());
        VERIFY_ARE_EQUAL(v5.type(), json::value::String);
        VERIFY_IS_TRUE(v5.is_string());
        VERIFY_ARE_EQUAL(v6.type(), json::value::String);
        VERIFY_IS_TRUE(v6.is_string());
        VERIFY_ARE_EQUAL(v7.type(), json::value::String);
        VERIFY_IS_TRUE(v7.is_string());
        VERIFY_ARE_EQUAL(v8.type(), json::value::String);
        VERIFY_IS_TRUE(v8.is_string());
        VERIFY_ARE_EQUAL(v9.type(), json::value::String);
        VERIFY_IS_TRUE(v9.is_string());
    }

    TEST(factory_overloads)
    {
        json::value v0 = json::value::null();
        json::value v1 = json::value::number(17);
        json::value v2 = json::value::number(3.1415);
        json::value v3 = json::value::boolean(true);
        json::value v4 = json::value::string(_XPLATSTR("Hello!"));
        json::value v5 = json::value::string(_XPLATSTR("Hello Again!"));
        json::value v6 = json::value::string(_XPLATSTR("Hello!"));
        json::value v7 = json::value::string(_XPLATSTR("Hello Again!"));
        json::value v8 = json::value::string(_XPLATSTR("Hello not-escaped!"), true);
        json::value v9 = json::value::string(_XPLATSTR("Hello not-escaped!"), false);
        json::value v10 = json::value::object();
        json::value v11 = json::value::array();

        VERIFY_ARE_EQUAL(v0.type(), json::value::Null);
        VERIFY_ARE_EQUAL(v1.type(), json::value::Number);
        VERIFY_ARE_EQUAL(v2.type(), json::value::Number);
        VERIFY_ARE_EQUAL(v3.type(), json::value::Boolean);
        VERIFY_ARE_EQUAL(v4.type(), json::value::String);
        VERIFY_ARE_EQUAL(v5.type(), json::value::String);
        VERIFY_ARE_EQUAL(v6.type(), json::value::String);
        VERIFY_ARE_EQUAL(v7.type(), json::value::String);
        VERIFY_ARE_EQUAL(v8.type(), json::value::String);
        VERIFY_ARE_EQUAL(v9.type(), json::value::String);
        VERIFY_ARE_EQUAL(v10.type(), json::value::Object);
        VERIFY_IS_TRUE(v10.is_object());
        VERIFY_ARE_EQUAL(v11.type(), json::value::Array);
        VERIFY_IS_TRUE(v11.is_array());
    }

    TEST(object_construction)
    {
        // Factory which takes a vector.
        std::vector<std::pair<string_t, json::value>> f;
        f.push_back(std::make_pair(_XPLATSTR("abc"), json::value(true)));
        f.push_back(std::make_pair(_XPLATSTR("xyz"), json::value(44)));
        json::value obj = json::value::object(f);

        VERIFY_ARE_EQUAL(f.size(), obj.size());

        obj[_XPLATSTR("abc")] = json::value::string(_XPLATSTR("str"));
        obj[_XPLATSTR("123")] = json::value(false);

        VERIFY_ARE_NOT_EQUAL(f.size(), obj.size());
        VERIFY_ARE_EQUAL(json::value::string(_XPLATSTR("str")).serialize(), obj[_XPLATSTR("abc")].serialize());
        VERIFY_ARE_EQUAL(json::value(false).serialize(), obj[_XPLATSTR("123")].serialize());

        // Tests constructing empty and adding.
        auto val1 = json::value::object();
        val1[_XPLATSTR("A")] = 44;
        val1[_XPLATSTR("hahah")] = json::value(true);
        VERIFY_ARE_EQUAL(2u, val1.size());
        VERIFY_ARE_EQUAL(_XPLATSTR("44"), val1[_XPLATSTR("A")].serialize());
        VERIFY_ARE_EQUAL(_XPLATSTR("true"), val1[_XPLATSTR("hahah")].serialize());

        // Construct as null value, then turn into object.
        json::value val2;
        VERIFY_IS_TRUE(val2.is_null());
        val2[_XPLATSTR("A")] = 44;
        val2[_XPLATSTR("hahah")] = json::value(true);
        VERIFY_ARE_EQUAL(2u, val2.size());
        VERIFY_ARE_EQUAL(_XPLATSTR("44"), val2[_XPLATSTR("A")].serialize());
        VERIFY_ARE_EQUAL(_XPLATSTR("true"), val2[_XPLATSTR("hahah")].serialize());
    }

    TEST(object_construction_keep_order)
    {
        std::vector<std::pair<string_t, json::value>> f;
        f.push_back(std::make_pair(_XPLATSTR("x"), json::value(0)));
        f.push_back(std::make_pair(_XPLATSTR("a"), json::value(1)));

        auto obj1 = json::value::object(f, /*keep_order==*/true);
        VERIFY_ARE_EQUAL(obj1.as_object().begin()->first, _XPLATSTR("x"));

        auto obj2 = json::value::object(f, /*keep_order==*/false);
        VERIFY_ARE_EQUAL(obj2.as_object().begin()->first, _XPLATSTR("a"));
    }

    TEST(object_construction_from_null_keep_order)
    {
        struct restore
        {
            ~restore() { json::keep_object_element_order(false); }
        } _;

        json::keep_object_element_order(true);

        auto val1 = json::value::null();
        val1[_XPLATSTR("B")] = 1;
        val1[_XPLATSTR("A")] = 1;
        VERIFY_ARE_EQUAL(val1.as_object().begin()->first, _XPLATSTR("B"));

        json::keep_object_element_order(false);

        auto val2 = json::value::null();
        val2[_XPLATSTR("B")] = 1;
        val2[_XPLATSTR("A")] = 1;
        VERIFY_ARE_EQUAL(val2.as_object().begin()->first, _XPLATSTR("A"));
    }

    TEST(array_construction)
    {
        // Constructor which takes a vector.
        std::vector<json::value> e;
        e.push_back(json::value(false));
        e.push_back(json::value::string(_XPLATSTR("hehe")));
        json::value arr = json::value::array(e);
        VERIFY_ARE_EQUAL(e.size(), arr.size());
        VERIFY_ARE_EQUAL(_XPLATSTR("false"), arr[0].serialize());
        arr[3] = json::value(22);
        VERIFY_ARE_NOT_EQUAL(e.size(), arr.size());
        VERIFY_ARE_EQUAL(_XPLATSTR("22"), arr[3].serialize());

        // Test empty factory and adding.
        auto arr2 = json::value::array();
        arr2[1] = json::value(false);
        arr2[0] = json::value::object();
        arr2[0][_XPLATSTR("A")] = json::value::string(_XPLATSTR("HE"));
        VERIFY_ARE_EQUAL(2u, arr2.size());
        VERIFY_ARE_EQUAL(_XPLATSTR("false"), arr2[1].serialize());
        VERIFY_ARE_EQUAL(_XPLATSTR("\"HE\""), arr2[0][_XPLATSTR("A")].serialize());

        // Construct as null value and then add elements.
        json::value arr3;
        VERIFY_IS_TRUE(arr3.is_null());
        arr3[1] = json::value(false);
        // Element [0] should already behave as an object.
        arr3[0][_XPLATSTR("A")] = json::value::string(_XPLATSTR("HE"));
        VERIFY_ARE_EQUAL(2u, arr3.size());
        VERIFY_ARE_EQUAL(_XPLATSTR("false"), arr3[1].serialize());
        VERIFY_ARE_EQUAL(_XPLATSTR("\"HE\""), arr3[0][_XPLATSTR("A")].serialize());

        // Test factory which takes a size.
        auto arr4 = json::value::array(2);
        VERIFY_IS_TRUE(arr4[0].is_null());
        VERIFY_IS_TRUE(arr4[1].is_null());
        arr4[2] = json::value(true);
        arr4[0] = json::value(false);
        VERIFY_ARE_EQUAL(_XPLATSTR("false"), arr4[0].serialize());
        VERIFY_ARE_EQUAL(_XPLATSTR("true"), arr4[2].serialize());
    }

    TEST(array_test)
    {
        json::value arr = json::value::array();
        const json::value& carr = arr;
        arr[0] = json::value(3.14);
        arr[1] = json::value(true);
        arr[2] = json::value("Yes");
        int count;
        json::array& array = arr.as_array();
        const json::array& carray = arr.as_array();

        VERIFY_THROWS(array.at(4), json::json_exception);
        VERIFY_THROWS(carray.at(5), json::json_exception);
        VERIFY_THROWS(arr.at(6), json::json_exception);
        VERIFY_THROWS(carr.at(7), json::json_exception);

        // The begin and end iterators on non-const instances
        count = 0;
        for (auto iter = array.begin(); iter != array.end(); ++iter)
        {
            VERIFY_IS_TRUE((*iter) == array[count]);
            VERIFY_IS_TRUE((*iter) == array.at(count));
            VERIFY_IS_TRUE((*iter) == carray.at(count));
            count++;
        }
        VERIFY_ARE_EQUAL(array.size(), count);

        count = 0;
        for (auto iter = array.cbegin(); iter != array.cend(); ++iter)
        {
            VERIFY_IS_TRUE((*iter) == array[count]);
            VERIFY_IS_TRUE((*iter) == array.at(count));
            count++;
        }
        VERIFY_ARE_EQUAL(array.size(), count);

        count = 0;
        for (auto iter = array.rbegin(); iter != array.rend(); ++iter)
        {
            VERIFY_IS_TRUE((*iter) == array[array.size() - 1 - count]);
            VERIFY_IS_TRUE((*iter) == array.at(array.size() - 1 - count));
            VERIFY_IS_TRUE((*iter) == carray.at(array.size() - 1 - count));
            count++;
        }
        VERIFY_ARE_EQUAL(array.size(), count);

        count = 0;
        for (auto iter = array.crbegin(); iter != array.crend(); ++iter)
        {
            VERIFY_IS_TRUE((*iter) == array[array.size() - 1 - count]);
            VERIFY_IS_TRUE((*iter) == arr[array.size() - 1 - count]);
            VERIFY_IS_TRUE((*iter) == array.at(array.size() - 1 - count));
            VERIFY_IS_TRUE((*iter) == arr.at(array.size() - 1 - count));
            count++;
        }
        VERIFY_ARE_EQUAL(array.size(), count);

        // The begin and end iterators on const instances
        count = 0;
        for (auto iter = carray.begin(); iter != carray.end(); ++iter)
        {
            VERIFY_IS_TRUE((*iter) == carray.at(count));
            VERIFY_IS_TRUE((*iter) == carr.at(count));
            count++;
        }
        VERIFY_ARE_EQUAL(array.size(), count);

        count = 0;
        for (auto iter = carray.rbegin(); iter != carray.rend(); ++iter)
        {
            VERIFY_IS_TRUE((*iter) == carray.at(array.size() - 1 - count));
            VERIFY_IS_TRUE((*iter) == carr.at(array.size() - 1 - count));
            count++;
        }
        VERIFY_ARE_EQUAL(array.size(), count);
    }

    TEST(object_test)
    {
        json::value obj = json::value::object();
        const json::value& cobj = obj;
        json::object& object = obj.as_object();
        const json::object& cobject = obj.as_object();

        VERIFY_IS_TRUE(object.empty());

        obj[_XPLATSTR("name")] = json::value(_XPLATSTR("John"));
        obj[_XPLATSTR("surname")] = json::value(_XPLATSTR("Smith"));
        obj[_XPLATSTR("height")] = json::value(5.9);
        obj[_XPLATSTR("vegetarian")] = json::value(true);
        int count;

        // Test at()
        VERIFY_ARE_EQUAL(_XPLATSTR("John"), obj.at(_XPLATSTR("name")).as_string());
        VERIFY_ARE_EQUAL(_XPLATSTR("John"), cobj.at(_XPLATSTR("name")).as_string());
        VERIFY_ARE_EQUAL(_XPLATSTR("Smith"), object.at(_XPLATSTR("surname")).as_string());
        VERIFY_ARE_EQUAL(_XPLATSTR("Smith"), cobject.at(_XPLATSTR("surname")).as_string());
        VERIFY_THROWS(obj.at(_XPLATSTR("wrong key")), json::json_exception);
        VERIFY_THROWS(cobj.at(_XPLATSTR("wrong key")), json::json_exception);

        // Test find()
        {
            auto iter = object.find(_XPLATSTR("height"));
            VERIFY_ARE_NOT_EQUAL(object.end(), iter);
            VERIFY_IS_TRUE(iter->second.is_number());
            VERIFY_ARE_EQUAL(5.9, iter->second.as_number().to_double());
            VERIFY_ARE_EQUAL(object.end(), object.find(_XPLATSTR("wrong_key")));
        }

        // Test find() const
        auto citer = cobject.find(_XPLATSTR("height"));
        VERIFY_ARE_NOT_EQUAL(cobject.end(), citer);
        VERIFY_IS_TRUE(citer->second.is_number());
        VERIFY_ARE_EQUAL(5.9, citer->second.as_number().to_double());
        VERIFY_ARE_EQUAL(cobject.end(), cobject.find(_XPLATSTR("wrong_key")));

        VERIFY_IS_FALSE(object.empty());

        // The begin and end iterators on non-const instances
        count = 0;
        for (auto iter = object.begin(); iter != object.end(); ++iter)
        {
            VERIFY_ARE_EQUAL(object[iter->first], iter->second);
            count++;
        }
        VERIFY_ARE_EQUAL(object.size(), count);

        count = 0;
        for (auto iter = object.rbegin(); iter != object.rend(); ++iter)
        {
            VERIFY_ARE_EQUAL(object[iter->first], iter->second);
            count++;
        }
        VERIFY_ARE_EQUAL(object.size(), count);

        count = 0;
        for (auto iter = object.cbegin(); iter != object.cend(); ++iter)
        {
            VERIFY_ARE_EQUAL(object[iter->first], iter->second);
            count++;
        }
        VERIFY_ARE_EQUAL(object.size(), count);

        count = 0;
        for (auto iter = object.crbegin(); iter != object.crend(); ++iter)
        {
            VERIFY_ARE_EQUAL(object[iter->first], iter->second);
            count++;
        }
        VERIFY_ARE_EQUAL(object.size(), count);

        // The begin and end iterators on const instances
        count = 0;
        for (auto iter = cobject.begin(); iter != cobject.end(); ++iter)
        {
            VERIFY_ARE_EQUAL(cobject.find(iter->first)->second, iter->second);
            count++;
        }
        VERIFY_ARE_EQUAL(cobject.size(), count);

        count = 0;
        for (auto iter = cobject.rbegin(); iter != cobject.rend(); ++iter)
        {
            VERIFY_ARE_EQUAL(cobject.find(iter->first)->second, iter->second);
            count++;
        }
        VERIFY_ARE_EQUAL(cobject.size(), count);
    }

    TEST(github_asan_989)
    {
        ::web::json::value::parse(_XPLATSTR(R"([ { "k1" : "v" }, { "k2" : "v" }, { "k3" : "v" }, { "k4" : "v" } ])"));
    }

} // SUITE(construction_tests)

} // namespace json_tests
} // namespace functional
} // namespace tests
