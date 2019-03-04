/*
** EPITECH PROJECT, 2018
** api
** File description:
** ThreadPool.ipp
*/

#pragma once


#include "Platform.hpp"
#include "ThreadPool.hpp"

#if defined(ZANY_ISUNIX)
	#include <signal.h>
#else
	#include <Windows.h>
#endif

namespace zany {

ThreadPool::ThreadPool(std::uint32_t threadNbr):
		_nbr(threadNbr), _inUse(0) {
	for (decltype(_nbr) i = 0; i < _nbr; ++i) {
		_threads.emplace_back(std::bind(&ThreadPool::_entrypoint, this));
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	_isReady = true;
	_readyNotifier.notify_all();
}

ThreadPool::~ThreadPool() {
	if (_aborted == true)
		return;
	_continue = false;
	_readyNotifier.notify_all();
	std::this_thread::yield();

	for (auto &t: _threads) {
		t.join();
	}
}

void	ThreadPool::abort() {
	_continue = false;
	_readyNotifier.notify_all();
	std::this_thread::yield();

	for (auto &t: _threads) {
		if (t.joinable()) {
#			if defined(ZANY_ISUNIX)
				pthread_kill(t.native_handle(), SIGKILL);
#			else
				TerminateThread(t.native_handle(), 0);
				CloseHandle(t.native_handle());
#			endif
		}
	}
	_aborted = true;
}

void	ThreadPool::_entrypoint() {
	std::mutex	mtx;
	{
		std::unique_lock<decltype(mtx)>	ulock(mtx);
		while (!_isReady) _readyNotifier.wait(ulock);
	}
	std::unique_lock<decltype(mtx)>	ulock(mtx);
	while (_continue) {
		Handler	hdl;
		while ((_handlers.size() == 0 && _continue)) // loop to avoid spurious wakeups
			_readyNotifier.wait(ulock);
		if (!_continue) break;
		{
			std::lock_guard<decltype(_lockPeek)>	_guard(_lockPeek);
			if (_handlers.size() != 0) {
				hdl = _handlers.front();
				_handlers.pop();
			} else {
				continue;
			}
		}
		++_inUse;
		hdl();
		--_inUse;
		{
			std::lock_guard<decltype(_lockPeek)>	_guard(_lockPeek);

			if (_handlers.size() == 0 && available() == _nbr) {
				_emptyNotifier.notify_all();
			}
		}
	}
}

void	ThreadPool::runTask(Handler const &hdl) {
	bool	notify = false;
	{
		std::lock_guard<decltype(_lockPeek)>	_guard(_lockPeek);

		if (_handlers.size() == 0) notify = true;
		_handlers.push(hdl);
	}
	if (notify)
		_readyNotifier.notify_all();
}

std::uint32_t	ThreadPool::available() {
	return _nbr - _inUse.operator unsigned int();
}

auto			ThreadPool::pending() {
	std::lock_guard<decltype(_lockPeek)>	_guard(_lockPeek);
	return _handlers.size();
}

void	ThreadPool::waitForEmpty() {
	std::mutex						mtx;
	std::unique_lock<decltype(mtx)>	ulock(mtx);
	bool 							needToWait;

retry:
	{
		std::lock_guard<decltype(_lockPeek)>	_guard(_lockPeek);

		needToWait = (_handlers.size() != 0 || available() != _nbr);
	}
	if (needToWait) {
		_emptyNotifier.wait(ulock);
		goto retry;
	}
}

}