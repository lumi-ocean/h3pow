#pragma once

#include "fobject.h"
#include "fclass.h"

namespace flib {

class StorageException : public std::exception {};
class StorageOverflowException : public StorageException {};
class StorageInvalidIdException : public StorageException {};

template <class T>
class fstorage : public fobject
{
private:
	flist<T> m_list;

public:
	flistview_ptr<T> by_pos;
	flistidx_ptr<fstring, T> by_id;
	flistidx_ptr<int, T> by_native_slot;

	flist<fstring> fixed_native_slots; // not saved
	fint total_native_slots = std::numeric_limits<int>::max() / 2; // not saved
	fstring generic_id_prefix; // not saved
	fint generic_id_counter;

	explicit fstorage() :
		m_list(),
		by_pos(m_list),
		by_id(m_list, [](T const& obj) { return obj.id; }),
		by_native_slot(m_list, [](T const& obj) { return obj.native_slot; })
	{
		m_list.set_parent(this);
	}

	fstorage(fstorage const& other) : fstorage()
	{
		for (T* obj : other)
		{
			m_list.add_new_and_initialize([&](T& new_obj) { new_obj.id = obj->id; new_obj.native_slot = obj->native_slot; }).jload(obj->jsave());
		}
		for (fstring const& id : other.fixed_native_slots)
		{
			fixed_native_slots.add(id);
		}
		total_native_slots = other.total_native_slots;
		generic_id_prefix = other.generic_id_prefix;
		generic_id_counter = other.generic_id_counter;
	}

	fiterator_ptr<T> begin() const { return fiterator_ptr(m_list.begin()); }
	fiterator_end end() const { return fiterator_end(); }

	bool is_empty() const { return m_list.is_empty(); }
	int count() const { return m_list.count(); }

	bool contains(fstring const& id) const
	{
		return by_id.contains(id);
	}

	bool contains(int native_slot) const
	{
		return by_native_slot.contains(native_slot);
	}

	template <typename... Args>
	T* create(fstring const& id, Args... args)
	{
		if (id.is_empty()) throw StorageInvalidIdException();

		int native_slot = fixed_native_slots.find(id);
		if (native_slot < 0)
		{
			native_slot = fixed_native_slots.count();
			while (by_native_slot.contains(native_slot) && native_slot < total_native_slots) native_slot++;
			if (native_slot >= total_native_slots) throw StorageOverflowException();
		}

		return &m_list.add_new_and_initialize([&](T& new_obj) { new_obj.id = id; new_obj.native_slot = native_slot; }, args...);
	}

	void remove(fstring const& id)
	{
		m_list.remove_first([&](T const& obj) { return obj.id == id; });
	}

	void clear()
	{
		m_list.clear();
	}

	virtual jobject jsave() const override
	{
		jobject j(J_BLOCK);
		if (generic_id_counter != 0)
		{
			j.add_item(J_EXPRESSION, jobject(J_IDENTIFIER, "generic_id_counter"), jobject(J_ASSIGN, generic_id_counter.jsave()));
		}
		for (T const& obj : m_list)
		{
			jobject jpair(J_PAIR);
			jpair.add_item(obj.native_slot.jsave());
			jpair.add_item(obj.jsave());
			j.add_item(J_EXPRESSION, jobject(J_IDENTIFIER, obj.id.std_str()), jobject(J_ASSIGN, jpair));
		}
		return j;
	}

	virtual void jload(jobject const& j) override
	{
		generic_id_counter = 0;
		clear();
		jmerge(j);
	}

