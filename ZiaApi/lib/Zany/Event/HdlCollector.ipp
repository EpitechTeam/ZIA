/*
** EPITECH PROJECT, 2018
** api
** File description:
** HdlCollector.ipp
*/

#include "HdlCollector.hpp"

namespace zany { namespace evt {

HdlCollector::~HdlCollector()
{
	flush();
}

Event::EvtHdlDestr
HdlCollector::add(Event::EvtHdlDestr const &hdl) {
	auto it = _unregisterEventsHdls.insert(_unregisterEventsHdls.end(), hdl);
	return [this, it] () {
		auto fct = *it;
		this->_unregisterEventsHdls.erase(it);
		fct();
	};
}

HdlCollector
&HdlCollector::operator<<(Event::EvtHdlDestr const &hdl) {
	_unregisterEventsHdls.push_back(hdl);
	return *this;
}

void	HdlCollector::flush(void) {
	for (auto &destructor : _unregisterEventsHdls) {
		(destructor)();
	}
	_unregisterEventsHdls.clear();
}

}}