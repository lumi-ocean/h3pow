#include "pch.h"
#include "jobject.h"

namespace flib {

void save_fjs_block(jobject const& obj, std::ostream& fjs, int level);

void save_fjs_value(jobject const& obj, std::ostream& fjs, int level)
{
	switch (obj.type)
	{
	case J_NONE:
		throw todo_throw_proper_exception();
	case J_NULL:
		fjs << "null";
		break;
	case J_TRUE:
		fjs << "true";
		break;
	case J_FALSE:
		fjs << "false";
		break;
	case J_IDENTIFIER:
		fjs << obj.text;
		break;
	case J_NUMBER:
		fjs << obj.text;
		break;
	case J_STRING:
		fjs << "\"";
		for (char ch : obj.text)
		{
			if (ch == '\t')
			{
				fjs << "\\t";
			}
			else if (ch == '\n')
			{
				fjs << "\\n";
			}
			else if (ch == '"')
			{
				fjs << "\\\"";
			}
			else if (ch == '\\')
			{
				fjs << "\\\\";
			}
			else if (ch >= 0 && ch < 0x20)
			{
				throw todo_throw_proper_exception();
			}
			else
			{
				fjs << ch;
			}
		}
		fjs << "\"";
		break;
	case J_PAIR:
		if (obj.count() != 2) throw todo_throw_proper_exception();
		save_fjs_value(obj[0], fjs, level);
		fjs << ": ";
		save_fjs_value(obj[1], fjs, level);
		break;
	case J_VECTOR:
		fjs << "<";
		for (int i = 0; i < obj.count(); i++)
		{
			if (i > 0) fjs << ":";
			save_fjs_value(obj[i], fjs, level);
		}
		fjs << ">";
		break;
	case J_LIST:
		fjs << "[";
		for (int i = 0; i < obj.count(); i++)
		{
			if (i > 0) fjs << ", ";
			save_fjs_value(obj[i], fjs, level);
		}
		fjs << "]";
		break;
	case J_BLOCK:
		fjs << "{";
		if (obj.count() > 0)
		{
			fjs << "\n";
			save_fjs_block(obj, fjs, level + 1);
			fjs << std::string(level, '\t');
		}
		fjs << "}";
		break;
	case J_EXPRESSION:
	case J_ASSIGN:
	case J_ADD_ASSIGN:
	case J_MERGE_ASSIGN:
	default:
		throw todo_throw_proper_exception();
	};
}

void save_fjs_block(jobject const& obj, std::ostream& fjs, int level)
{
	if (obj.type != J_BLOCK) throw todo_throw_proper_exception();
	for (jobject const& jexpression : obj)
	{
		if (jexpression.type != J_EXPRESSION) throw todo_throw_proper_exception();
		if (jexpression.count() != 2) throw todo_throw_proper_exception();
		fjs << std::string(level, '\t');
		save_fjs_value(jexpression[0], fjs, level);
		switch (jexpression[1].type)
		{
		case J_ASSIGN:
			fjs << " = ";
			break;
		case J_ADD_ASSIGN:
			fjs << " += ";
			break;
		case J_MERGE_ASSIGN:
			fjs << " |= ";
			break;
		default:
			throw todo_throw_proper_exception();
		}
		if (jexpression[1].count() != 1) throw todo_throw_proper_exception();
		save_fjs_value(jexpression[1][0], fjs, level);
		fjs << "\n";
	}
}

void jobject::save_fjs(std::filesystem::path const& path) const
{
	std::ofstream fjs(path);
	save_fjs_block(*this, fjs, 0);
}


char read_char(std::istream& fjs)
{
	char ch = fjs.get();
	if (fjs.eof() || fjs.fail())
	{
		return 0;
	}
	return ch;
}

jobject load_fjs_block(std::istream& fjs);

jobject load_fjs_value(std::istream& fjs, char first)
{
	if ((first >= 'a' && first <= 'z') || (first >= 'A' && first <= 'Z') || first == '_')
	{
		std::string id = std::string("") + first;
		while (char ch = read_char(fjs))
		{
			if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
			{
				id += ch;
			}
			else
			{
				fjs.seekg(-1, std::ios_base::cur);
				break;
			}
		}
		if (id == "null") return jobject(J_NULL);
		else if (id == "true") return jobject(J_TRUE);
		else if (id == "false") return jobject(J_FALSE);
		else return jobject(J_IDENTIFIER, id);
	}
	else if ((first >= '0' && first <= '9') || first == '-' || first == '+')
	{
		std::string num = std::string("") + first;
		while (char ch = read_char(fjs))
		{
			if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '.' || ch == '-' || ch == '+')
			{
				num += ch;
			}
			else
			{
				fjs.seekg(-1, std::ios_base::cur);
				break;
			}
		}
		return jobject(J_NUMBER, num);
	}
	else if (first == '"')
	{
		std::string str;
		while (char ch = read_char(fjs))
		{
			if (ch == '\\')
			{
				char escape = read_char(fjs);
				switch (escape)
				{
				case 't':
					str += '\t';
					break;
				case 'n':
					str += '\n';
					break;
				case '"':
					str += '"';
					break;
				case '\\':
					str += '\\';
					break;
				default:
					throw todo_throw_proper_exception();
				}
			}
			else if (ch == '"')
			{
				return jobject(J_STRING, str);
			}
			else if (ch >= 0 && ch < 0x20)
			{
				throw todo_throw_proper_exception();
			}
			else
			{
				str += ch;
			}
		}
		throw todo_throw_proper_exception();
	}
	else if (first == '<')
	{
		jobject obj(J_VECTOR);
		while (char ch = read_char(fjs))
		{
			switch (ch)
			{
			case '\t':
			case '\n':
			case ' ':
			case ':':
				break;
			case '>':
				return obj;
			default:
				obj.add_item(load_fjs_value(fjs, ch));
				break;
			}
		}
		throw todo_throw_proper_exception();
	}
	else if (first == '[')
	{
		jobject obj(J_LIST);
		while (char ch = read_char(fjs))
		{
			switch (ch)
			{
			case '\t':
			case '\n':
			case ' ':
			case ',':
				break;
			case ':':
				if (obj.count() < 1 || obj[obj.count() - 1].type == J_PAIR) throw todo_throw_proper_exception();
				do { ch = read_char(fjs); } while (ch == '\t' || ch == '\n' || ch == ' ');
				obj[obj.count() - 1] = jobject(J_PAIR, obj[obj.count() - 1], load_fjs_value(fjs, ch));
				break;
			case ']':
				return obj;
			default:
				obj.add_item(load_fjs_value(fjs, ch));
				break;
			}
		}
		throw todo_throw_proper_exception();
	}
	else if (first == '{')
	{
		return load_fjs_block(fjs);
	}
	else throw todo_throw_proper_exception();
}

