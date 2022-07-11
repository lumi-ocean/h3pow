#pragma once

#include "fobject.h"
#include "fstring.h"
#include "fpair.h"
#include "flist.h"
#include "fiterator.h"

namespace flib {

class DictException : public std::exception {};
class DictInvalidKeyException : public DictException {};
class DictDuplicateKeyException : public DictException {};

template <class Key, class Value>
class fdict : public fobject
{
private:
	flist<fpair<Key, Value>> m_list;
	flistidx<Key, fpair<Key, Value>> m_idx;

public:
	explicit fdict() :
		m_list(),
		m_idx(m_list, [](fpair<Key, Value> const& pair) { return pair.key; })
	{
		m_list.set_parent(this);
	}

	fdict(fdict const& other) : fdict()
	{
		for (fpair<Key, Value> const& pair : other)
		{
			m_list.add(pair);
		}
	}

	fiterator<fpair<Key, Value>> begin() const { return m_list.begin(); }
	fiterator_end end() const { return fiterator_end(); }

	bool contains(Key const& key) const
	{
		return m_idx.contains(key);
	}

	fpair<Key, Value>& add(Key const& key, Value const& value)
	{
		return m_list.add_new(key, value);
	}

	void remove_first(std::function<bool(fpair<Key, Value> const&)> predicate)
	{
		m_list.remove_first(predicate);
	}

	void remove(Key const& key)
	{
		remove_first([&](fpair<Key, Value> const& pair) { return pair.key == key; });
	}

	void clear()
	{
		m_list.clear();
	}

	Value& operator [](Key const& key) const
	{
		return m_idx[key].value;
	}

	virtual jobject jsave() const override
	{
		return m_list.jsave();
	}

	virtual void jload(jobject const& j) override
	{
		m_list.jload(j);
	}

	virtual jobject jdiff(jobject const& jother) const override
	{
		return m_list.jdiff(jother);
	}

	virtual void jpatch(jobject const& j) override
	{
		m_list.jpatch(j);
	}
};

template <class Key, class Value>
class fautodict : public fobject
{
private:
	fdict<Key, Value> m_dict;
	fbox<Value> m_default;

public:
	explicit fautodict()
	{
		m_dict.set_parent(this);
		m_default.set_parent(this);
	}

	fautodict(fautodict const& other) : fautodict()
	{
		for (fpair<Key, Value> const& pair : other)
		{
			m_dict.add(pair.key, pair.value);
		}
	}

	fiterator<fpair<Key, Value>> begin() const { return m_dict.begin(); }
	fiterator_end end() const { return fiterator_end(); }

	void clear()
	{
		m_dict.clear();
	}

	void clear_default()
	{
		for (auto pair : m_dict)
		{
			if (pair.value == m_default)
			{
				m_dict.remove(pair.key);
			}
		}
	}

	Value& operator [](Key const& key)
	{
		clear_default();
		if (!m_dict.contains(key))
		{
			m_dict.add(key, Value());
		}
		return m_dict[key];
	}

	Value const& operator [](Key const& key) const
	{
		if (!m_dict.contains(key))
		{
			return m_default;
		}
		return m_dict[key];
	}

	virtual jobject jsave() const override
	{
		fautodict<Key, Value> copy = *this;
		copy.clear_default();
		return copy.m_dict.jsave();
	}

	virtual void jload(jobject const& j) override
	{
		m_dict.jload(j);
	}

	virtual jobject jdiff(jobject const& jother) const override
	{
		fautodict<Key, Value> copy = *this;
		copy.clear_default();
		return copy.m_dict.jdiff(jother);
	}

	virtual void jpatch(jobject const& j) override
	{
		m_dict.jpatch(j);
	}
};

} // namespace flib
