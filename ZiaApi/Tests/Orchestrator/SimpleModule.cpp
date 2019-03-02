#include <iostream>
#include "Zany.hpp"

class SimpleModule: public zany::Loader::AbstractModule {
public:
	SimpleModule() = default;
	~SimpleModule() { printf("SimpleModule Deleted!\n"); };

	virtual void				init();
	virtual const std::string&	name() const {
		static const std::string name("SimpleModule");

		return name;
	}
private:
	/* data */
};

void	SimpleModule::init() {
	std::cout << "SimpleModule init()" << std::endl;

	garbage << master->getPipeline().getHookSet<zany::Pipeline::Hooks::ON_DATA_AVAILABLE>().addTask([] (zany::Pipeline::Instance &) {
		std::cout << "test" << std::endl;
	});
}

extern "C" ZANY_DLL
zany::Loader::AbstractModule	*entrypoint() {
	return new SimpleModule();
}

