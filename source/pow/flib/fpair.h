#pragma once

#include "fobject.h"
#include "fbox.h"

namespace flib {

template <class Key, class Value>
class fpair : public fobject
{
public:
	Key key;
	Value value;

	explicit fpair()
	{
		fbox_if_needed(key).set_parent(this);
		fbox_if_needed(value).set_parent(this);
	}

	explicit fpair(Key const& key, Value const& value) : fpair()
	{
		this->key = key;
		this->value = value;
	}

	fpair(fpair const& other) : fpair()
	{
		key = other.key;
		value = other.value;
	}

	bool operator ==(fpair const& other) const { return key == other.key && value == other.value; } // todo: remove
	bool operator !=(fpair const& other) const { return !(*this == other); }

	virtual jobject jsave() const override
	{
		jobject j(J_PAIR);
		j.add_item(fbox_if_needed(key).jsave());
		j.add_item(fbox_if_needed(value).jsave());
		return j;
	}

	virtual void jload(jobject const& j) override
	{
		if (j.type != J_PAIR) throw todo_throw_proper_exception();
		if (j.count() != 2) throw todo_throw_proper_exception();
		if (is_not_fobject<Key> || is_not_fobject<Value>) throw todo_throw_proper_exception();
		fbox_if_needed(key).jload(j[0]);
		fbox_if_needed(value).jload(j[1]);
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
