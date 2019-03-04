/*
** EPITECH PROJECT, 2018
** api
** File description:
** MutableObject.hpp
*/

#pragma once

#include <typeinfo>
#include <memory>
#include <exception>
#include <utility>

namespace zany {

class Property {
public:
	class Exception: public std::exception {
	public:
		~Exception() = default;
		virtual const char* what() const noexcept final
			{ return "Bad Property cast !"; }
	};

	Property() = default;
	Property(Property const &) = default;
	Property(Property &&) = default;

	Property &operator=(Property const &) = default;

	~Property() = default;

	/*
	** Cree un Property, call le constructeur de T avec les arguments passés
	*/
	template<typename _T, typename ..._Args>
	static inline Property make(_Args &&...__args) {
		return Property(new _Data<_T>(std::forward<_Args>(__args)...));
	}

	/*
	** Set une nouvelle donnée;
	*/
	template<typename _T, typename ..._Args>
	inline _T &set(_Args &&...__args) {
		_bdata = decltype(_bdata)(new _Data<_T>(std::forward<_Args>(__args)...));
		return get<_T>();
	}

	/*
	** Get le typeid de la donnée
	*/
	inline const auto	&type() const { return _bdata->typeID; }

	/*
	** Get la valeur en fonction de T
	**  throw si typeid(T) de correspond pas a la donnée;
	*/
	template<typename _T> const _T	&get() const {
		if (type() != typeid(_T)) {
			throw Exception();
		}
		return static_cast<_Data<_T>*>(_bdata.get())->data;
	}

	/*
	** Get la valeur en fonction de T
	**  throw si typeid(T) de correspond pas a la donnée;
	*/
	template<typename _T> _T		&get() {
		if (type() != typeid(_T)) {
			throw Exception();
		}
		return static_cast<_Data<_T>*>(_bdata.get())->data;
	}
private:
	struct _BaseData {
		template<typename _Arg> _BaseData(_Arg &&__typeID): typeID(__typeID) {}
		virtual ~_BaseData() = default;

		const std::type_info&	typeID;
	};
	template<typename _T> struct _Data: public _BaseData {
		~_Data() = default;

		using type = typename std::remove_const<_T>::type;

		template<typename ..._Args>
		_Data(_Args &&...__args):
			_BaseData(typeid(type)),
			data(std::forward<_Args>(__args)...) {}

		type	data;
	};
	Property(_BaseData *bdata): _bdata(bdata) {}

	std::shared_ptr<_BaseData>	_bdata = nullptr;
};

} // zany
