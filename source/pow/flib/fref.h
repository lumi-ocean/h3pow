#pragma once

#include "fobject.h"
#include "fclass.h"
#include "fstorage.h"

#define F_RESOLVER(TYPE) \
fstorage<TYPE> const& _resolve_storage__##TYPE(fref<TYPE> const& ref); \
template <> \
struct fresolver<TYPE> \
{ \
	static TYPE* resolve(fref<TYPE> const& ref) { return ref.get(_resolve_storage__##TYPE(ref)); } \
}; \
fstorage<TYPE> const& _resolve_storage__##TYPE(fref<TYPE> const& ref)

namespace flib {

template <class T>
class fref;

class ResolverException : public std::exception {};
class ResolverNotImplementedException : public ResolverException {};

template <class T>
struct fresolver
{
	static T* resolve(fref<T> const& ref) { throw ResolverNotImplementedException(); }
};

template <class T>
class fref : public fobject
{
public:
	fstring ref_id;

	fref() {}

	fref(fref const& other) : fref() { ref_id = other.ref_id; }
	fref(T const* obj) : fref() { ref_id = (obj != nullptr ? obj->id : fstring("")); }
	fref(nullptr_t const& null) : fref() { ref_id = ""; }

	fref(fstring const& id) : fref() { ref_id = id; }
	fref(std::string const& id) : fref() { ref_id = id; }
	fref(char const* id) : fref() { ref_id = id; }

	fref& operator =(fref const& other) { ref_id = other.ref_id; return *this; }
	fref& operator =(T const* obj) { ref_id = (obj != nullptr ? obj->id : fstring("")); return *this; }
	fref& operator =(nullptr_t const& null) { ref_id = ""; return *this; }

	fref& operator =(fstring const& id) { ref_id = id; return *this; }
	fref& operator =(std::string const& id) { ref_id = id; return *this; }
	fref& operator =(char const* id) { ref_id = id; return *this; }

	operator T*() const { return fresolver<T>::resolve(*this); }
	T* operator ->() const { return fresolver<T>::resolve(*this); }

	bool operator ==(fref const& other) const { return ref_id == other.ref_id; }
	bool operator !=(fref const& other) const { return ref_id != other.ref_id; }

	bool operator ==(T const* obj) const { return ref_id == (obj != nullptr ? obj->id : fstring("")); }
	bool operator !=(T const* obj) const { return ref_id != (obj != nullptr ? obj->id : fstring("")); }

	bool operator ==(nullptr_t const& null) const { return ref_id.is_empty(); }
	bool operator !=(nullptr_t const& null) const { return !ref_id.is_empty(); }

	bool operator ==(fstring const& id) const { return ref_id == id; }
	bool operator !=(fstring const& id) const { return ref_id != id; }

	bool operator ==(std::string const& id) const { return ref_id == id; }
	bool operator !=(std::string const& id) const { return ref_id != id; }

	bool operator ==(char const* id) const { return ref_id == id; }
	bool operator !=(char const* id) const { return ref_id != id; }

	bool operator <(fref const& other) const { return ref_id < other.ref_id; }
	bool operator >(fref const& other) const { return ref_id > other.ref_id; }
	bool operator <=(fref const& other) const { return ref_id <= other.ref_id; }
	bool operator >=(fref const& other) const { return ref_id >= other.ref_id; }

	bool is_null() const { return ref_id.is_empty(); }

	bool is_valid(fstorage<T> const& storage) const
	{
		return storage.by_id.contains(ref_id);
	}

	T* get(fstorage<T> const& storage) const
	{
		if (storage.contains(ref_id)) return storage.by_id[ref_id];
		else return nullptr;
	}

	virtual jobject jsave() const override
	{
		return ref_id.is_empty() ? jobject(J_NULL) : jobject(J_IDENTIFIER, ref_id.std_str());
	}

	virtual void jload(jobject const& j) override
	{
		switch (j.type)
		{
		case J_NULL:
			ref_id = "";
			break;
		case J_IDENTIFIER:
			ref_id = j.text;
			break;
		default:
			throw todo_throw_proper_exception();
		}
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


namespace fsample {

class RefSample : public fstruct
{
	F_STRUCT(RefSample) {}

	F_PROPERTY(storage, fstorage<SampleClass>) {}
	F_PROPERTY(sample, fref<SampleClass>) { value.ref_id = "test"; }

public:
	void initialize() { storage.create("test"); }
	void test() { sample.get(storage)->name = "123"; }
};

} // namespace fsample
