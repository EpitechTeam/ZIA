#include <iostream>
#include <random>
#include "Zany.hpp"
#include "Zany/Property.hpp"

int	testsPipeline() {
	zany::Pipeline		master;
	zany::ThreadPool	pool(8);
	auto				&onDataEnd = master.getHookSet<zany::Pipeline::Hooks::ON_DATA_END>();

	master.linkThreadPool(pool);
	onDataEnd.addTask([] (zany::Pipeline::Instance &) {
		std::cout << "Low" << std::endl;
	});
	onDataEnd.addTask<zany::Pipeline::Priority::MEDIUM, zany::Pipeline::Rights::READ_ONLY>([] (zany::Pipeline::Instance const &) {
		std::cout << "Medium0" << std::endl;
	});
	onDataEnd.addTask<zany::Pipeline::Priority::HIGH>([] (zany::Pipeline::Instance &) {
		std::cout << "High" << std::endl;
	});

	for (int i = 1; i < 50; ++i) {
		onDataEnd.addTask<zany::Pipeline::Priority::MEDIUM, zany::Pipeline::Rights::READ_ONLY>([i] (zany::Pipeline::Instance const &) {
			std::random_device 				rd;
			std::mt19937 					gen(rd());
			std::uniform_int_distribution<> dis(1, 1000);
			//std::this_thread::sleep_for(std::chrono::milliseconds(dis(gen)));
			printf("Medium%d\n", i);
		});
	}

	// zany::Pipeline::Instance	newInstance;

	// std::make_shared<std::string>("test");
	// std::shared_ptr<zany::PropertyBase> ptr(new zany::PropertyBase(zany::Property<std::string>::make("patate")));
	// //master.executeHook<zany::Pipeline::Hooks::AFTER_ACCEPT>(newInstance);
	// zany::Pipeline::Hooks::forEach([&newInstance, &master] (auto hook) {
	// 	std::cout << "MiddleWare!" << std::endl;
	// 	master.getHookSet(hook).execute(newInstance);
	// });
	return 0;
}
