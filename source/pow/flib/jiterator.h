#pragma once

#include "base.h"

#define J_ITERATOR() \
public: \
	jiterator begin() const \
	{ \
		jiterator it; \
		_initialize_iterator(it); \
		return it; \
	} \
	jiterator_end end() const \
	{ \
		return jiterator_end(); \
	} \
private: \
	void _initialize_iterator(jiterator& it) const

namespace flib {

struct jobject;

class jiterator_end
{
};

class jiterator
{
private:
	std::list<std::shared_ptr<jobject>> m_list;

public:
	explicit jiterator() {}

	// initialization
	void add(std::shared_ptr<jobject> const& value) { m_list.push_back(value); }

	// usage
	jobject& operator *() const
	{
		if (m_list.empty()) throw todo_throw_proper_exception();
		return *m_list.front();
	}

	jiterator& operator ++()
	{
		if (m_list.empty()) throw todo_throw_proper_exception();
		m_list.pop_front();
		return *this;
	}

	bool operator ==(jiterator_end const& end) const { return m_list.empty(); }
	bool operator !=(jiterator_end const& end) const { return !m_list.empty(); }
};

} // namespace flib
