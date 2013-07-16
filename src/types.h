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
	StringFormatArg( const str& v );
	StringFormatArg( const char& v );
	StringFormatArg( const uchar& v );
	StringFormatArg( const qchar& v );
	
#define NUMERIC_FORMAT_ARG(T,C) \
	StringFormatArg (const T& v) { \
		char valstr[32]; \
		sprintf (valstr, "%" #C, v); \
		m_val = valstr; \
	}
	
	NUMERIC_FORMAT_ARG( int, d )
	NUMERIC_FORMAT_ARG( short, d )
	NUMERIC_FORMAT_ARG( long, ld )
	NUMERIC_FORMAT_ARG( uint, u )
	NUMERIC_FORMAT_ARG( ushort, u )
	NUMERIC_FORMAT_ARG( ulong, lu )
	
	StringFormatArg( const float& v );
	StringFormatArg( const double& v );
	StringFormatArg( const char* v );
	StringFormatArg( const void* v );
	
	template<class T> StringFormatArg( const list<T>& v ) {
		m_val = "{ ";
		uint i = 0;
		
		for( const T& it : v ) {
			if( i++ )
				m_val += ", ";
			
			StringFormatArg arg( it );
			m_val += arg.value();
		}
		
		if( i )
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
str doFormat( initlist<StringFormatArg> args );

// printf replacement
void doPrint( initlist<StringFormatArg> args );

// Macros to access these functions
#ifndef IN_IDE_PARSER
# define fmt(...) doFormat({ __VA_ARGS__ })
# define print(...) doPrint({ __VA_ARGS__ })
#else
str fmt( const char* fmtstr, ... );
void print( const char* fmtstr, ... );
#endif

#endif // TYPES_H