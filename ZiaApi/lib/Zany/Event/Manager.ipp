/*
** EPITECH PROJECT, 2018
** api
** File description:
** Manager.ipp
*/

#include <iostream>
#include <sstream>
#include <utility>
#include "Manager.hpp"

namespace zany { namespace evt {

template<typename T>
void	BasicManager<T>::removeEvent(T const &eventName) {
	_events.erase(eventName);
}

#if !defined(NDEBUG)
template<typename T>
void	BasicManager<T>::dump() const {
	for (auto event : _events) {
		std::cout << "evt[" << event.first << "]" << " = ";
		event.second.dump();
		std::cout << std::endl;
	}
}
#endif

template<typename T>
EventWrapper
BasicManager<T>::operator[](T const &eventName) {
	auto	rIt = _events.find(eventName);
	Event	*_evt;

	if (rIt == _events.end()) {
		auto it = _events.insert(std::make_pair(
			eventName,
			Event([this, eventName] {
				this->removeEvent(eventName);
			})
		));

		_evt = &(it.first->second);
	} else {
		_evt = &(rIt->second);
	}
	return EventWrapper(*_evt, [this, eventName, _evt] (void) {
		if (_events.find(eventName) != _events.end() &&
			_evt->isEmpty())
			this->removeEvent(eventName);
	});
}

template<typename T>
inline BasicManager<T>	&BasicManager<T>::get(void) {
		static BasicManager<T>	manager;
		return manager;
}

}}