#pragma once

#include "base.h"
#include "jiterator.h"

namespace flib {

enum jtype
{
	J_NONE,
	J_NULL,
	J_TRUE,
	J_FALSE,
	J_IDENTIFIER,
	J_NUMBER,
	J_STRING,
	J_PAIR,
	J_VECTOR,
	J_LIST,
	J_BLOCK,
	J_EXPRESSION,
	J_ASSIGN,
	J_ADD_ASSIGN,
	J_MERGE_ASSIGN,
};

struct jobject
{
	J_ITERATOR()
	{
		for (std::shared_ptr<jobject> const& item : items)
		{
			it.add(item);
		}
	}

public:
	jtype type = J_NONE;
	std::string text;
	std::vector<std::shared_ptr<jobject>> items;

	explicit jobject() {}
	explicit jobject(jtype type) : type(type) {}
	explicit jobject(jtype type, std::string const& text) : type(type), text(text) {}

	explicit jobject(jtype type, jobject const& item) : type(type)
	{
		add_item(item);
	}

	explicit jobject(jtype type, jobject const& item1, jobject const& item2) : type(type)
	{
		add_item(item1);
		add_item(item2);
	}

	int count() const { return items.size(); }

	template <typename... Args>
	jobject& add_item(Args... args)
	{
		items.push_back(std::make_shared<jobject>(args...));
		return *items.back();
	}

	void add_items(jobject const& other)
	{
		for (jobject const& item : other)
		{
			add_item(item);
		}
	}

	bool operator ==(jobject const& other) const
	{
		if (type != other.type) return false;
		if (text != other.text) return false;
		if (count() != other.count()) return false;
		for (int i = 0; i < count(); i++)
		{
			if (*items[i] != *other.items[i]) return false;
		}
		return true;
	}
	bool operator !=(jobject const& other) const { return !(*this == other); }

	jobject& operator [](int index) const
	{
		if (index < 0 || index >= count()) throw todo_throw_proper_exception();
		return *items[index];
	}

	void save_fjs(std::filesystem::path const& path) const;
	static jobject load_fjs(std::filesystem::path const& path);
};

} // namespace flib
