//
// Created by seb on 04/02/19.
//

#pragma once
#include <regex>
#include <iostream>

namespace zany {

class HttpHeader {
public:
	HttpHeader() = default;
	HttpHeader(std::string const &data): data(data) {}

	inline bool		isNumber();
	inline double	getNumber();

	inline std::string	const &getData();

	inline std::string	&operator*() { return data; }
	inline std::string	*operator->() { return &data; }
private:
	std::string	data;
};

}


#include "HttpHeader.ipp"
