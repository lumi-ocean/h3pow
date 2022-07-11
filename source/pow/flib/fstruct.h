#pragma once

#include "fobject.h"
#include "fstring.h"
#include "flist.h"

#define F_STRUCT(TYPE) \
public: \
	TYPE() { initialize_parent(); _initialize(); } \
	TYPE(TYPE const& other) : TYPE() { initialize_as_copy(other); } \
private: \
	typedef TYPE _this_type; \
	void _initialize()

#define F_PROPERTY(NAME, ...) \
public: \
	__VA_ARGS__ NAME = [&] { \
		auto constructor_check = [&] { _initialize(); }; \
		fproperty p(#NAME, &NAME); \
		__VA_ARGS__ default_value; \
		_initialize__##NAME(default_value, p.optional); \
		p.jdefault = default_value.jsave(); \
		add_property(p); \
		return __VA_ARGS__(default_value); \
	}(); \
private: \
	void _initialize__##NAME(__VA_ARGS__& value, bool& optional)

namespace flib {

class fproperty : public fobject
{
public:
	fstring name;
	fobject* value;
	jobject jdefault;
	bool optional = false;

	explicit fproperty(fstring const& name, fobject* value) : name(name), value(value) {}

	fproperty() : fproperty("", nullptr) {}
	fproperty(fproperty const& other) : fproperty(other.name, other.value)
	{
		jdefault = other.jdefault;
		optional = other.optional;
	}

	virtual jobject jsave() const override
	{
		jobject j(J_BLOCK);
		jobject jvalue = value->jsave();
		if (!optional || jvalue != jdefault)
		{
			j.add_item(J_EXPRESSION, jobject(J_IDENTIFIER, name.std_str()), jobject(J_ASSIGN, jvalue));
		}
		return j;
	}

	virtual void jload(jobject const& j) override
	{
		value->jload(jdefault);
		jpatch(j);
	}

	virtual jobject jdiff(jobject const& jother) const override
	{
		if (jother.type != J_BLOCK) throw todo_throw_proper_exception();
		jobject j(J_BLOCK);
		jobject jvalue_diff = value->jdiff(get_jvalue(jother));
		if (jvalue_diff.type != J_NONE)
		{
			j.add_item(J_EXPRESSION, jobject(J_IDENTIFIER, name.std_str()), jvalue_diff);
		}
		return j;
	}

	virtual void jpatch(jobject const& j) override
	{
		if (j.type != J_BLOCK) throw todo_throw_proper_exception();
		for (jobject const& jexpression : j)
		{
			if (jexpression.type != J_EXPRESSION) throw todo_throw_proper_exception();
			if (jexpression.count() != 2) throw todo_throw_proper_exception();
			if (jexpression[0].type != J_IDENTIFIER) throw todo_throw_proper_exception();
			if (name == jexpression[0].text)
			{
				value->jpatch(jexpression[1]);
			}
		}
	}

private:
	jobject const& get_jvalue(jobject const& j) const
	{
		if (j.type != J_BLOCK) throw todo_throw_proper_exception();
		for (jobject const& jexpression : j)
		{
			if (jexpression.type != J_EXPRESSION) throw todo_throw_proper_exception();
			if (jexpression.count() != 2) throw todo_throw_proper_exception();
			if (jexpression[0].type != J_IDENTIFIER) throw todo_throw_proper_exception();
			if (name == jexpression[0].text)
			{
				if (jexpression[1].type != J_ASSIGN) throw todo_throw_proper_exception();
				if (jexpression[1].count() != 1) throw todo_throw_proper_exception();
				return jexpression[1][0];
			}
		}
		return jdefault;
	}
};

class fstruct : public fobject
{
private:
	flist<fproperty> m_properties;

public:
	flistview<fproperty> properties;
	flistidx<fstring, fproperty> properties_idx;

	explicit fstruct() :
		m_properties(),
		properties(m_properties),
		properties_idx(m_properties, [](fproperty const& prop) { return prop.name; })
	{
		m_properties.set_parent(this);
	}

protected:
	void add_property(fproperty const& prop)
	{
		m_properties.add(prop);
	}

	void initialize_parent()
	{
		for (fproperty const& prop : properties)
		{
			prop.value->set_parent(this);
		}
	}

	void initialize_as_copy(fstruct const& other)
	{
		for (fproperty& prop : properties)
		{
			prop.value->jload(other.properties_idx[prop.name].value->jsave());
		}
	}

public:
	virtual jobject jsave() const override
	{
		jobject j(J_BLOCK);
		for (fproperty const& prop : properties)
		{
			j.add_items(prop.jsave());
		}
		return j;
	}

	virtual void jload(jobject const& j) override
	{
		for (fproperty& prop : properties)
		{
			prop.jload(j);
		}
	}

	virtual jobject jdiff(jobject const& jother) const override
	{
		jobject j(J_BLOCK);
		for (fproperty& prop : properties)
		{
			j.add_items(prop.jdiff(jother));
		}
		return j.count() > 0 ? jobject(J_MERGE_ASSIGN, j) : jobject(J_NONE);
	}

	virtual void jpatch(jobject const& j) override
	{
		switch (j.type)
		{
		case J_ASSIGN:
			if (j.count() != 1) throw todo_throw_proper_exception();
			jload(j[0]);
			break;
		case J_MERGE_ASSIGN:
			if (j.count() != 1) throw todo_throw_proper_exception();
			for (fproperty& prop : properties)
			{
				prop.jpatch(j[0]);
			}
			break;
		case J_NONE:
			break;
		default:
			throw todo_throw_proper_exception();
		}
	}
};

} // namespace flib


namespace fsample {

class SampleStruct : public fstruct
{
	F_STRUCT(SampleStruct) {}

	F_PROPERTY(a, fint) { value = 5; }
	F_PROPERTY(b, fint) { optional = true; }
};

} // namespace fsample
