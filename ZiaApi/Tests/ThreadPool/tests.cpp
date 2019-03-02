#include <iostream>
#include "Zany/ThreadPool.hpp"

int TESTNBR = 0;

int testsThreadPool() {
	zany::ThreadPool	pool(8);
	//std::mutex			mtx;

	for (auto i = 0; i < 10; ++i) {
		pool.runTask([/*i, &mtx, &pool*/] {
			for (auto j = 0; j < 1000000; ++j) {
				TESTNBR++;
			}
		});
	}
	pool.waitForEmpty();
	std::cout << "done!" << std::endl;
	return 0;
}