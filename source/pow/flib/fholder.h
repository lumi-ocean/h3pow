#pragma once

#include "fobject.h"
#include "fbox.h"

namespace flib {

template <typename T>
class fholder
{
private:
	std::shared_ptr<T> m_box;

public:
	template <typename... Args>
	explicit fholder(Args... args) : m_box(std::make_shared<T>(args...)) {}

	T& operator *() const { return *m_box; }
	T* operator ->() const { return &*m_box; }

	T& box() const { return *m_box; }
};

template <typename T> requires is_not_fobject<T>
class fholder<T>
{
private:
	std::shared_ptr<fbox<T>> m_box;

public:
	template <typename... Args>
	explicit fholder(Args... args) : m_box(std::make_shared<fbox<T>>(T(args...))) {}

	T& operator *() const { return m_box->raw_value(); }
	T* operator ->() const { return &m_box->raw_value(); }

	fbox<T>& box() const { return *m_box; }
};

} // namespace flib
