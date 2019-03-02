#include <iostream>
#include "Zany.hpp"

int testsEvents() {
	auto &m = zany::evt::Manager::get();

	m["test"]->addHandler<void, int>([] (auto a) {
		std::cout << a << std::endl;
	});

	for (auto i = 0; i < 32; ++i) {
		m["test"]->addHandler<int>([i] () {
			return i;
		});
	}

	m["test"]->fire(32);
	auto v = m["test"]->fire<int>();
	for (auto i: *v) {
		std::cout << i << " ";
	}
	std::cout << std::endl;
	return 0;
}