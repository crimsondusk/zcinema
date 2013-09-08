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
StringFormatArg::StringFormatArg (const QChar& v)  { m_val = v; }
StringFormatArg::StringFormatArg (const float& v)  { m_val = str::number (v); }
StringFormatArg::StringFormatArg (const double& v) { m_val = str::number (v); }
StringFormatArg::StringFormatArg (const char* v)   { m_val = v; }
StringFormatArg::StringFormatArg (const void* v)   { m_val.sprintf ("%p", v); }