/*
** EPITECH PROJECT, 2018
** api
** File description:
** Connection.hpp
*/

#pragma once

#include <memory>
#include <utility>
#include "./Property.hpp"

namespace zany {

class Connection: public std::enable_shared_from_this<Connection> {
public:
	Connection() = default;
	template<typename ...Ts>
	Connection(Ts &&...__info): info{std::forward<Ts>(__info)...} {}
	virtual ~Connection() = default;

	using SharedInstance = std::shared_ptr<Connection>;

	virtual std::iostream &stream() = 0;

	struct {
		std::string 	ip;
		std::uint16_t	port;
	}	info;
};

}