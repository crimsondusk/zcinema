#include <QObject>
#include <QStringList>
#include <QTextStream>
#include <assert.h>
#include "types.h"

str doFormat( std::vector<StringFormatArg> args ) {
	assert( args.size() >= 1 );
	str text = args[0].value();
	
	for( uchar i = 1; i < args.size(); ++i )
		text = text.arg( args[i].value() );
	
	return text;
}

void doPrint( initlist<StringFormatArg> args ) {
	printf( "%s", doFormat( args ).toStdString().c_str() );
}

// =============================================================================
StringFormatArg::StringFormatArg( const str& v )    { m_val = v; }
StringFormatArg::StringFormatArg( const char& v )   { m_val = v; }
StringFormatArg::StringFormatArg( const uchar& v )  { m_val = v; }
StringFormatArg::StringFormatArg( const qchar& v )  { m_val = v; }
StringFormatArg::StringFormatArg( const float& v )  { m_val = str::number( v ); }
StringFormatArg::StringFormatArg( const double& v ) { m_val = str::number( v ); }
StringFormatArg::StringFormatArg( const char* v )   { m_val = v; }
StringFormatArg::StringFormatArg( const void* v )   { m_val.sprintf( "%p", v ); }