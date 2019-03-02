/*
** EPITECH PROJECT, 2018
** api
** File description:
** Orchestrator.hpp
*/

/**
 * \file Orchestrator.hpp
 * \brief Core of the API, master controller
 * \author Benjamin Viguier
 */

#pragma once

#include <functional>
#include <deque>
#include <mutex>
#include <atomic>
#include <type_traits>
#include <iostream>
#include <exception>
#include "./Platform.hpp"
#include "./Pipeline.hpp"
#include "./Loader.hpp"
#include "./Context.hpp"
#include "./Connection.hpp"

/** \namespace zany
 * 
 * Namespace of the project
 */
namespace zany {

/** \class Orchestrator
 * 
 * Orchestrator class for controlling the project
 */
class Orchestrator {
private:
	template<typename T>
	union _CastUtility {
		_CastUtility(void (T::*_routine)()): routine(_routine) {}
		_CastUtility(void *_ptr): ptr(_ptr) {}

		void			(T::*routine)();
		void				*ptr;
	};
public:
	template<typename T = Orchestrator, typename std::enable_if<std::is_base_of<Orchestrator, T>::value, int>::type = 0>
	inline Orchestrator(InterfaceContext &ctx, void (T::*member)() = nullptr);
	Orchestrator(Orchestrator const &other) = delete;
	Orchestrator(Orchestrator &&other) = default;
	Orchestrator &operator=(Orchestrator const &other) = delete;

	/** \fn loadModule(std::string const &filename, std::function<void(Loader::AbstractModule &)> const &callback, std::function<void(std::exception)> const &error = nullptr)
	 * 
	 * Loading a module asynchronously
	 * \param filename Path of the file module (*.so)
	 * \param callback Function to call at the end of the load
	 * \param error Function to call on load error
	 */
	inline void	loadModule(
		std::string const &filename,
		std::function<void(Loader::AbstractModule &)> const &callback,
		std::function<void(zany::Loader::Exception)> const &error = nullptr);

	/** \fn unloadModule(Loader::AbstractModule const &module, std::function<void()> const &callback)
	 * 
	 * Unloading a module asynchronously
	 * \param module Module to unload (to get from Loader (Ochestrator::getLoader()))
	 * \param callback Function to call at the end of the unload
	 */
	inline void unloadModule(
		Loader::AbstractModule const &module,
		std::function<void()> const &callback,
		std::function<void(zany::Loader::Exception)> const &error = nullptr);

	/** \fn startPipeline(zany::Socket sockFd)
	 * 
	 * Start an instance of a Pipeline after the accept of a connexion
	 */
	inline auto startPipeline(
		zany::Connection::SharedInstance co,
		std::function<void(zany::Pipeline::Instance&)> const &beforeAll = nullptr);

	/** \fn waitForSafeHandlersFinished()
	 * 
	 * Launch safe handlers execution
	 */
	inline void waitForSafeHandlersFinished();

	/*
	** Ajouter un safe handler a la queue, un safe handler est une
	**  fonction qui doit etre executé dans un context sécurisé
	**  par exemple le chargement d'un module
	*/

	/** \fn addSafeHandler(T &&function)
	 * 
	 * 
	 */
	template<typename T> inline void	addSafeHandler(T &&function);

	/** \fn zany::Entity	getConfig()
	 * 
	 *  get the main config
	 */
	virtual const zany::Entity	getConfig() const = 0;

	/** \fn void reload()
	 * 
	 * Reload the server
	 */
	virtual void	reload() { std::cerr << "Reload: Not implemented!\n"; }


	/*
	** Exectué quand une pipeline est prete, permet le debut de l'execution des hooks par le serveur (doit etre override et threadsafe)
	*/

	/*
	** Lie une threadpool au gestionnaire de pipeline
	*/
	inline void	linkThreadPool(ThreadPool &pool) { _pline.linkThreadPool(pool); }
	inline auto	&getThreadPool() { return _pline.getThreadPool(); }
	inline auto	&getThreadPool() const { return _pline.getThreadPool(); }
	inline auto	&getLoader() const { return _loader; }
	inline auto &getPipeline() { return _pline; }
	inline auto const &getPipeline() const { return _pline; }
	inline auto &getContext() { return _ctx; }
	inline auto *getCore() { return _coreModule; }
protected:
	class	PipelineExecutionError: public std::runtime_error {
	public:
		template<typename T> PipelineExecutionError(T const &message): std::runtime_error(message) {}
	};

	virtual void	onPipelineThrow(PipelineExecutionError const &exception) = 0;

	InterfaceContext	&_ctx;
	Pipeline			_pline;
	Loader				_loader;
private:
	inline void	_onPipelineReady(zany::Pipeline::Instance &);
	inline void	_routine();

	void												*__routine;
	Loader::AbstractModule								*_coreModule = nullptr;
	std::deque<std::function<void()>>					_safeHdls;
	std::mutex											_safeMtx;
	std::atomic<bool>									_safeIsComputing = false;
	std::vector<std::shared_ptr<Pipeline::Instance>>	_waitSafeConnections;
};

}

#include "Orchestrator.ipp"