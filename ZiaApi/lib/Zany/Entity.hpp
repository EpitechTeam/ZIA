/*
** EPITECH PROJECT, 2018
** api
** File description:
** Entity.hpp
*/

#pragma once

#include <unordered_map>
#include <vector>
#include <ostream>
#include <istream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <functional>
#include "Platform.hpp"

#if defined(ZANY_ISWINDOWS)
 #define STRCASECMP ::_stricmp
 #define STRDUP ::_strdup
#else
 #define STRCASECMP ::strcasecmp
 #define STRDUP ::strdup
#endif

namespace zany
{

class AbstractData;
class Entity;
using ObjEntry = std::pair<const std::string, Entity>;
using makeObject = std::initializer_list<ObjEntry>;

using ArrayEntry = Entity;
using makeArray = std::initializer_list<ArrayEntry>;

class Entity
{
public:
	enum class CloneOption {
		LAZY,
		DEEP
	};
	enum class Type {
		NBR,
		STR,
		ARR,
		OBJ,
		BOL,
		NUL
	};
	enum class StringifyAttr {
		PRETTY,
		MINIFIED
	};

	using OnConflitFunc = std::function<void (std::string const &, Entity &, Entity&)>;
private:
	static inline void
		_basicOnConfit(std::string const &key, Entity &first, Entity &second);
public:
	 Entity(Entity const &) = default;
	 inline Entity(Type type = Type::NUL);
	 inline Entity(double nbr);
	 inline Entity(long nbr);
	 inline Entity(unsigned long nbr);
	 inline Entity(int nbr);
	 inline Entity(unsigned int nbr);
	 inline Entity(bool bol);
	 inline Entity(std::string const &str);
	 inline Entity(const char *s);
	 inline Entity(std::initializer_list<ArrayEntry> list);
	 inline Entity(std::initializer_list<ObjEntry> list);

	Entity	&operator=(Entity const &other) = default;
	inline bool	operator==(Entity const &other) const;
	inline bool	operator!=(Entity const &other) const;

	inline auto		operator[](std::string const &key) -> Entity&;
	inline auto		operator[](std::string const &key) const -> const Entity&;
	inline auto		operator[](unsigned idx) -> Entity&;
	inline auto		operator[](unsigned idx) const -> const Entity&;
	inline Entity	&push(Entity const &obj);
	inline void		merge(Entity const &toAdd, OnConflitFunc const &onConflit = _basicOnConfit);
	
	inline Entity	clone(CloneOption attr = CloneOption::DEEP) const;

	inline bool	isObject(void) const;
	inline bool	isArray(void) const;
	inline bool	isNumber(void) const;
	inline bool	isString(void) const;
	inline bool	isBool(void) const;
	inline bool	isNull(void) const;

	template<typename T>
	T	to() const;
	
	template<typename T>
	T	&getData() {
		return *(reinterpret_cast<T*>(_data.get()));
	}

	template<typename T>
	const T	&getData() const {
		return *(reinterpret_cast<T*>(_data.get()));
	}

	template<typename T>
	auto	&value() {
		return (reinterpret_cast<T*>(_data.get()))->get();
	}

	template<typename T>
	const auto	&value() const {
		return (reinterpret_cast<T*>(_data.get()))->get();
	}

	template<typename T, typename U>
	U	value() {
		return (reinterpret_cast<T*>(_data.get()))->get();
	}

	template<typename T, typename U>
	U	value() const {
		return (reinterpret_cast<T*>(_data.get()))->get();
	}

	template<typename T>
	const T	&constGetData() const
	{
		return *(reinterpret_cast<T*>(_data.get()));
	}

	static inline Entity	newObject()
	{
		return Entity(Type::OBJ);
	}

	static inline Entity	newArray()
	{
		return Entity(Type::ARR);
	}
private:
	inline Entity(std::shared_ptr<AbstractData> &data);
private:
	std::shared_ptr<AbstractData>	_data;
};

template <>
inline int		Entity::to<int>() const;
template <>
inline long		Entity::to<long>() const;
template <>
inline double		Entity::to<double>() const;
template <>
inline std::string	Entity::to<std::string>() const;
template <>
inline char		*Entity::to<char *>() const;
template <>
inline bool		Entity::to<bool>() const;

class AbstractData
{
public:
	virtual bool		operator==(AbstractData const &) const = 0;
	virtual Entity::Type	getType() const = 0;
	virtual std::shared_ptr<AbstractData>
		clone(Entity::CloneOption attr) const = 0;

	inline virtual std::string	toString(void) const;
	inline virtual double		toNumber(void) const;
	inline virtual bool			toBool(void) const;
};

class Number final : public AbstractData
{
public:
	inline Number(double nbr);
	
	inline virtual bool		operator==(AbstractData const &) const final;
	inline virtual Entity::Type	getType() const final;
	inline virtual std::shared_ptr<AbstractData>
		clone(Entity::CloneOption attr) const final;
	inline double	get(void) const;
	inline void	set(double val);

	inline virtual std::string	toString(void) const final;
	inline virtual double		toNumber(void) const final;
	inline virtual bool		toBool(void) const final;
private:
	double	_value;
};

class Null final : public AbstractData
{
public:
	inline Null();
	
	inline virtual bool		operator==(AbstractData const &) const final;
	inline virtual Entity::Type	getType() const final;
	inline virtual std::shared_ptr<AbstractData>
		clone(Entity::CloneOption attr) const final;
	inline void	*get(void) const;

	inline virtual std::string	toString(void) const final;
};

class String final : public AbstractData, private std::string
{
public:
	inline String(std::string const &str);

	inline virtual bool		operator==(AbstractData const &) const final;
	inline virtual Entity::Type	getType() const final;
	inline virtual std::shared_ptr<AbstractData>
		clone(Entity::CloneOption attr) const final;
	inline const std::string	&get(void) const;
	inline std::string         &get(void);
	inline void				set(std::string const &str);

	inline virtual std::string	toString(void) const final;
	inline virtual double		toNumber(void) const final;
	inline virtual bool		toBool(void) const final;
};

class Bool final : public AbstractData
{
public:
	inline Bool(bool val);
	
	inline virtual bool		operator==(AbstractData const &) const final;
	inline virtual Entity::Type	getType() const final;
	inline virtual std::shared_ptr<AbstractData>
		clone(Entity::CloneOption attr) const final;
	inline bool	get(void) const;
	inline void	set(bool bol);

	inline virtual std::string	toString(void) const final;
	inline virtual double		toNumber(void) const final;
	inline virtual bool			toBool(void) const final;
private:
	bool	_value;
};

class Object final : public AbstractData, private std::unordered_map<std::string, Entity>
{
public:
	inline Object();
	inline Object(std::initializer_list<ObjEntry> list);

	inline virtual bool		operator==(AbstractData const &) const final;
	inline virtual Entity::Type	getType() const final;
	inline virtual std::shared_ptr<AbstractData>
		clone(Entity::CloneOption attr) const final;
	inline std::unordered_map<std::string, Entity>			&get(void);
	inline const std::unordered_map<std::string, Entity>	&get(void) const;
};

class Array final : public AbstractData, private std::vector<Entity>
{
public:
	inline Array();
	inline Array(std::initializer_list<Entity> list);

	inline virtual bool		operator==(AbstractData const &) const final;
	inline virtual Entity::Type	getType(void) const final;
	inline virtual std::shared_ptr<AbstractData>
		clone(Entity::CloneOption attr) const final;
	inline std::vector<Entity>			&get(void);
	inline const std::vector<Entity>	&get(void) const;
};

}

#include "Entity.ipp"