/*
** EPITECH PROJECT, 2018
** api
** File description:
** Context.ipp
*/

#pragma once

#include <thread>
#include "Context.hpp"

namespace zany {

Context::~Context() {
	this->stop();
}

void	Context::addTask(Handler const &handler) {
	bool	notify;
	{
		std::lock_guard<decltype(_hdlsLock)> _guard(_hdlsLock);
		notify = _hdls.empty();
		_hdls.push(handler);
	}
	if (notify)
		_notLongerEmptyNotifier.notify_all();
}

void	Context::waitUntilEmpty() {
	std::mutex						mtx;
	bool							haveToWait;
	std::unique_lock<decltype(mtx)>	ulock(mtx);

	{
		std::lock_guard<decltype(_hdlsLock)> _guard(_hdlsLock);
		haveToWait = !_hdls.empty();
	}
	if (haveToWait)
		_emptyNotifier.wait(ulock);
}

void	Context::waitWhileEmpty() {
	std::mutex						mtx;
	bool							haveToWait;
	std::unique_lock<decltype(mtx)>	ulock(mtx);

	{
		std::lock_guard<decltype(_hdlsLock)> _guard(_hdlsLock);
		haveToWait = _hdls.empty();
	}
	if (haveToWait)
		_notLongerEmptyNotifier.wait(ulock);
}

void	Context::run() {
	std::mutex	lock;

	_continue = true;
	_running = true;
	while (_continue) {
		std::unique_lock<decltype(lock)> ul(lock);

		while ([this]{
			std::lock_guard<decltype(_hdlsLock)> _guard(_hdlsLock);
			return _hdls.empty();
		}()) {
			_notLongerEmptyNotifier.wait(ul);
			if (_continue == false)
				goto stoploop;
		}

		Handler handler;
		{
			std::lock_guard<decltype(_hdlsLock)> _guard(_hdlsLock);
			handler = _hdls.front();
			_hdls.pop();
		}
		handler();
		
		bool isEmpty;
		{
			std::lock_guard<decltype(_hdlsLock)> _guard(_hdlsLock);
			isEmpty = _hdls.empty();
		}
		if (isEmpty) {
			_emptyNotifier.notify_all();
			std::this_thread::yield();
		}
	}
stoploop:
	_emptyNotifier.notify_all();
	std::this_thread::yield();
	_running = false;
}

void	Context::stop() {
	_continue = false;
	_notLongerEmptyNotifier.notify_all();
	_emptyNotifier.notify_all();
}

std::size_t	Context::queueSize() {
	std::lock_guard<decltype(_hdlsLock)> _guard(_hdlsLock);
	return _hdls.size();
}

}