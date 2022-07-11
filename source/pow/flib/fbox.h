#pragma once

#include "fobject.h"

#define F_BOX(CLASS, TYPE) \
private: \
	TYPE m_value; \
public: \
	CLASS() : m_value() { _initialize(); } \
	CLASS(CLASS const& other) : CLASS() { m_value = other.m_value; } \
	CLASS(TYPE value) : CLASS() { m_value = value; } \
	CLASS& operator =(CLASS const& other) { m_value = other.m_value; return *this; } \
	CLASS& operator =(TYPE value) { m_value = value; return *this; } \
	operator TYPE() const { return m_value; } \
	bool operator ==(CLASS const& other) const { return m_value == other.m_value; } \
	bool operator !=(CLASS const& other) const { return m_value != other.m_value; } \
	bool operator ==(TYPE value) const { return m_value == value; } \
	bool operator !=(TYPE value) const { return m_value != value; } \
	TYPE& raw_value() { return m_value; } \
	TYPE const& raw_value() const { return m_value; } \
private: \
	void _initialize()

namespace flib {

template <typename T, bool is_fobj = is_fobject<T>>
struct fboxing;

template <typename T>
struct fboxing<T, true>
{
	using box = T;
};

template <typename T>
using fbox = fboxing<T>::box;

template <typename T> requires is_fobject<T>
T& fbox_if_needed(T& obj) { return obj; }

template <typename T> requires is_fobject<T>
T const& fbox_if_needed(T const& obj) { return obj; }

template <typename T> requires is_not_fobject<T>
fbox<T> fbox_if_needed(T const& obj) { return obj; }

class fbool : public fobject
{
	F_BOX(fbool, bool) {}

public:
	virtual jobject jsave() const override
	{
		return jobject(m_value ? J_TRUE : J_FALSE);
	}

	virtual void jload(jobject const& j) override
	{
		if (j.type == J_TRUE) m_value = true;
		else if (j.type == J_FALSE) m_value = false;
		else throw todo_throw_proper_exception();
	}

	virtual jobject jdiff(jobject const& jother) const override
	{
		return simple_jdiff(*this, jother);
	}

	virtual void jpatch(jobject const& j) override
	{
		simple_jpatch(*this, j);
	}
};

template <>
struct fboxing<bool>
{
	using box = fbool;
};

class fint : public fobject
{
	F_BOX(fint, int) {}

public:
	virtual jobject jsave() const override
	{
		return jobject(J_NUMBER, std::to_string(m_value));
	}

	virtual void jload(jobject const& j) override
	{
		if (j.type != J_NUMBER) throw todo_throw_proper_exception();
		m_value = std::stoi(j.text);
	}

	virtual jobject jdiff(jobject const& jother) const override
	{
		return simple_jdiff(*this, jother);
	}

	virtual void jpatch(jobject const& j) override
	{
		simple_jpatch(*this, j);
	}
};

template <>
struct fboxing<int>
{
	using box = fint;
};

class ffloat : public fobject
{
	F_BOX(ffloat, float) {}

public:
	virtual jobject jsave() const override
	{
		std::ostringstream strstream;
		strstream << m_value;
		return jobject(J_NUMBER, strstream.str());
	}

	virtual void jload(jobject const& j) override
	{
		if (j.type != J_NUMBER) throw todo_throw_proper_exception();
		m_value = std::stof(j.text);
	}

	virtual jobject jdiff(jobject const& jother) const override
	{
		return simple_jdiff(*this, jother);
	}

	virtual void jpatch(jobject const& j) override
	{
		simple_jpatch(*this, j);
	}
};

template <>
struct fboxing<float>
{
	using box = ffloat;
};

class fdouble : public fobject
{
	F_BOX(fdouble, double) {}

public:
	virtual jobject jsave() const override
	{
		std::ostringstream strstream;
		strstream << m_value;
		return jobject(J_NUMBER, strstream.str());
	}

	virtual void jload(jobject const& j) override
	{
		if (j.type != J_NUMBER) throw todo_throw_proper_exception();
		m_value = std::stod(j.text);
	}

	virtual jobject jdiff(jobject const& jother) const override
	{
		return simple_jdiff(*this, jother);
	}

	virtual void jpatch(jobject const& j) override
	{
		simple_jpatch(*this, j);
	}
};

template <>
struct fboxing<double>
{
	using box = fdouble;
};

template <typename T>
class frawptr : public fobject
{
	F_BOX(frawptr, T*) {}

public:
	virtual jobject jsave() const override
	{
		throw todo_throw_proper_exception();
	}

	virtual void jload(jobject const& j) override
	{
		throw todo_throw_proper_exception();
	}

	virtual jobject jdiff(jobject const& jother) const override
	{
		throw todo_throw_proper_exception();
	}

	virtual void jpatch(jobject const& j) override
	{
		throw todo_throw_proper_exception();
	}
};

template <typename T>
struct fboxing<T*, false>
{
	using box = frawptr<T>;
};

} // namespace flib
