/*
** EPITECH PROJECT, 2018
** api
** File description:
** Pipeline.ipp
*/

#pragma once

#include <memory>
#include "./Context.hpp"
#include "Pipeline.hpp"

namespace zany {

template<>
struct Pipeline::Set::_FunctionTypeSelector<Pipeline::Rights::READ_ONLY> {
	using type = std::function<void(Pipeline::Instance const &)>;
};
template<>
struct Pipeline::Set::_FunctionTypeSelector<Pipeline::Rights::READ_WRITE> {
	using type = std::function<void(Pipeline::Instance &)>;
};

namespace __hidden {
template<Pipeline::Priority P, Pipeline::Rights R>
struct _PRSign {
	static constexpr std::uint16_t	value = ((std::uint16_t) P << 8) | (std::uint16_t) R;
};
}

template<Pipeline::Priority P, Pipeline::Rights R>
inline Pipeline::Set::ID	Pipeline::Set::addTask(typename _FunctionTypeSelector<R>::type const &fct) {
	std::lock_guard<decltype(_mtx)>	_guard(_mtx);
	auto newId = _genId();

	auto *hdl = new (typename _FunctionTypeSelector<R>::type)(fct);
	_handlers[__hidden::_PRSign<P, R>::value].insert(std::make_pair(newId, std::shared_ptr<void>(hdl)));
	return { this, __hidden::_PRSign<P, R>::value, newId };
}

void	Pipeline::Set::removeTask(ID id) {
	auto it = _handlers.find(id.PRSignValue);
	if (it == _handlers.end())
		return;
	auto &handlerMap = it->second;
	auto itH = handlerMap.find(id.uniqueId);
	if (itH == handlerMap.end()) {
		return;
	} else {
		handlerMap.erase(itH);
		if (handlerMap.empty())
			_handlers.erase(it);
	}
}

template<auto D>
template<typename Hdl>
void	Pipeline::Hooks::HookMetaIterate<D>::callSet(Hdl &&hdl) {
	hdl(D);
}

template<auto D, auto ...Ds>
template<typename ...Args>
void	Pipeline::Hooks::HookMetaIterate<D, Ds...>::callSet(Args &&...args) {
	HookMetaIterate<D>::callSet(args...);
	HookMetaIterate<Ds...>::callSet(args...);
}

template<Pipeline::Hooks::Decl H>
Pipeline::Set	&Pipeline::getHookSet() {
	return getHookSet(H);
}

Pipeline::Set	&Pipeline::getHookSet(Pipeline::Hooks::Decl hook) {
	auto it = _sets.find(hook);
	if (it == _sets.end()) {
		throw std::runtime_error("Bad Hook !");
	} else {
		return *(it->second);
	}
}

void	Pipeline::Set::execute(Pipeline::Instance &pipeline) {
	for (auto &group: _handlers) {
		auto r = static_cast<Rights>(group.first & 0xFF);
		if (r == Rights::READ_ONLY) {
			std::condition_variable			isDone;
			std::mutex						mtx;
			std::unique_lock<decltype(mtx)>	lock(mtx);
			std::atomic<std::uint32_t>		runCounter(group.second.size());

			for (auto &hdl: group.second) {
				_parent.getThreadPool().runTask([&] {
					reinterpret_cast<typename _FunctionTypeSelector<Rights::READ_ONLY>::type*>(hdl.second.get())->operator()(pipeline);
					--runCounter;
					if (runCounter.load() <= 0)
						isDone.notify_all();
				});
			}
			while (runCounter.load() > 0)
				isDone.wait(lock);
			std::this_thread::yield(); //wait for all thread
		} else {
			for (auto &hdl: group.second) {
				reinterpret_cast<typename _FunctionTypeSelector<Rights::READ_WRITE>::type*>(hdl.second.get())->operator()(pipeline);
			}
		}
	}
}

}