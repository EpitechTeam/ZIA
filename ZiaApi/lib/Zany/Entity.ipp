/*
** EPITECH PROJECT, 2018
** api
** File description:
** Entity.ipp
*/

#pragma once

#include <cstring>
#include "Entity.hpp"

namespace zany
{

Entity::Entity(Type type)
{
	switch (type) {
		case Type::OBJ:
			_data = std::make_shared<Object>();
			break;
		case Type::ARR:
			_data = std::make_shared<Array>();
			break;
		case Type::NBR:
			_data = std::make_shared<Number>(0);
			break;
		case Type::STR:
			_data = std::make_shared<String>("");
			break;
		case Type::BOL:
			_data = std::make_shared<Bool>(false);
			break;
		case Type::NUL:
			_data = std::make_shared<Null>();
		default:
			break;
	}
}

Entity::Entity(double nbr):
	_data(std::make_shared<Number>(nbr))
{}

Entity::Entity(long nbr):
	_data(std::make_shared<Number>((double) nbr))
{}

Entity::Entity(unsigned long nbr):
	_data(std::make_shared<Number>((double) nbr))
{}

Entity::Entity(int nbr):
	_data(std::make_shared<Number>((double) nbr))
{}

Entity::Entity(unsigned int nbr):
	_data(std::make_shared<Number>((double) nbr))
{}

Entity::Entity(bool val):
	_data(std::make_shared<Bool>(val))
{}

Entity::Entity(std::string const &str):
	_data(std::make_shared<String>(str))
{}

Entity::Entity(const char *str):
	_data(std::make_shared<String>(std::string(str)))
{}

Entity::Entity(std::initializer_list<Entity> list):
	_data(std::make_shared<Array>(list))
{}

Entity::Entity(std::initializer_list<std::pair<const std::string, Entity>> list):
	_data(std::make_shared<Object>(list))
{}

Entity::Entity(std::shared_ptr<AbstractData> &data):
	_data(data)
{}

template <>
int	Entity::to<int>() const
{
	return int(_data->toNumber());
}

template <>
long	Entity::to<long>() const
{
	return long(_data->toNumber());
}

template <>
double	Entity::to<double>() const
{
	return _data->toNumber();
}

template <>
std::string	Entity::to<std::string>() const
{
	return _data->toString();
}

template <>
char	*Entity::to<char *>() const
{	
	return STRDUP(_data->toString().c_str());
}

template <>
bool	Entity::to<bool>() const
{
	return _data->toBool();
}

bool	Entity::operator==(Entity const &other) const
{
	return ((*_data) == other.constGetData<AbstractData>());
}

bool	Entity::operator!=(Entity const &other) const
{
	return !((*_data) == other.constGetData<AbstractData>());
}

Entity	&Entity::operator[](std::string const &key)
{
	if (_data == nullptr || _data->getType() != Type::OBJ) {
		throw std::runtime_error("zany::Entity: Error: "
			"Cannot use this Entity");
	}
	return getData<Object>().get()[key];
}

Entity	&Entity::operator[](unsigned idx)
{
	if (_data == nullptr || _data->getType() != Type::ARR) {
		throw std::runtime_error("zany::Entity: Error: "
			"Cannot use this Entity");
	}
	return getData<Array>().get()[idx];
}

const Entity	&Entity::operator[](std::string const &key) const
{
	if (_data == nullptr || _data->getType() != Type::OBJ) {
		throw std::runtime_error("zany::Entity: Error: "
			"Cannot use this Entity");
	}
	auto it = value<Object>().find(key);
	if (it != value<Object>().end()) {
		return it->second;
	} else {
		throw std::runtime_error("zany::Entity: Error: "
			"Cannot use this Entity");
	}
}

const Entity	&Entity::operator[](unsigned idx) const
{
	if (_data == nullptr || _data->getType() != Type::ARR) {
		throw std::runtime_error("zany::Entity: Error: "
			"Cannot use this Entity");
	}
	auto &v = value<Array>();
	if (v.size() < idx) {
		return v.at(idx);
	} else {
		throw std::runtime_error("zany::Entity: Error: "
			"Cannot use this Entity");
	}
}

bool	Entity::isObject(void) const
{
	return (_data->getType() == Type::OBJ);
}

bool	Entity::isArray(void) const
{
	return (_data->getType() == Type::ARR);
}

bool	Entity::isNumber(void) const
{
	return (_data->getType() == Type::NBR);
}

bool	Entity::isString(void) const
{
	return (_data->getType() == Type::STR);
}

bool	Entity::isBool(void) const
{
	return (_data->getType() == Type::BOL);
}

bool	Entity::isNull(void) const
{
	return (_data->getType() == Type::NUL);
}

Entity	&Entity::push(Entity const &obj)
{
	if (_data == nullptr || _data->getType() != Type::ARR) {
		throw std::runtime_error("zany::Entity: Error: "
			"Cannot use this Entity");
	}
	auto &arr = getData<Array>().get();

	arr.emplace_back(obj);
	return arr.back();
}

void	Entity::merge(Entity const &toAdd, OnConflitFunc const &onConflit)
{
	auto	myType = _data->getType();

	if ((myType == Type::ARR || myType == Type::OBJ) &&
			myType == toAdd.constGetData<AbstractData>().getType()) {
		if (myType == Type::ARR) {
			auto &vec = const_cast<Array&>(toAdd.constGetData<Array>()).get();
		
			getData<Array>().get().insert(
				getData<Array>().get().end(),
				vec.begin(), vec.end()
			);
		} else {
			auto	&toAddMap = const_cast<Object&>(toAdd.constGetData<Object>()).get();
			auto	&myMap = getData<Object>().get();
			
			for (auto it = toAddMap.begin(); it != toAddMap.end(); it++) {
				auto myIt = myMap.find(it->first);
				if (myIt != myMap.end()) {
					if ((myIt->second.isArray() && it->second.isArray()) ||
						(myIt->second.isObject() && it->second.isObject())) {
						myIt->second.merge(it->second, onConflit);
					} else {
						const_cast<OnConflitFunc &>
							(onConflit)(myIt->first, myIt->second, it->second);
					}
				} else {
					myMap.insert(*it);
				}
			}
		}
	} else {
		throw std::runtime_error("zany::Entity: Error: "
			"Cannot use this Entity");
	}
}

Entity	Entity::clone(CloneOption attr) const
{
	auto	copiedData = _data->clone(attr);

	return Entity(copiedData);
}

void
Entity::_basicOnConfit(std::string const &, Entity &first, Entity &second)
{
	first = {
		first,
		second
	};
}

/***********************************************
 *	AbstractData
***********************************************/
std::string	AbstractData::toString(void) const
{
	throw std::runtime_error("zany::Entity: Error: "
		"Cannot use this Entity");
}

double		AbstractData::toNumber(void) const
{
	throw std::runtime_error("zany::Entity: Error: "
		"Cannot use this Entity");
}

bool		AbstractData::toBool(void) const
{
	throw std::runtime_error("zany::Entity: Error: "
		"Cannot use this Entity");
}

/***********************************************
 *	NUMBER
***********************************************/
static inline std::string	_to_string_with_precision(double a_value, int n)
{
	std::ostringstream	out;

	out << std::setprecision(n) << a_value;
	return out.str();
}

Number::Number(double nbr) :
	_value(nbr)
{}

bool		Number::operator==(AbstractData const &other) const
{
	return (other.getType() == Entity::Type::NBR &&
		_value == static_cast<const Number&>(other).get());
}

Entity::Type	Number::getType() const
{
	return Entity::Type::NBR;
}

std::shared_ptr<AbstractData>	Number::clone(Entity::CloneOption attr) const
{
	(void) attr;
	return std::make_shared<Number>(_value);
}

double	Number::get(void) const
{
	return _value;
}

void	Number::set(double nbr)
{
	_value = nbr;
}

std::string	Number::toString(void) const
{
	return std::to_string(_value);
}

double		Number::toNumber(void) const
{
	return _value;
}

bool		Number::toBool(void) const
{
	return bool(_value);
}

/***********************************************
 *	NULL
***********************************************/

Null::Null()
{}

Entity::Type	Null::getType() const
{
	return Entity::Type::NUL;
}

bool		Null::operator==(AbstractData const &other) const
{
	return (other.getType() == Entity::Type::NUL);
}

std::shared_ptr<AbstractData>	Null::clone(Entity::CloneOption attr) const
{
	(void) attr;
	return std::make_shared<Null>();
}

void	*Null::get(void) const
{
	return nullptr;
}

std::string	Null::toString(void) const
{
	return "null";
}

/***********************************************
 *	STRING
***********************************************/
String::String(std::string const &str) :
	std::string(str)
{}

Entity::Type	String::getType() const
{
	return Entity::Type::STR;
}

bool		String::operator==(AbstractData const &other) const
{
	return (other.getType() == Entity::Type::STR &&
		*this == static_cast<const String&>(other).get());
}

std::shared_ptr<AbstractData>	String::clone(Entity::CloneOption attr) const
{
	(void) attr;
	return std::make_shared<String>(*this);
}

const std::string	&String::get(void) const
{
	return *this;
}

std::string         &String::get(void)
{
	return *this;
}

void		String::set(std::string const &str)
{
	dynamic_cast<std::string&>(*this) = str;
}

std::string	String::toString(void) const
{
	return *this;
}

double		String::toNumber(void) const
{
	return std::atof(this->c_str());
}

bool		String::toBool(void) const
{
	if (STRCASECMP(this->c_str(), "true") == 0) {
		return true;
	} else if (STRCASECMP(this->c_str(), "false") == 0) {
		return false;
	} else {
		throw std::runtime_error("zany::Entity: Error: "
			"Cannot use this Entity");
	}
}

/***********************************************
 *	Bool
***********************************************/
Bool::Bool(bool val) :
	_value(val)
{}

Entity::Type	Bool::getType() const
{
	return Entity::Type::BOL;
}

bool		Bool::operator==(AbstractData const &other) const
{
	return (other.getType() == Entity::Type::BOL &&
		_value == static_cast<const Bool&>(other).get());
}

std::shared_ptr<AbstractData>	Bool::clone(Entity::CloneOption attr) const
{
	(void) attr;
	return std::make_shared<Bool>(_value);
}

bool	Bool::get(void) const
{
	return _value;
}

void	Bool::set(bool val)
{
	_value = val;
}

std::string	Bool::toString(void) const
{
	return (_value ? "true" : "false");
}

double		Bool::toNumber(void) const
{
	return (_value ? 1.0 : 0.0);
}

bool		Bool::toBool(void) const
{
	return _value;
}


/***********************************************
 *	Object
***********************************************/
Object::Object()
{}

Object::Object(std::initializer_list<ObjEntry> list):
	std::unordered_map<std::string, Entity>(list)	
{}

Entity::Type	Object::getType() const
{
	return Entity::Type::OBJ;
}

bool		Object::operator==(AbstractData const &other) const
{
	return (other.getType() == Entity::Type::OBJ &&
		*this == const_cast<Object&>(static_cast<const Object&>(other)).get());
}

std::shared_ptr<AbstractData>	Object::clone(Entity::CloneOption attr) const
{
	auto	newObj = std::make_shared<Object>();

	for (auto &elem : *this) {
		if (attr == Entity::CloneOption::DEEP) {
			newObj->get()[elem.first] = elem.second.clone(attr);
		} else {
			newObj->get()[elem.first] = elem.second;
		}
	}
	return newObj;
}

std::unordered_map<std::string, Entity>	&Object::get(void)
{
	return *this;
}

const std::unordered_map<std::string, Entity>	&Object::get(void) const
{
	return *this;
}

/***********************************************
 *	Array
***********************************************/

Array::Array()
{}

Array::Array(std::initializer_list<Entity> list):
	std::vector<Entity>(list)
{}

Entity::Type	Array::getType() const
{
	return Entity::Type::ARR;
}

bool		Array::operator==(AbstractData const &other) const
{
	return (other.getType() == Entity::Type::ARR &&
		*this == const_cast<Array&>(static_cast<const Array&>(other)).get());
}

std::shared_ptr<AbstractData>	Array::clone(Entity::CloneOption attr) const
{
	auto	newArr = std::make_shared<Array>();

	for (auto &elem : *this) {
		if (attr == Entity::CloneOption::DEEP) {
			newArr->get().push_back(elem.clone(attr));
		} else {
			newArr->get().push_back(elem);
		}
	}
	return newArr;
}

std::vector<Entity>	&Array::get(void)
{
	return *this;
}

const std::vector<Entity>	&Array::get(void) const
{
	return *this;
}

}
