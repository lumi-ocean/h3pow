#pragma once

#include "fobject.h"
#include "fholder.h"
#include "fiterator.h"

namespace flib {

class ArrayException : public std::exception {};
class ArrayIndexOutOfRangeException : public ArrayException {};

template <class T, int SIZE>
class farray : public fobject
{
private:
	fholder<T> m_array[SIZE];

public:
	static const int SIZE = SIZE;

	explicit farray()
	{
		for (int i = 0; i < SIZE; i++)
		{
			m_array[i].box().set_parent(this);
		}
	}

	farray(farray const& other) : farray()
	{
		for (int i = 0; i < SIZE; i++)
		{
			*m_array[i] = other[i];
		}
	}

	void initialize(T const& value)
	{
		for (int i = 0; i < SIZE; i++)
		{
			*m_array[i] = value;
		}
	}

	T& operator [](int index)
	{
		if (index < 0 || index >= SIZE) throw ArrayIndexOutOfRangeException();
		return *m_array[index];
	}

	T const& operator [](int index) const
	{
		if (index < 0 || index >= SIZE) throw ArrayIndexOutOfRangeException();
		return *m_array[index];
	}

	virtual jobject jsave() const override
	{
		jobject j(J_LIST);
		for (int i = 0; i < SIZE; i++)
		{
			j.add_item(m_array[i].box().jsave());
		}
		return j;
	}

	virtual void jload(jobject const& j) override
	{
		if (j.type != J_LIST) throw todo_throw_proper_exception();
		if (j.count() != SIZE) throw todo_throw_proper_exception();
		for (int i = 0; i < SIZE; i++)
		{
			m_array[i].box().jload(j[i]);
		}
	}

	virtual jobject jdiff(jobject const& jother) const override
	{
		if (jother.type != J_LIST) throw todo_throw_proper_exception();
		if (jother.count() != SIZE) throw todo_throw_proper_exception();
		for (int i = 0; i < SIZE; i++)
		{
			if (m_array[i].box().jdiff(jother[i]).type != J_NONE)
			{
				return jobject(J_ASSIGN, jsave());
			}
		}
		return jobject(J_NONE);
	}

	virtual void jpatch(jobject const& j) override
	{
		simple_jpatch(*this, j);
	}
};

} // namespace flib
