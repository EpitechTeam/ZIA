/*
** EPITECH PROJECT, 2018
** api
** File description:
** Loader.hpp
*/

#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <tuple>
#include <array>
#include <iterator>
#include "./Pipeline.hpp"
#include "./Entity.hpp"
#include "./Platform.hpp"
#include "./Connection.hpp"

#if defined(ZANY_ISUNIX)
# include <dlfcn.h>
#else
# include <windows.h>
#endif

namespace zany {

class Orchestrator;

class Loader {
public:
	class Exception: public std::runtime_error {
	public:
		template<typename T> Exception(T &&message): std::runtime_error(std::forward<T>(message)) {}
	};

	using ID = std::uintptr_t;

	class AbstractModule {
	private:
		ID			_unique;
	public:
		AbstractModule(): _unique(reinterpret_cast<ID>(this)) {}
		virtual inline ~AbstractModule() = default;

		/*
		** Called after load
		*/
		virtual void		init() = 0;

		/*
		** Return true if your Module is a config parser
		*/
		virtual bool		isAParser() { return false; };

		/*
		** Parse the file,
		**	return false if it fail
		**	return a zany::Entity on success
		*/
		virtual Entity		parse(std::string const &filename) { return false; }

		/*
		** Return if is a master module (like ssl module)
		*/
		virtual bool		isACoreModule() { return false; }

		/*
		** Create a custom pipeline instance
		*/
		virtual void		coreBeginPipeline(zany::Connection &) {}

		virtual void		startListening(std::vector<std::uint16_t> &ports) {}

		/*
		** Get the module name
		*/
		virtual auto		name() const -> const std::string& = 0;

		/*
		** Get the unique id of the module
		*/
		auto				getUniqueId() const { return _unique; }
		void				linkOrchestrator(Orchestrator &p) { master = &p; }
		static inline bool	isValidParseResult(Entity const &variant);
	protected:
		class Collector {
		public:
			inline ~Collector();
			Pipeline::Set::ID	operator<<(Pipeline::Set::ID id)
				{ _handlerIDs[_handlerLen++] = id; return id; }
		private:
			std::array<Pipeline::Set::ID, Pipeline::Hooks::count()>
						_handlerIDs;
			std::size_t	_handlerLen = 0;
		};
		Collector		garbage;
		Orchestrator	*master = nullptr;
	};

	class Iterator;

	Loader() = default;
	Loader(Loader const &other) = delete;
	Loader(Loader &&other) = default;
	Loader &operator=(Loader const &other) = delete;

	/*
	** /!\ dont use this, use Orchestrator::loadModule instead !!
	** Load a module immediately
	*/
	inline AbstractModule	&load(std::string const &name);

	/*
	** /!\ dont use this, use Orchestrator::unloadModule instead !!
	** Unload a module immediately
	*/
	inline void				unload(AbstractModule const &module);
	inline void				unloadAll();
private:
	class ModuleWrapper {
	public:
		using Handler =
#		if defined(ZANY_ISUNIX)
			void *;
#		else
			HINSTANCE;
#		endif

		ModuleWrapper(Handler h, AbstractModule *am)
			: _h(h), _m(am) {}
		~ModuleWrapper() {
			delete _m;
#			if defined(ZANY_ISUNIX)
				::dlclose(_h);
#			else
				FreeLibrary(_h);
#			endif
		}
	private:
		Handler			_h;
		AbstractModule	*_m;

		friend Loader;
		friend Loader::Iterator;
	};

	std::unordered_map<ID, std::unique_ptr<ModuleWrapper>>	_modules;
public:
	class Iterator: public std::iterator<std::input_iterator_tag, struct epoll_event> {
	public:
		using Type = AbstractModule;

		Iterator() = default;
		Iterator(decltype(Loader::_modules)::iterator &&it): _current(it) {}
		Iterator(const Iterator& mit): _current(mit._current) {}
		Iterator& operator++()
			{ ++_current; return *this; }
		Iterator operator++(int)
			{ Iterator tmp(*this); operator++(); return tmp; }
		bool operator==(const Iterator &rhs) const
			{ return _current == rhs._current; }
		bool operator!=(const Iterator &rhs) const
			{ return _current != rhs._current; }
		Type &operator*() { return *_current->second->_m; }
		Type *operator->() { return _current->second->_m; }
	private:
		decltype(Loader::_modules)::iterator	_current;

		friend Loader;
	};

	/*
	**	Get the first module (Iterator)
	*/
	inline Iterator			begin() { return _modules.begin(); }

	/*
	**	Get the last module  (Iterator)
	*/
	inline Iterator			end() { return _modules.end(); }

	/*
	**	Get the first module
	*/
	inline Iterator::Type	&front() { return *(Iterator(_modules.begin())); }

	/*
	**	Get the number of loaded modules
	*/
	inline std::size_t		size() { return _modules.size(); }

	/*
	** /!\ dont use this, use Orchestrator::unloadModule instead !!
	** Unload a module immediately
	*/
	inline Iterator			erase(Iterator it) { return _modules.erase(it._current); } 
};

}

#include "Loader.ipp"