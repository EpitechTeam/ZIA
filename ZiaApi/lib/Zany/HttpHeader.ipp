//
// Created by seb on 04/02/19.
//

#pragma once

#include "HttpHeader.hpp"
#include <cstdlib>

namespace zany {

std::string	const& HttpHeader::getData() {
	return _data;
}

bool HttpHeader::isNumber() {
	char	*err;

	if (_data.empty()) return false;
	std::strtod(_data.c_str(), &err);

	return *err == '\0';
}

double HttpHeader::getNumber() {
	return std::atof(_data.c_str());
}

}