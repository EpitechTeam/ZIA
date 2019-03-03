#include <iostream>
#include <fstream>
#include <cassert>
#include "Zany.hpp"

void testsParser(zany::Loader::AbstractModule &module) {
	{
		std::ofstream configFile("/tmp/config");

		configFile
			<< "option1:attr1\n"
			<< "option3:attr3\n"
			<< "option2:attr2\n"
			<< "option5:attr5\n"
			<< "option4:attr4\n"
				<< std::endl;
	}
	if (module.isAParser()) { 
		auto value = module.parse("/tmp/config");
		if (module.isValidParseResult(value)) {
			std::cout << value["option3"].value<zany::String>() << std::endl;
			assert((value["option3"].value<zany::String>() == "attr3"));
		}
	}
}

int testsLoader(zany::Orchestrator &master) {
	zany::Loader	&l = const_cast<zany::Loader&>(master.getLoader());

	std::string		libPath[2];

	if constexpr (zany::isUnix) {
		libPath[0] = "build-Linux/libSimpleModule.so";
		libPath[1] = "build-Linux/libSimpleParserModule.so";
	} else {
		libPath[0] = "SimpleModule.dll";
		libPath[1] = "SimpleParserModule.dll";
	}

	auto &module = l.load(libPath[0]);
	auto &moduleParser = l.load(libPath[1]);

	module.linkOrchestrator(master);
	module.init();

	moduleParser.linkOrchestrator(master);
	moduleParser.init();

	std::cout << "iterate: Start" << std::endl;
	for (auto &m: l) {
		std::cout << "module: " << m.name() << std::endl;
	}
	std::cout << "iterate: End" << std::endl;

	zany::ThreadPool	pool(8);

	master.linkThreadPool(pool);

	testsParser(moduleParser);

	// zany::Pipeline::Instance newInstance;
	// zany::Pipeline::Hooks::forEach([&newInstance, &pipelineMaster] (auto hook) {
	// 	pipelineMaster.getHookSet(hook).execute(newInstance);
	// });

	l.unloadAll();
	return 0;
}