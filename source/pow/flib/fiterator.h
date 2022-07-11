#pragma once

#include "fobject.h"
#include "fholder.h"

#define F_ITERATOR(...) \
public: \
	fiterator<__VA_ARGS__> begin() const \
	{ \
		fiterator<__VA_ARGS__> it; \
		_initialize_iterator(it); \
		return it; \
	} \
	fiterator_end end() const \
	{ \
		return fiterator_end(); \
	} \
private: \
	void _initialize_iterator(fiterator<__VA_ARGS__>& it) const

namespace flib {

class fiterator_end
{
};

template <typename T>
class fiterator
{
private:
	std::list<fholder<T>> m_list;

public:
	explicit fiterator() {}

	// initialization
	void add(fholder<T> const& value) { m_list.push_back(value); }

	// usage
	T& operator *() const
	{
		if (m_list.empty()) throw todo_throw_proper_exception();
		return *m_list.front();
	}

	fiterator& operator ++()
	{
		if (m_list.empty()) throw todo_throw_proper_exception();
		m_list.pop_front();
		return *this;
	}

	bool operator ==(fiterator_end const& end) const { return m_list.empty(); }
	bool operator !=(fiterator_end const& end) const { return !m_list.empty(); }
};

template <typename T>
class fiterator_ptr
{
private:
	fiterator<T> m_it;

public:
	explicit fiterator_ptr(fiterator<T> const& it) : m_it(it) {}

	T* operator *() const { return &*m_it; }
	fiterator_ptr& operator ++() { ++m_it; return *this; }

	bool operator ==(fiterator_end const& end) const { return m_it == end; }
	bool operator !=(fiterator_end const& end) const { return m_it != end; }
};

} // namespace flib
