/*
** EPITECH PROJECT, 2018
** api
** File description:
** Pipeline.hpp
*/

/**
 * \file Pipeline.hpp
 * \brief Execution queue of the tasks
 * \author Benjamin Viguier
 */

#pragma once

#include <type_traits>
#include <unordered_map>
#include <exception>
#include <array>
#include "./Property.hpp"
#include "./ThreadPool.hpp"
#include "./HttpBase.hpp"
#include "./Context.hpp"
#include "./Entity.hpp"
#include "./Connection.hpp"

/** \namespace zany
 * 
 * Namespace of the project
 */
namespace zany {

/** \class Pipeline
 * 
 * Class handling the hooks to plug the modules functions to the Orchestrator
 * Try to see it as a manager of sets of handlers, the execution is managed in the Instance Class
 */
class Pipeline {
public:

	/** \class Hooks
	 * 
	 * Hooks enum
	 */
	class Hooks {
	private:
		template<auto ...Ds>
		struct HookMetaIterate {};
		
		template<auto D>
		struct HookMetaIterate<D> {
			template<typename Hdl>
			static inline void	callSet(Hdl &&);

			static inline constexpr std::size_t	count()
				{ return 1; }

			static inline constexpr decltype(D)	next(decltype(D) hook)
				{ return HookMetaIterate<D>::getHook(); }

			static inline constexpr decltype(D)	getHook()
				{ return D; }
		};
		template<auto D, auto ...Ds>
		struct HookMetaIterate<D, Ds...> {
			template<typename ...Args>
			static inline void	callSet(Args &&...);

			static inline constexpr std::size_t	count()
				{ return HookMetaIterate<Ds...>::count() + 1; }
			
			static inline constexpr decltype(D)	next(decltype(D) hook)
				{ return (hook == D)
					? HookMetaIterate<Ds...>::getHook()
					: HookMetaIterate<Ds...>::next(hook); }

			static inline constexpr decltype(D)	getHook()
				{ return D; }
		};
	public:
#		define HOOKSFACTORY(...) \
			enum Decl { \
				__BEGIN__, \
				__VA_ARGS__, \
				__END__ \
			}; \
			template<typename ...Args> \
			static inline void forEach(Args &&...args) \
				{ HookMetaIterate<__VA_ARGS__>::callSet(args...); } \
			static inline constexpr std::size_t	count() \
				{ return HookMetaIterate<__VA_ARGS__>::count(); } \
			static inline constexpr Decl	next(Decl hook) \
				{ return HookMetaIterate<__BEGIN__, __VA_ARGS__, __END__>::next(hook); }
		
		HOOKSFACTORY(
			BEFORE_HANDLE_REQUEST,	/**< Hooks intervening before every request */
			ON_HANDLE_REQUEST,	/**< Hooks intervening just after a request (e.g. TLS decryption) */
			ON_DATA_READY,		/**< Hooks intervening to interpret the request */
			BEFORE_HANDLE_RESPONSE,	/**< Hooks intervening when you create the data you need for the response */
			ON_HANDLE_RESPONSE,	/**< Hooks intervening when you format the data of the response (e.g. HTTP formating) */
			BEFORE_CLOSE_SOCKET	/**< Hooks intervening to encode the response (e.g. TLS encoding) */
		)
#		undef HOOKSMAKER
	};

	/** \enum Priority
	 * 
	 * Priority of the hook
	 */
	enum class Priority: std::uint8_t {
		HIGH,
		MEDIUM,
		LOW,
	};
	/** \enum Rights
	 * 
	 * Permissions of the hook (READ_ONLY allow threading in the Pipeline())
	 */
	enum class Rights: std::uint8_t {
		READ_ONLY,
		READ_WRITE
	};

	class Instance;

	/** \class Set
	 * 
	 * A set is a group of handlers for a specific hook
	 * It sorts the handlers from their Priority()
	 * You don't need to create them, Pipeline::getHookSet() allows it
	 */
	class Set {
	public:
		struct ID {
			Set				*set;
			std::uint16_t	PRSignValue;
			std::uint64_t	uniqueId;
		};

		Set(Pipeline *parent): _parent(*parent) {}
		Set(Set const &other) = delete;
		Set(Set &&other) = default;
		Set &operator=(Set const &other) = delete;


		template<Rights R>
		struct _FunctionTypeSelector {};

		/** \fn addTask(typename _FunctionTypeSelector<R>::type const &fct)
		 * 
		 * Add an handler to the set
		 */
		template<Priority P = Priority::LOW, Rights R = Rights::READ_WRITE>
		inline ID	addTask(typename _FunctionTypeSelector<R>::type const &fct);

		/** \fn execute(Instance &pipeline)
		 * 
		 * Execute the handlers in the right order
		 * \param pipeline The instance of the execution
		*/
		inline void	execute(Instance &pipeline);

		/** \fn removeTask(ID id)
		 * 
		 * Remove an handler
		 * \param id ID of the handler returned by addTask(typename _FunctionTypeSelector<R>::type const &fct)
		*/
		inline void	removeTask(ID id);

		inline auto	&getMutex() { return _mtx; }
		inline auto	&getParent() { return _parent; }
	private:
		std::uint64_t	_genId()
			{ static std::uint64_t nextid = 0; return ++nextid; }

		std::map<
			std::uint16_t,
			std::unordered_map<
				std::uint64_t,
				std::shared_ptr<void>
			>>		_handlers;
		std::mutex	_mtx;
		Pipeline	&_parent;
	};

	Pipeline(): _pool(nullptr) {
		Hooks::forEach([this] (Hooks::Decl hook) {
			_sets.emplace(std::make_pair(hook, std::make_unique<Set>(this)));
		});
	}
	Pipeline(Pipeline const &other) = delete;
	Pipeline(Pipeline &&other) = default;
	Pipeline &operator=(Pipeline const &other) = delete;

	/** \class Instance
	 * 
	 * Class instancing a Pipeline, launching the sets in the right order, etc...
	 */
	class Instance {
	public:
		Instance() = delete;
		Instance(Connection::SharedInstance &co_): connection(co_) {};
		Instance(Instance const &other) = delete;
		Instance(Instance &&other) = default;
		Instance &operator=(Instance const &other) = delete;

		/**
		 * Allow data sharing between different hooks
		 */
		std::unordered_map<std::string, Property>	properties;

		/**
		** Request header404
		*/
		HttpRequest		request;

		/**
		** Response header
		*/
		HttpResponse	response;

		/**
		 * Connection
		 */
		Connection::SharedInstance	connection;

		Entity						serverConfig = zany::Entity::Type::NUL;

		Context						context;

		std::uintptr_t				writerID = 0;
	};
	static inline auto	makePipelineInstance(Connection::SharedInstance co) { return std::make_shared<Instance>(co); }
	inline void	linkThreadPool(ThreadPool &pool) { _pool = &pool; }
	inline auto	getThreadPool() -> auto & { return *_pool; }
	inline auto	getThreadPool() const -> const auto & { return *_pool; }

	/** \fn getHookSet()
	 * 
	 * Get the set of an hook
	 */
	template<Hooks::Decl H>
	inline Set	&getHookSet();
	inline Set	&getHookSet(Hooks::Decl hook);

	/** \fn executeHook(Instance &instance)
	 * 
	 * Execute a hook set
	*/
	template<Hooks::Decl H>
	inline void	executeHook(Instance &instance)
		{ getHookSet<H>().execute(instance); }
private:
	ThreadPool												*_pool;
	std::unordered_map<Hooks::Decl, std::unique_ptr<Set>>	_sets;
};


}

#include "Pipeline.ipp"