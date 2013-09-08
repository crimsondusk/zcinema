/*
 *  ZCinema: Zandronum demo launcher
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

#ifndef ZCINEMA_CFG_H
#define ZCINEMA_CFG_H

// =============================================================================
#include <QString>
#include <QVariant>
#include <QKeySequence>

#define CONFIG(T, NAME, DEFAULT) namespace cfg { T NAME = DEFAULT; } \
	cfg::ConfigAdder zz_ConfigAdder_##NAME (&cfg::NAME, cfg::T##Type, #NAME, DEFAULT);

#define EXTERN_CONFIG(T, NAME) namespace cfg { extern T NAME; }

// =========================================================
namespace cfg {
	enum Type {
		IntType,
		StringType,
		FloatType,
		BoolType,
		KeySeqType,
		ListType,
		MapType,
	};
	
	// Type-definitions for the above enum list
	typedef int Int;
	typedef QString String;
	typedef float Float;
	typedef bool Bool;
	typedef QKeySequence KeySeq;
	typedef QList<QVariant> List;
	typedef QMap<QString, QVariant> Map;
	
	// ------------------------------------------
	bool load();
	bool save();
	void reset();
	
	class ConfigAdder {
	public:
		ConfigAdder (void* ptr, Type type, const char* name, QVariant def);
	};
};

#endif // ZCINEMA_CFG_H