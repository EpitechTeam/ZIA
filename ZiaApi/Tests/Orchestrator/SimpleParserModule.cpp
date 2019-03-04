#include <iostream>
#include <fstream>
#include <sstream>
#include "Zany.hpp"

class SimpleParserModule: public zany::Loader::AbstractModule {
public:
	SimpleParserModule() = default;
	~SimpleParserModule() { printf("SimpleParserModule Deleted!\n"); };

	virtual auto			name() const -> const std::string&
		{ static const std::string name("SimpleParserModule"); return name; }
	virtual void			init();
	virtual bool			isAParser() final { return true; }
	virtual zany::Entity	parse(std::string const &filename) final;
private:
	/* data */
};

void	SimpleParserModule::init() {
	std::cout << "SimpleParserModule init()" << std::endl;
}

zany::Entity	SimpleParserModule::parse(std::string const &filename) {
	std::ifstream	stm(filename);

	if (stm.is_open()) {
		zany::Entity	entity(zany::Entity::Type::OBJ);

		for (std::string line; std::getline(stm, line); ) {
			std::vector<std::string>	splited;
			std::istringstream			sstm(line);
			int 						i{0};

			splited.reserve(2);
			for (std::string split; std::getline(sstm, split, ':') && i < 2; ++i) {
				splited.push_back(split);
			}
			//std::cout << "djqzfjqpzf: " << splited[0]  << " : " << splited[1] << std::endl;
			if (splited.size() >= 2) {
				entity[splited[0]] = splited[1];
			}
		}
		return entity;
	} else {
		return false;
	}
}

extern "C" ZANY_DLL
zany::Loader::AbstractModule	*entrypoint() {
	return new SimpleParserModule();
}

