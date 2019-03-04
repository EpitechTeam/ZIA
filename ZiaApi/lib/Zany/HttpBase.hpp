//
// Created by seb on 30/01/19.
//

#pragma once

#include <string>
#include <unordered_map>
#include "HttpHeader.hpp"

namespace zany {

struct HttpBaseRequest {
	std::string									protocol;
	std::string									protocolVersion;
	std::unordered_map<std::string, HttpHeader>	headers;
};

struct HttpRequest: public HttpBaseRequest {
	enum class RequestMethods: std::uint8_t {
#		if defined(DELETE)
#		 undef	DELETE
#		endif /* DELETE */
#		if defined(ERROR)
#		 undef	ERROR
#		endif /* ERROR */
		ERROR = 0,
		GET = 42,
		HEAD,
		POST,
		PUT,
		DELETE,
		TRACE,
		OPTIONS,
		CONNECT,
		PATCH
	};

	RequestMethods									method;
	std::string										methodString;
	std::string										host;
	std::uint16_t									port;
	std::string										path;
	std::string										fullQuery;
	std::unordered_map<std::string, std::string>	query;
};

struct HttpResponse: public HttpBaseRequest {
	unsigned int 	status = 200;
	bool			done = false;
};

}
