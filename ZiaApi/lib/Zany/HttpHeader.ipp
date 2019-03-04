//
// Created by seb on 04/02/19.
//

#pragma once

#include "HttpHeader.hpp"
#include <cstdlib>

namespace zany {

std::string	const& HttpHeader::getData() {
	return data;
}

bool HttpHeader::isNumber() {
	char	*err;

	std::strtod(data.c_str(), &err);

	return *err == '\0';
}

double HttpHeader::getNumber() {
	return std::atof(data.c_str());
}

}