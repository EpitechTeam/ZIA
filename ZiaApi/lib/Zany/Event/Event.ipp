/*
** EPITECH PROJECT, 2018
** api
** File description:
** Event.ipp
*/

#pragma once

#include <sstream>
#include <typeinfo>
#include <typeindex>
#include "Event.hpp"
#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING

#if !defined(NDEBUG)
 #include <iostream>
#endif

namespace zany { namespace evt {

template <typename Ret, typename ...Args>
inline std::string	Event::_extractTemplate() {
	std::ostringstream	stm;
	bool			first = false;

	stm << '(';
	for (auto &typeIdx: std::vector<std::type_index> {typeid(Args)...}) {
		if (first)
			stm << ", ";
		else
			first = true;
		stm << typeIdx.name();
	}
	stm << ") -> " << typeid(Ret).name();
	return stm.str();
}


template <typename Ret, typename ...Args>
const Event::EvtHdlDestr
&Event::addHandler(typename FctSign<std::function<Ret(Args...)>>::type const &hdl) {
	EvtHdlBlock<Ret, Args...>	*blk = new EvtHdlBlock<Ret, Args...>();
	static std::string		tpl = _extractTemplate<Ret, Args...>();

	blk->hdl = hdl;
	_addToList(tpl, blk);
	return blk->destructor;
}

template <typename Ret, typename ...Args>
typename std::enable_if<(! std::is_same<Ret, void>::value), typename Event::FctSign<Ret>::ret>::type
Event::fire(Args... args) {
	static std::string	tpl = _extractTemplate<Ret, Args...>();
	int					i = 0;
	auto				rList = _hdls.find(tpl);
	
	if (rList == _hdls.end())
		return std::make_shared<std::vector<Ret>>();

	auto	ret = std::make_shared<std::vector<Ret>>();

	ret->reserve(rList->second.size());
	auto it = rList->second.begin();
	while (it != rList->second.end()) {
		auto *ptr = *it;
		
		++it;
		ret->push_back(reinterpret_cast<EvtHdlBlock<Ret, Args...>*>(ptr)->hdl(args...));
		++i;
	}
	return ret;
}

template <typename Ret, typename ...Args>
typename std::enable_if<std::is_same<Ret, void>::value, void>::type
Event::fire(Args... args) {
	static std::string	tpl = _extractTemplate<void, Args...>();
	auto				rList = _hdls.find(tpl);
	
	if (rList == _hdls.end())
		return;

	auto it = rList->second.begin();
	while (it != rList->second.end()) {
		auto *ptr = *it;
		
		++it;
		reinterpret_cast<EvtHdlBlock<void, Args...>*>(ptr)->hdl(args...);
	}
}

template <typename ...Args>
void	Event::fire(Args... args) {
	fire<void, Args...>(args...);
}

Event::Event(std::function<void ()> const &onEmpty_):
	onEmpty(onEmpty_) {}

#if !defined(NDEBUG)
void	Event::dump() const {
	std::cout << "{" << std::endl;
	for (auto &evtType : _hdls) {
		std::cout << "\t[\"" << evtType.first.c_str() << "\"] = " <<
			evtType.second.size() << ',' << std::endl;
	}
	std::cout << "}" << std::endl;
}
#endif

bool	Event::isEmpty(void) const {
	return _hdls.empty();
}

void	Event::_addToList(std::string const &tpl, Event::EvtHdlBlockDestr *blk) {
	_hdls[tpl].push_back(blk);
	auto it = _hdls[tpl].end();
	--it;
	blk->destructor = [this, &tpl, it, blk] {
		_hdls[tpl].erase(it);
		if (_hdls[tpl].empty())
			_hdls.erase(tpl);
		if (onEmpty != nullptr && _hdls.empty())
			onEmpty();
		delete blk;
	};
}

Event::~Event() {
	for (auto &evtType : _hdls) {
		for (auto ptr : evtType.second) {
			delete ptr;
		}
		evtType.second.clear();
	}
}

}}