#pragma once

#include "fobject.h"
#include "fstruct.h"
#include "fbox.h"

namespace flib {

class fvector2 : public fstruct
{
	F_STRUCT(fvector2) {}

	F_PROPERTY(x, fint) {}
	F_PROPERTY(y, fint) {}

public:
	fvector2(int x, int y) : fvector2()
	{
		this->x = x;
		this->y = y;
	}

	fvector2& operator =(fvector2 const& other) { x = other.x; y = other.y; return *this; }

	bool operator ==(fvector2 const& other) const { return x == other.x && y == other.y; }
	bool operator !=(fvector2 const& other) const { return !(*this == other); }

	virtual jobject jsave() const override
	{
		return jobject(J_VECTOR, x.jsave(), y.jsave());
	}

	virtual void jload(jobject const& j) override
	{
		if (j.type != J_VECTOR) throw todo_throw_proper_exception();
		if (j.count() != 2) throw todo_throw_proper_exception();
		x.jload(j[0]);
		y.jload(j[1]);
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

} // namespace flib
