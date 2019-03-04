/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <benjamin.viguier@epitech.eu> wrote this file. As long as you retain this 
 * notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in
 * return Benjamin Viguier
 * --------------------------------------------------------------------------
 */

#pragma once

#include <unordered_map>
#include <functional>
#include "./Event.hpp"

namespace zany { namespace evt {
	
class EventWrapper;

template<typename T>
class BasicManager
{
public:
public:
	explicit BasicManager() = default;

	static inline BasicManager &get(void);

	inline EventWrapper	operator[](T const &eventName);
	inline void		removeEvent(T const &eventName);
#if !defined(NDEBUG)
	inline void	dump() const;
#endif
private:
	std::unordered_map<T, Event>	_events;
};

class EventWrapper
{
public:
	explicit EventWrapper(Event &evt, std::function<void (void)> const &hdl):
		_evt(evt),
		_hdl(hdl) {};
	~EventWrapper() {
		_hdl();
	}
	Event	*operator->(void) {
		return &_evt;
	}
private:
	Event				&_evt;
	std::function<void (void)>	_hdl;
};

using Manager = BasicManager<std::string>;

}}

#include "Manager.ipp"