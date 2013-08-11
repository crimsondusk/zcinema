/*
 *  ZanDemo: Zandronum demo launcher
 *  Copyright (C) 2013 Santeri Piippo
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TYPES_H
#define TYPES_H

#include "main.h"
#include <QString>
#include <QList>
#include <QVariant>

typedef QString str;
typedef QChar qchar;
typedef QVariant var;
template<class T> using list = QList<T>;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;
template<class T> using initlist = std::initializer_list<T>;
using std::size_t;

typedef qint8 int8;
typedef qint16 int16;
typedef qint32 int32;
typedef qint64 int64;
typedef quint8 uint8;
typedef quint16 uint16;
typedef quint32 uint32;
typedef quint64 uint64;

#ifdef IN_IDE_PARSER // :|
typedef void FILE;
#endif

// =============================================================================
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// =============================================================================
// StringFormatArg
//
// Converts a given value into a string that can be retrieved with ::value ().
// Used as the argument type to the formatting functions, hence its name.
// =============================================================================
class StringFormatArg {
public:
	StringFormatArg (const str& v);
	StringFormatArg (const char& v);
	StringFormatArg (const uchar& v);
	StringFormatArg (const qchar& v);
	
#define NUMERIC_FORMAT_ARG(T,C) \
	StringFormatArg (const T& v) { \
		char valstr[32]; \
		sprintf (valstr, "%" #C, v); \
		m_val = valstr; \
	}
	
	NUMERIC_FORMAT_ARG (int, d)
	NUMERIC_FORMAT_ARG (short, d)
	NUMERIC_FORMAT_ARG (long, ld)
	NUMERIC_FORMAT_ARG (uint, u)
	NUMERIC_FORMAT_ARG (ushort, u)
	NUMERIC_FORMAT_ARG (ulong, lu)
	
	StringFormatArg (const float& v);
	StringFormatArg (const double& v);
	StringFormatArg (const char* v);
	StringFormatArg (const void* v);
	
	template<class T> StringFormatArg (const list<T>& v) {
		m_val = "{ ";
		uint i = 0;
		const bool isString = typeid (T) == typeid (str);
		
		for (const T& it : v) {
			if (i++)
				m_val += ", ";
			
			StringFormatArg arg (it);
			
			if (isString)
				m_val += "\"" + arg.value() + "\"";
			else
				m_val += arg.value();
		}
		
		if (i)
			m_val += " ";
		
		m_val += "}";
	}
	
	str value() const {
		return m_val;
	}
private:
	str m_val;
};

// Formatter function
str doFormat (initlist<StringFormatArg> args);

// printf replacement
void doPrint (FILE* fp, initlist<StringFormatArg> args);

// Macros to access these functions
#ifndef IN_IDE_PARSER
# define fmt(...) doFormat({ __VA_ARGS__ })
# define print(...) doPrint (stdout, { __VA_ARGS__ })
# define fprint(FP, ...) doPrint (FP, { __VA_ARGS__ })
#else
str fmt (const char* fmtstr, ...);
void print (const char* fmtstr, ...);
void fprint (FILE* fp, const char* fmtstr, ...);
#endif

#endif // TYPES_H