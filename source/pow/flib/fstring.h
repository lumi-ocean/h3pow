#pragma once

#include "fobject.h"

namespace flib {

class fstring : public fobject
{
private:
	std::string m_stdstr;

public:
	fstring() : m_stdstr("") {}
	fstring(fstring const& other) : m_stdstr(other.m_stdstr) {}
	fstring(std::string const& stdstr) : m_stdstr(stdstr) {}
	fstring(char const* cstr) : m_stdstr(cstr) {}
	fstring(char ch) : m_stdstr(std::string("") + ch) {}
	fstring& operator =(fstring const& other) { m_stdstr = other.m_stdstr; return *this; }
	fstring& operator =(std::string const& stdstr) { m_stdstr = stdstr; return *this; }
	fstring& operator =(char const* cstr) { m_stdstr = cstr; return *this; }
	fstring& operator =(char ch) { m_stdstr = std::string("") + ch; return *this; }

	std::string std_str() const { return m_stdstr; }
	char const* c_str() const { return m_stdstr.c_str(); }

	bool operator ==(fstring const& other) const { return m_stdstr == other.m_stdstr; }
	bool operator !=(fstring const& other) const { return m_stdstr != other.m_stdstr; }

	bool operator ==(std::string const& stdstr) const { return m_stdstr == stdstr; }
	bool operator !=(std::string const& stdstr) const { return m_stdstr != stdstr; }

	bool operator ==(char const* cstr) const { return m_stdstr == cstr; }
	bool operator !=(char const* cstr) const { return m_stdstr != cstr; }

	bool operator <(fstring const& other) const { return m_stdstr < other.m_stdstr; }
	bool operator >(fstring const& other) const { return m_stdstr > other.m_stdstr; }
	bool operator <=(fstring const& other) const { return m_stdstr <= other.m_stdstr; }
	bool operator >=(fstring const& other) const { return m_stdstr >= other.m_stdstr; }

	fstring operator +(fstring const& other) const { return m_stdstr + other.m_stdstr; }

	bool is_empty() const { return m_stdstr.empty(); }
	bool starts_with(fstring const& prefix) const { return m_stdstr.starts_with(prefix.m_stdstr); }

	fstring lower() const
	{
		std::string str = m_stdstr;
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return str;
	}

	fstring substring(int start) const { return m_stdstr.substr(start); }
	fstring substring(int start, int count) const { return m_stdstr.substr(start, count); }

	int find(fstring const& str) const
	{
		auto pos = m_stdstr.find(str.std_str());
		if (pos != std::string::npos) return pos;
		else return -1;
	}

	int rfind(fstring const& str) const
	{
		auto pos = m_stdstr.rfind(str.std_str());
		if (pos != std::string::npos) return pos;
		else return -1;
	}

	int parse_int() const { return std::stoi(m_stdstr); }

	virtual jobject jsave() const override
	{
		return jobject(J_STRING, m_stdstr);
	}

	virtual void jload(jobject const& j) override
	{
		if (j.type != J_STRING) throw todo_throw_proper_exception();
		m_stdstr = j.text;
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
