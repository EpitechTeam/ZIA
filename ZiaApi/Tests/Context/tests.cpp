
#include "Zany.hpp"

int testsContext() {
	zany::Context	context;

	context.addTask([&] {
		context.addTask([] {
			std::cout << '[' << std::this_thread::get_id() << ']' << " -- Test Context 2" << std::endl; 
		});
		std::cout << '[' << std::this_thread::get_id() << ']' << " -- Test Context 1" << std::endl; 
	});

	std::thread thread([&] {
		std::this_thread::yield(); // force context.run() to be performed before!
		context.addTask([&] {
			context.addTask([] {
				std::cout << '[' << std::this_thread::get_id() << ']' << " -- Test Context from thread 2 !" << std::endl; 
			});
			std::cout << '[' << std::this_thread::get_id() << ']' << " -- Test Context from thread 1 !" << std::endl; 
		});
		context.waitUntilEmpty();
		context.stop();
	});
	context.run();
	thread.join();
	return 0;
}