jtype get_expression_type(std::istream& fjs, jobject& a)
{
	while (char ch = read_char(fjs))
	{
		switch (ch)
		{
		case '\t':
		case '\n':
		case ' ':
			break;
		case ':':
			if (a.type == J_PAIR) throw todo_throw_proper_exception();
			do { ch = read_char(fjs); } while (ch == '\t' || ch == '\n' || ch == ' ');
			a = jobject(J_PAIR, a, load_fjs_value(fjs, ch));
			break;
		case '=':
			return J_ASSIGN;
		case '+':
			if (read_char(fjs) != '=') throw todo_throw_proper_exception();
			return J_ADD_ASSIGN;
		case '|':
			if (read_char(fjs) != '=') throw todo_throw_proper_exception();
			return J_MERGE_ASSIGN;
		default:
			throw todo_throw_proper_exception();
		}
	}
	throw todo_throw_proper_exception();
}

jobject load_fjs_block(std::istream& fjs)
{
	jobject obj(J_BLOCK);
	while (char ch = read_char(fjs))
	{
		switch (ch)
		{
		case '\t':
		case '\n':
		case ' ':
			break;
		case ':':
			if (obj.count() < 1 || obj[obj.count() - 1][1][0].type == J_PAIR) throw todo_throw_proper_exception();
			do { ch = read_char(fjs); } while (ch == '\t' || ch == '\n' || ch == ' ');
			obj[obj.count() - 1][1][0] = jobject(J_PAIR, obj[obj.count() - 1][1][0], load_fjs_value(fjs, ch));
			break;
		case '}':
			return obj;
		default:
			jobject a = load_fjs_value(fjs, ch);
			jtype expression_type = get_expression_type(fjs, a);

			do { ch = read_char(fjs); } while (ch == '\t' || ch == '\n' || ch == ' ');
			jobject b = load_fjs_value(fjs, ch);

			obj.add_item(J_EXPRESSION, a, jobject(expression_type, b));
			break;
		}
	}
	return obj;
}

jobject jobject::load_fjs(std::filesystem::path const& path)
{
	std::ifstream fjs(path);
	return load_fjs_block(fjs);
}

} // namespace flib
