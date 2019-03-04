#include <iostream>
#include <cassert>
#include "Zany/Orchestrator.hpp"

class ImplOrch : public zany::Orchestrator {
public:
	ImplOrch(zany::Context &ctx): zany::Orchestrator(ctx) {
		test = &_pline.createInstance();

		test->properties["patate"] = zany::Property::make<std::string>("Patate");
	}

	virtual void routine() final {
		std::cout << ++counter << " ";

		if (counter > 32) {
			_ctx.waitUntilEmpty();
			_ctx.stop();
			std::cout << std::endl;
			std::cout << test->properties["patate"].get<std::string>() << std::endl;
			test->properties["patate"].set<int>(32);
			assert((test->properties["patate"].type() == typeid(int)));
			try {
				test->properties["patate"].get<std::string>();
			} catch (std::exception const &e) {
				std::cout << e.what() << std::endl;
			}
			std::cout << test->properties["patate"].get<int>() << std::endl;

			class Test {
			public:
				Test(int nbr): _ptr(new int(nbr)) {}
				~Test() { delete _ptr; }

				int	&operator*() { return *_ptr; }
			private:
				int	*_ptr;
			};
			test->properties["test"] = zany::Property::make<Test>(46);
			std::cout << *(test->properties["test"].get<Test>()) << std::endl;
		}
	}
private:
	int 						counter = 0;
	zany::Pipeline::Instance	*test; //Property Test !
};

int testsLoader(zany::Orchestrator &);

int testsOrchestrator() {
	zany::Context		ctx;
	ImplOrch			m(ctx);
	zany::ThreadPool	tp(8);

	m.linkThreadPool(tp);

	testsLoader(m);

	ctx.run();
	return 0;
}