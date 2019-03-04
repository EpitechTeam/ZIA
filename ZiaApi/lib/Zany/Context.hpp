/*
** EPITECH PROJECT, 2018
** api
** File description:
** Context.hpp
*/

/**
 * \file Context.hpp
 * \brief Execution queue of the tasks
 * \author Benjamin Viguier
 */

#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

/** \namespace zany
 * 
 * Namespace of the project
 */
namespace zany {

/** \class InterfaceContext
 * 
 * Interface Class for managing the queue of tasks
 */
class InterfaceContext {
public:
	using Handler = std::function<void()>;

	InterfaceContext() = default;
	virtual ~InterfaceContext() = default;

	/** 
	 * Add a task
	 * 
	 * \param The function/lambda to add to the queue
	 */
	virtual void	addTask(Handler const &handler) = 0;

	/**
	 * Wait while execution queue is not empty
	 */
	virtual void	waitUntilEmpty() = 0;

	/**
	 * Wait while execution queue is empty
	 */
	virtual void	waitWhileEmpty() = 0;

	/**
	 * Start (blocking)
	 */
	virtual void	run() = 0;

	/**
	 * Stop the context
	 */
	virtual void	stop() = 0;

	/**
	 * return true if is running
	 */
	virtual bool	running() = 0;

	/**
	 * return number of waiting tasks;
	 */
	virtual std::size_t	queueSize() = 0;
};

/** \brief One implementation of context
 * 
 * You can use your own if you want, don't forget to make it thread safe like this one
 */
class Context : public InterfaceContext {
public:
	/**
	 * Default constructor
	 */
	Context() = default;
	/**
	 * No copy by parenthesis
	 */
	Context(Context const &other) = delete;
	/**
	 * Default move constructor
	 */
	Context(Context &&other) = default;
	/**
	 * No copy by equal sign
	 */
	Context &operator=(Context const &other) = delete;

	inline ~Context();

	virtual inline void	addTask(Handler const &handler) final;
	virtual inline void	waitUntilEmpty() final;
	virtual inline void	waitWhileEmpty() final;
	virtual inline void	run() final;
	virtual inline void	stop() final;
	virtual inline bool	running() final { return _running; };
	virtual inline auto	queueSize() -> std::size_t final;
private:
	std::mutex				_hdlsLock;		/**< Mutex for thread safe */
	std::queue<Handler>		_hdls; 				/**< Queue of tasks */
	std::condition_variable	_notLongerEmptyNotifier;		/**< ??? */
	std::condition_variable	_emptyNotifier;				/**< ??? */
	bool					_continue = false;	/**< Boolean to stop the queue after it finishes */
	bool					_running = false;
};

}

#include "Context.ipp"