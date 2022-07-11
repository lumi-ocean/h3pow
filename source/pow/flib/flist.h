#pragma once

#include "fobject.h"
#include "fholder.h"
#include "fstring.h"
#include "fiterator.h"

namespace flib {

class ListException : public std::exception {};
class ListIndexOutOfRangeException : public ListException {};
class ListInvalidKeyException : public ListException {};
class ListDuplicateKeyException : public ListException {};

template <typename T>
class flist : public fobject
{
	F_ITERATOR(T)
	{
		for (fholder<T> const& holder : m_list)
		{
			it.add(holder);
		}
	}

private:
	std::vector<fholder<T>> m_list;

public:
	fevent<fholder<T> const&> on_before_add_holder;
	fevent<fholder<T> const&> on_before_remove_holder;

	explicit flist() {}

	flist(flist const& other) : flist()
	{
		for (T const& obj : other)
		{
			add(obj);
		}
	}

	std::vector<fholder<T>> const& holders() const { return m_list; }

	template <typename... Args>
	fholder<T>& add_new_holder(Args... args)
	{
		return add_new_holder_and_initialize([&](fholder<T>& holder) {}, args...);
	}

	template <typename... Args>
	fholder<T>& add_new_holder_and_initialize(std::function<void(T&)> initialize_func, Args... args)
	{
		return add_new_holder_and_initialize([&](fholder<T>& holder) { initialize_func(*holder); }, args...);
	}

	template <typename... Args>
	fholder<T>& add_new_holder_and_initialize(std::function<void(fholder<T>&)> initialize_func, Args... args)
	{
		fholder<T> holder(args...);
		holder.box().set_parent(this);
		initialize_func(holder);
		on_before_add_holder(holder);
		m_list.push_back(holder);
		return m_list.back();
	}

	template <typename... Args>
	T& add_new(Args... args) { return *add_new_holder(args...); }

	template <typename... Args>
	T& add_new_and_initialize(std::function<void(T&)> initialize_func, Args... args) { return *add_new_holder_and_initialize(initialize_func, args...); }

	T& add(T const& obj) { return add_new(obj); }

	template <typename Value>
	int find(Value const& value) { return find_first([&](T const& obj) { return obj == value; }); }

	int find_first(std::function<bool(T const&)> predicate)
	{
		for (int i = 0; i < count(); i++)
		{
			if (predicate(*m_list[i]))
			{
				return i;
			}
		}
		return -1;
	}

	void remove_at(int index) { m_list.erase(m_list.begin() + index); }

	template <typename Value>
	void remove(Value const& value) { remove_first([&](T const& obj) { return obj == value; }); }

	void remove_first(std::function<bool(T const&)> predicate)
	{
		int index = find_first(predicate);
		if (index >= 0)
		{
			remove_at(index);
		}
	}

	void clear()
	{
		for (fholder<T> const& holder : m_list)
		{
			on_before_remove_holder(holder);
		}
		m_list.clear();
	}

	bool is_empty() const { return m_list.empty(); }
	int count() const { return m_list.size(); }

	T& operator [](int index) const
	{
		if (index < 0 || index >= count()) throw ListIndexOutOfRangeException();
		return *m_list[index];
	}

	virtual jobject jsave() const override
	{
		jobject j(J_LIST);
		for (fholder<T> const& holder : m_list)
		{
			j.add_item(holder.box().jsave());
		}
		return j;
	}

	virtual void jload(jobject const& j) override
	{
		if (j.type != J_LIST) throw todo_throw_proper_exception();
		clear();
		for (jobject const& jobj : j)
		{
			add_new_holder_and_initialize([&](fholder<T>& holder) { holder.box().jload(jobj); });
		}
	}

	virtual jobject jdiff(jobject const& jother) const override
	{
		if (jother.type != J_LIST) throw todo_throw_proper_exception();
		if (count() < jother.count())
		{
			return jobject(J_ASSIGN, jsave());
		}
		for (int i = 0; i < jother.count(); i++)
		{
			if (m_list[i].box().jdiff(jother[i]).type != J_NONE)
			{
				return jobject(J_ASSIGN, jsave());
			}
		}
		if (count() > jother.count())
		{
			jobject j(J_LIST);
			for (int i = jother.count(); i < count(); i++)
			{
				j.add_item(m_list[i].box().jsave());
			}
			return jobject(J_ADD_ASSIGN, j);
		}
		return jobject(J_NONE);
	}

	virtual void jpatch(jobject const& j) override
	{
		switch (j.type)
		{
		case J_ASSIGN:
			if (j.count() != 1) throw todo_throw_proper_exception();
			jload(j[0]);
			break;
		case J_ADD_ASSIGN:
			if (j.count() != 1) throw todo_throw_proper_exception();
			if (j[0].type != J_LIST) throw todo_throw_proper_exception();
			for (jobject const& jobj : j[0])
			{
				add_new_holder_and_initialize([&](fholder<T>& holder) { holder.box().jload(jobj); });
			}
			break;
		case J_NONE:
			break;
		default:
			throw todo_throw_proper_exception();
		}
	}
};

template <typename T>
class flistview
{
private:
	flist<T>* m_list;

public:
	explicit flistview(flist<T>& list) : m_list(&list) {}

	fiterator<T> begin() const { return m_list->begin(); }
	fiterator_end end() const { return fiterator_end(); }

	bool is_empty() const { return m_list->is_empty(); }
	int count() const { return m_list->count(); }

	T& operator [](int index) const { return (*m_list)[index]; }
};

template <typename T>
class flistview_ptr
{
private:
	flist<T>* m_list;

public:
	explicit flistview_ptr(flist<T>& list) : m_list(&list) {}

	fiterator_ptr<T> begin() const { return fiterator_ptr(m_list->begin()); }
	fiterator_end end() const { return fiterator_end(); }

	bool is_empty() const { return m_list->is_empty(); }
	int count() const { return m_list->count(); }

	T* operator [](int index) const { return &(*m_list)[index]; }
};

template <typename Key, typename T>
class flistidx
{
private:
	flist<T>* m_list;
	std::map<Key, fholder<T>> m_idx;
	std::function<Key(T const&)> m_func;

public:
	explicit flistidx(flist<T>& list, std::function<Key(T const&)> const& func) : m_list(&list), m_func(func)
	{
		for (fholder<T> const& holder : m_list->holders())
		{
			m_idx[m_func(*holder)] = holder;
		}

		m_list->on_before_add_holder += [&](fholder<T> const& holder) {
			Key key = m_func(*holder);
			if (m_idx.contains(key)) throw ListDuplicateKeyException();
			m_idx[key] = holder;
		};

		m_list->on_before_remove_holder += [&](fholder<T> const& holder) {
			Key key = m_func(*holder);
			m_idx.erase(key);
		};
	}

	bool contains(Key const& key) const
	{
		return m_idx.contains(key);
	}

	T& operator [](Key const& key) const
	{
		if (!m_idx.contains(key)) throw ListInvalidKeyException();
		return *m_idx.at(key);
	}
};

template <typename Key, typename T>
class flistidx_ptr
{
private:
	flistidx<Key, T> m_idx;

public:
	explicit flistidx_ptr(flist<T>& list, std::function<Key(T const&)> const& func) : m_idx(list, func) {}

	bool contains(Key const& key) const { return m_idx.contains(key); }

	T* operator [](Key const& key) const
	{
		if (!m_idx.contains(key)) return nullptr;
		return &m_idx[key];
	}
};

} // namespace flib
