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

#include <list>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>


namespace zany { namespace evt {

class Event
{
public:
	template <typename T>
	struct FctSign {
		using type = T;
		using ret = std::shared_ptr<std::vector<T>>;
	};
	using EvtHdlDestr = std::function<void (void)>;

	struct EvtHdlBlockDestr {
		EvtHdlDestr	destructor;
	};

	template <typename Ret, typename ...Args>
	struct EvtHdlBlock : public EvtHdlBlockDestr {
		std::function<Ret (Args...)>	hdl;
	};

	inline explicit Event(std::function<void ()> const &onEmpty_ = nullptr);
	
	inline ~Event();

	template <typename Ret = void, typename ...Args>
	inline const EvtHdlDestr
	&addHandler(typename FctSign<std::function<Ret(Args...)>>::type const &hdl);

	template <typename Ret, typename ...Args>
	inline typename std::enable_if<(! std::is_same<Ret, void>::value), typename FctSign<Ret>::ret>::type
	fire(Args... args);

	template <typename Ret, typename ...Args>
	inline typename std::enable_if<std::is_same<Ret, void>::value, void>::type
	fire(Args... args);

	template <typename ...Args>
	inline void	fire(Args... args);
	inline bool	isEmpty(void) const;
#if !defined(NDEBUG)
	inline void	dump() const;
#endif
public:
	std::function<void ()>	onEmpty;
private:
	inline void	_addToList(	std::string const &key,
				Event::EvtHdlBlockDestr *blk);
private:
	template <typename Ret, typename ...Args>
	inline std::string	_extractTemplate();

	std::unordered_map <
		std::string,
		std::list<evt::Event::EvtHdlBlockDestr*>
	> _hdls;
};

}}

#include "Event.ipp"