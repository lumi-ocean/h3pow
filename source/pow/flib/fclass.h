#pragma once

#include "fobject.h"
#include "fstruct.h"
#include "fbox.h"

#define F_CLASS(TYPE) \
public: \
	TYPE() { initialize_parent(); _initialize(); } \
	TYPE(TYPE const& other) = delete; \
private: \
	typedef TYPE _this_type; \
	void _initialize()

namespace flib {

class fclass : public fstruct
{
public:
	fstring id; // not saved (should be saved by storage)
	fint native_slot = -1; // not saved (should be saved by storage)
};

} // namespace flib


namespace fsample {

class SampleClass : public fclass
{
	F_CLASS(SampleClass) {}

	F_PROPERTY(sample, SampleStruct) { value.a = 10; }
	F_PROPERTY(name, fstring) {}
};

} // namespace fsample
