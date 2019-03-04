#include "Zany.hpp"
#include <iostream>
#include <cassert>

int testsEntity() {
	zany::Entity	obj = zany::makeObject {
		{"key", "value"},
		{"key2", zany::makeObject {{"obj2", 32}}},
		{"key3", 78.98}
	};

	zany::Entity	obj2 = obj["key2"]; // obj2 == {"obj2": 32}
	obj2["obj2"] = 42; // now obj2 == 42 and obj["key2"]["obj2"] too

	assert((obj["key2"]["obj2"].value<zany::Number, double>() == 42.0));

	obj2 = obj["key2"].clone();

	obj2 = zany::makeObject { // now obj2 == {"key": "value"}, but obj["key2"] not
		{ "key", "value" }
	};

	assert(obj["key2"].isObject());
	return 0;
}