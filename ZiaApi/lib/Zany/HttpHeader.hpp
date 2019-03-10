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
	HttpHeader(std::string const &data): _data(data) {}

	template<typename T>
	HttpHeader &operator=(T &&data) { _data = std::forward<T>(data); return *this; }

	inline bool		isNumber();
	inline double	getNumber();

	inline std::string	const &getData();

	inline std::string	&operator*() { return _data; }
	inline std::string	*operator->() { return &_data; }
private:
	std::string	_data;
};

}


#include "HttpHeader.ipp"
