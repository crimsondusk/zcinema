#include <QObject>
#include <QStringList>
#include <QTextStream>
#include <assert.h>
#include "types.h"

str doFormat (initlist<StringFormatArg> args) {
	assert (args.size() >= 1);
	str text = args.begin()->value();
	
	for (initlist<StringFormatArg>::iterator it = args.begin() + 1; it != args.end(); ++it)
		text = text.arg (it->value());
	
	return text;
}

void doPrint (FILE* fp, initlist<StringFormatArg> args) {
	fprintf (fp, "%s", doFormat (args).toStdString().c_str());
}

// =============================================================================
StringFormatArg::StringFormatArg (const str& v)    { m_val = v; }
StringFormatArg::StringFormatArg (const char& v)   { m_val = v; }
StringFormatArg::StringFormatArg (const uchar& v)  { m_val = v; }
StringFormatArg::StringFormatArg (const qchar& v)  { m_val = v; }
StringFormatArg::StringFormatArg (const float& v)  { m_val = str::number (v); }
StringFormatArg::StringFormatArg (const double& v) { m_val = str::number (v); }
StringFormatArg::StringFormatArg (const char* v)   { m_val = v; }
StringFormatArg::StringFormatArg (const void* v)   { m_val.sprintf ("%p", v); }