	virtual jobject jdiff(jobject const& jother) const override
	{
		jobject j(J_BLOCK);
		flist<fstring> found_objects;
		flistidx<fstring, fstring> found_objects_idx(found_objects, [](fstring const& id) { return id; });

		if (jother.type != J_BLOCK) throw todo_throw_proper_exception();
		for (jobject const& jexpression : jother)
		{
			if (jexpression.type != J_EXPRESSION) throw todo_throw_proper_exception();
			if (jexpression.count() != 2) throw todo_throw_proper_exception();
			if (jexpression[0].type != J_IDENTIFIER) throw todo_throw_proper_exception();
			if (jexpression[1].type != J_ASSIGN) throw todo_throw_proper_exception();
			if (jexpression[1].count() != 1) throw todo_throw_proper_exception();

			fstring id = jexpression[0].text;
			jobject const& jvalue_or_pair = jexpression[1][0];

			if (id == "generic_id_counter")
			{
				jobject const& jvalue = jvalue_or_pair;
				jobject jvalue_diff = generic_id_counter.jdiff(jvalue);
				if (jvalue_diff.type != J_NONE)
				{
					j.add_item(J_EXPRESSION, jobject(J_IDENTIFIER, "generic_id_counter"), jvalue_diff);
				}
				found_objects.add("generic_id_counter");
			}
			else if (by_id.contains(id))
			{
				if (jvalue_or_pair.type == J_PAIR)
				{
					if (jvalue_or_pair.count() != 2) throw todo_throw_proper_exception();
					// native slot is ignored
				}
				jobject const& jvalue = jvalue_or_pair.type == J_PAIR ? jvalue_or_pair[1] : jvalue_or_pair;
				jobject jvalue_diff = by_id[id]->jdiff(jvalue);
				if (jvalue_diff.type != J_NONE)
				{
					j.add_item(J_EXPRESSION, jobject(J_IDENTIFIER, id.std_str()), jvalue_diff);
				}
				found_objects.add(id);
			}
			else
			{
				j.add_item(J_EXPRESSION, jobject(J_IDENTIFIER, id.std_str()), jobject(J_ASSIGN, jobject(J_NULL)));
			}
		}

		if (!found_objects_idx.contains("generic_id_counter") && generic_id_counter != 0)
		{
			j.add_item(J_EXPRESSION, jobject(J_IDENTIFIER, "generic_id_counter"), jobject(J_ASSIGN, generic_id_counter.jsave()));
		}
		for (T const& obj : m_list)
		{
			if (!found_objects_idx.contains(obj.id))
			{
				// native slot is ignored
				j.add_item(J_EXPRESSION, jobject(J_IDENTIFIER, obj.id.std_str()), jobject(J_ASSIGN, obj.jsave()));
			}
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
			jmerge(j[0]);
			break;
		case J_NONE:
			break;
		default:
			throw todo_throw_proper_exception();
		}
	}

private:
	void jmerge(jobject const& j)
	{
		if (j.type != J_BLOCK) throw todo_throw_proper_exception();
		for (jobject const& jexpression : j)
		{
			if (jexpression.type != J_EXPRESSION) throw todo_throw_proper_exception();
			if (jexpression.count() != 2) throw todo_throw_proper_exception();
			if (jexpression[0].type != J_IDENTIFIER) throw todo_throw_proper_exception();

			fstring id = jexpression[0].text;
			jobject const& patch = jexpression[1];

			if (id == "generic_id_counter")
			{
				generic_id_counter.jpatch(patch);
			}
			else if (by_id.contains(id))
			{
				if (patch.type == J_ASSIGN && patch.count() == 1 && patch[0].type == J_NULL)
				{
					remove(id);
				}
				else
				{
					by_id[id]->jpatch(patch);
				}
			}
			else
			{
				if (patch.type != J_ASSIGN) throw todo_throw_proper_exception();
				if (patch.count() != 1) throw todo_throw_proper_exception();

				if (patch[0].type == J_BLOCK)
				{
					create(id)->jload(patch[0]);
				}
				else if (patch[0].type == J_PAIR)
				{
					if (patch[0].count() != 2) throw todo_throw_proper_exception();

					fint native_slot;
					native_slot.jload(patch[0][0]);
					jobject const& block = patch[0][1];

					if (native_slot < 0 || native_slot >= total_native_slots) throw todo_throw_proper_exception();

					m_list.add_new_and_initialize([&](T& new_obj) { new_obj.id = id; new_obj.native_slot = native_slot; }).jload(block);
				}
				else throw todo_throw_proper_exception();
			}
		}
	}
};

} // namespace flib
