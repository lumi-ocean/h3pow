#pragma once

#include "jobject.h"

namespace flib {

template <typename... Args>
struct fevent
{
private:
	std::vector<std::function<void(Args...)>> m_handlers;

public:
	fevent& operator +=(std::function<void(Args...)> const& handler)
	{
		m_handlers.push_back(handler);
		return *this;
	}

	void operator ()(Args... args)
	{
		for (auto& handler : m_handlers)
		{
			handler(args...);
		}
	}
};

class fobject
{
	fobject(fobject const&) = delete;
	fobject& operator =(fobject const&) = delete;

private:
	fobject* m_parent = nullptr;

public:
	explicit fobject() {}
	virtual ~fobject() {}

	virtual jobject jsave() const = 0;
	virtual void jload(jobject const& j) = 0;

	virtual jobject jdiff(jobject const& jother) const = 0;
	virtual void jpatch(jobject const& j) = 0;

	template <class T>
	T* get_parent() const
	{
		if (m_parent == nullptr) return nullptr;

		T* cast = dynamic_cast<T*>(m_parent);
		if (cast != nullptr) return cast;

		return m_parent->get_parent<T>();
	}
	void set_parent(fobject* parent) { m_parent = parent; } // only for containers
};

template <typename T>
inline constexpr bool is_fobject = std::is_base_of<fobject, T>::value;

template <typename T>
inline constexpr bool is_not_fobject = !is_fobject<T>;

template <class T>
inline jobject simple_jdiff(T const& obj, jobject const& jother)
{
	T other = T();
	other.jload(jother);
	if (obj != other)
	{
		return jobject(J_ASSIGN, obj.jsave());
	}
	return jobject(J_NONE);
}

template <class T>
inline void simple_jpatch(T& obj, jobject const& j)
{
	switch (j.type)
	{
	case J_ASSIGN:
		if (j.count() != 1) throw todo_throw_proper_exception();
		obj.jload(j[0]);
		break;
	case J_NONE:
		break;
	default:
		throw todo_throw_proper_exception();
	}
}

} // namespace flib
