/*
 * libnbt++ - A library for the Minecraft Named Binary Tag format.
 * Copyright (C) 2013, 2015  ljfa-ag
 *
 * This file is part of libnbt++.
 *
 * libnbt++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libnbt++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libnbt++.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "microtest.h"
#include "libnbt.h"

using namespace nbt;

void test_get_type()
{
    ASSERT(tag_byte().get_type()       == tag_type::Byte);
    ASSERT(tag_short().get_type()      == tag_type::Short);
    ASSERT(tag_int().get_type()        == tag_type::Int);
    ASSERT(tag_long().get_type()       == tag_type::Long);
    ASSERT(tag_float().get_type()      == tag_type::Float);
    ASSERT(tag_double().get_type()     == tag_type::Double);
    //ASSERT(tag_byte_array().get_type() == tag_type::Byte_Array);
    ASSERT(tag_string().get_type()     == tag_type::String);
    ASSERT(tag_list().get_type()       == tag_type::List);
    ASSERT(tag_compound().get_type()   == tag_type::Compound);
    //ASSERT(tag_int_array().get_type()  == tag_type::Int_Array);
    std::clog << "test_get_type passed" << std::endl;
}

void test_tag_primitive()
{
    tag_int tag(6);
    ASSERT(tag.get() == 6);
    int& ref = tag;
    ref = 12;
    ASSERT(tag == 12);
    ASSERT(tag != 6);
    tag.set(24);
    ASSERT(ref == 24);
    tag = 7;
    ASSERT(7 == static_cast<int>(tag));

    ASSERT(tag_double() == 0.0);
    std::clog << "test_tag_primitive passed" << std::endl;
}

void test_tag_string()
{
    tag_string tag("foo");
    ASSERT(tag.get() == "foo");
    std::string& ref = tag;
    ref = "bar";
    ASSERT(tag.get() == "bar");
    ASSERT(tag.get() != "foo");
    tag.set("baz");
    ASSERT(ref == "baz");
    tag = "quux";
    ASSERT("quux" == static_cast<std::string>(tag));
    std::string str("foo");
    tag = str;
    ASSERT(tag.get() == str);

    ASSERT(tag_string(str).get() == "foo");
    ASSERT(tag_string().get() == "");
    std::clog << "test_tag_string passed" << std::endl;
}

void test_tag_compound()
{
    tag_compound comp{
        {"foo", int16_t(12)},
        {"bar", "baz"},
        {"baz", -2.0},
        {"list", tag_list::of<tag_byte>({16, 17})}
    };

    ASSERT(comp["foo"].get_type() == tag_type::Short);
    ASSERT(int32_t(comp["foo"]) == 12);
    ASSERT(int16_t(comp.at("foo")) == int16_t(12));
    EXPECT_EXCEPTION(int8_t(comp["foo"]), std::bad_cast);
    EXPECT_EXCEPTION(std::string(comp["foo"]), std::bad_cast);

    EXPECT_EXCEPTION(comp["foo"] = 32, std::bad_cast);
    comp["foo"] = int8_t(32);
    ASSERT(int16_t(comp["foo"]) == 32);

    ASSERT(comp["bar"].get_type() == tag_type::String);
    ASSERT(std::string(comp["bar"]) == "baz");
    EXPECT_EXCEPTION(int(comp["bar"]), std::bad_cast);

    EXPECT_EXCEPTION(comp["bar"] = -128, std::bad_cast);
    comp["bar"] = "barbaz";
    ASSERT(std::string(comp["bar"]) == "barbaz");

    ASSERT(comp["baz"].get_type() == tag_type::Double);
    ASSERT(double(comp["baz"]) == -2.0);
    EXPECT_EXCEPTION(float(comp["baz"]), std::bad_cast);

    comp["quux"] = tag_compound{{"Hello", "World"}, {"zero", 0}};
    ASSERT(comp.at("quux").get_type() == tag_type::Compound);
    ASSERT(std::string(comp["quux"].at("Hello")) == "World");
    ASSERT(std::string(comp["quux"]["Hello"]) == "World");
    ASSERT(comp["list"][1] == tag_byte(17));

    EXPECT_EXCEPTION(comp.at("nothing"), std::out_of_range);

    tag_compound comp2{
        {"foo", int16_t(32)},
        {"bar", "barbaz"},
        {"baz", -2.0},
        {"quux", tag_compound{{"Hello", "World"}, {"zero", 0}}},
        {"list", tag_list::of<tag_byte>({16, 17})}
    };
    ASSERT(comp == comp2);
    ASSERT(comp != (const tag_compound&)comp2["quux"]);
    ASSERT(comp != comp2["quux"]);

    ASSERT(comp2.size() == 5);
    const char* keys[] = {"bar", "baz", "foo", "list", "quux"}; //alphabetic order
    unsigned i = 0;
    for(const std::pair<const std::string, value>& val: comp2)
    {
        ASSERT(i < comp2.size());
        ASSERT(val.first == keys[i]);
        ASSERT(val.second == comp2[keys[i]]);
        ++i;
    }

    ASSERT(comp.erase("nothing") == false);
    ASSERT(comp.has_key("quux"));
    ASSERT(comp.erase("quux") == true);
    ASSERT(!comp.has_key("quux"));

    comp.clear();
    ASSERT(comp == tag_compound{});

    ASSERT(comp.put("abc", value(tag_double(6.0))).second == true);
    ASSERT(comp.put("abc", value(tag_long(-28))).second == false);
    ASSERT(comp.insert("ghi", value(tag_string("world"))).second == true);
    ASSERT(comp.insert("abc", value(tag_string("hello"))).second == false);
    ASSERT(comp.emplace<tag_string>("def", "ghi").second == true);
    ASSERT(comp.emplace<tag_byte>("def", 4).second == false);
    ASSERT((comp == tag_compound{
        {"abc", tag_long(-28)},
        {"def", tag_byte(4)},
        {"ghi", tag_string("world")}
    }));

    std::clog << "test_tag_compound passed" << std::endl;
}

void test_value()
{
    value val1;
    value val2(std::unique_ptr<tag>(new tag_int(42)));
    value val3(tag_int(42));

    ASSERT(!val1 && val2 && val3);
    ASSERT(val1 == val1);
    ASSERT(val1 != val2);
    ASSERT(val2 == val3);
    ASSERT(val3 == val3);

    val1 = int64_t(42);
    ASSERT(val2 != val1);
    EXPECT_EXCEPTION(val2 = int64_t(12), std::bad_cast);
    ASSERT(int64_t(val2) == 42);
    val2 = 52;
    ASSERT(int32_t(val2) == 52);

    EXPECT_EXCEPTION(val1["foo"], std::bad_cast);
    EXPECT_EXCEPTION(val1.at("foo"), std::bad_cast);

    val3 = 52;
    ASSERT(val2 == val3);
    ASSERT(val2.get_ptr() != val3.get_ptr());

    val3 = std::move(val2);
    ASSERT(val3 == tag_int(52));
    ASSERT(!val2);

    tag_int& tag = dynamic_cast<tag_int&>(val3.get());
    ASSERT(tag == tag_int(52));
    tag = 21;
    ASSERT(int32_t(val3) == 21);
    val1.set_ptr(std::move(val3.get_ptr()));
    ASSERT(tag_int(val1) == 21);

    ASSERT(val1.get_type() == tag_type::Int);
    ASSERT(val2.get_type() == tag_type::Null);
    ASSERT(val3.get_type() == tag_type::Null);

    val2 = val1;
    val1 = val3;
    ASSERT(!val1 && val2 && !val3);
    ASSERT(val1.get_ptr() == nullptr);
    ASSERT(val2.get() == tag_int(21));
    ASSERT(value(val1) == val1);
    ASSERT(value(val2) == val2);
    val1 = val1;
    val2 = val2;
    ASSERT(!val1);
    ASSERT(val2 == tag_int(21));
    std::clog << "test_value passed" << std::endl;
}

void test_tag_list()
{
    tag_list list;
    ASSERT(list.el_type() == tag_type::Null);
    EXPECT_EXCEPTION(list.push_back(value(nullptr)), std::bad_cast);

    list.emplace_back<tag_string>("foo");
    ASSERT(list.el_type() == tag_type::String);
    list.push_back(value(tag_string("bar")));
    EXPECT_EXCEPTION(list.push_back(value(tag_int(42))), std::bad_cast);
    EXPECT_EXCEPTION(list.emplace_back<tag_compound>(), std::bad_cast);

    ASSERT((list == tag_list{"foo", "bar"}));
    ASSERT(list[0] == tag_string("foo"));
    ASSERT(std::string(list.at(1)) == "bar");

    ASSERT(list.size() == 2);
    EXPECT_EXCEPTION(list.at(2), std::out_of_range);
    EXPECT_EXCEPTION(list.at(-1), std::out_of_range);

    list.set(1, value(tag_string("baz")));
    ASSERT(std::string(list[1]) == "baz");

    const char* values[] = {"foo", "baz"};
    unsigned i = 0;
    for(const value& val: list)
    {
        ASSERT(i < list.size());
        ASSERT(std::string(val) == values[i]);
        ++i;
    }

    list.pop_back();
    ASSERT(list == tag_list{"foo"});
    ASSERT(list == tag_list::of<tag_string>({"foo"}));
    ASSERT(tag_list::of<tag_string>({"foo"}) == tag_list{"foo"});
    ASSERT((list != tag_list{2, 3, 5, 7}));

    list.clear();
    ASSERT(list.size() == 0);
    EXPECT_EXCEPTION(list.push_back(value(tag_short(25))), std::bad_cast);
    EXPECT_EXCEPTION(list.push_back(value(nullptr)), std::bad_cast);

    ASSERT(tag_list() == tag_list(tag_type::Int));
    ASSERT(tag_list(tag_type::Short) == tag_list(tag_type::Int));

    tag_list short_list = tag_list::of<tag_short>({25, 36});
    ASSERT(short_list.el_type() == tag_type::Short);
    ASSERT((short_list == tag_list{int16_t(25), int16_t(36)}));
    ASSERT((short_list != tag_list{25, 36}));
    ASSERT((short_list == tag_list{value(tag_short(25)), value(tag_short(36))}));

    EXPECT_EXCEPTION((tag_list{value(tag_byte(4)), value(tag_int(5))}), std::bad_cast);
    EXPECT_EXCEPTION((tag_list{value(nullptr), value(tag_int(6))}), std::bad_cast);
    EXPECT_EXCEPTION((tag_list{value(tag_int(7)), value(tag_int(8)), value(nullptr)}), std::bad_cast);
    ASSERT((tag_list(std::initializer_list<value>{})).el_type() == tag_type::Null);
    ASSERT((tag_list{2, 3, 5, 7}).el_type() == tag_type::Int);
    std::clog << "test_tag_list passed" << std::endl;
}

int main()
{
    test_get_type();
    test_tag_primitive();
    test_tag_string();
    test_tag_compound();
    test_value();
    test_tag_list();
}
