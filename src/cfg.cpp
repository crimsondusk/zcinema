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

#include <cassert>
#include <QDir>
#include <QTextStream>
#include <QSettings>
#include "main.h"
#include "config.h"

#define MAX_CONFIG 512

// =============================================================================
// -----------------------------------------------------------------------------
namespace cfg {
	static struct {
		void* ptr;
		Type type;
		const char* name;
		QVariant def;
	} g_configData[MAX_CONFIG];
	
	static int g_cfgDataCursor = 0;
	
#define CASE(T) case T##Type: return QVariant (*(reinterpret_cast<T*> (ptr)));
	static QVariant getConfigValue (void* ptr, Type type) {
		switch (type) {
		CASE (Int)
		CASE (String)
		CASE (Float)
		CASE (Bool)
		CASE (List)
		CASE (Map)
		
		case KeySeqType:
			return QVariant (reinterpret_cast<KeySeq*> (ptr)->toString());
		}
		
		assert (false);
		return QVariant();
	}
#undef CASE
	
#define CASE(T, METHOD) case T##Type: (*(reinterpret_cast<T*> (ptr))) = val.METHOD(); return;
	static void setConfigValue (void* ptr, Type type, const QVariant& val) {
		switch (type) {
		CASE (Int, toInt)
		CASE (String, toString)
		CASE (Float, toFloat)
		CASE (Bool, toBool)
		CASE (List, toList)
		CASE (Map, toMap)
		
		case KeySeqType:
			reinterpret_cast<KeySeq*> (ptr)->fromString (val.toString());
			break;
		}
	}
	
	// =============================================================================
	// Get the QSettings object.
	// -----------------------------------------------------------------------------
	static QSettings* getSettingsObject() {
		return new QSettings;
	}
	
	// =============================================================================
	// Load the configuration from file
	// -----------------------------------------------------------------------------
	bool load() {
		QSettings* settings = getSettingsObject();
		print ("config::load: Loading configuration file from %1\n", settings->fileName());
		
		for (alias i : g_configData) {
			if (i.name == null)
				break;
			
			setConfigValue (i.ptr, i.type, settings->value (i.name, i.def));
		}
		
		settings->deleteLater();
		return true;
	}
	
	// =============================================================================
	// Save the configuration to disk
	// -----------------------------------------------------------------------------
	bool save() {
		QSettings* settings = getSettingsObject();
		settings->clear();
		print ("Saving configuration to %1...\n", settings->fileName());
		
		for (alias i : g_configData) {
			if (i.name == null)
				break;
			
			QVariant val = getConfigValue (i.ptr, i.type);
			
			if (val == i.def)
				continue;
			
			settings->setValue (i.name, val);
		}
		
		settings->sync();
		settings->deleteLater();
		return true;
	}
	
	// =============================================================================
	// Reset configuration defaults.
	// -----------------------------------------------------------------------------
	void reset() {
		for (alias i : g_configData) {
			if (i.name == null)
				break;
			
			setConfigValue (i.ptr, i.type, i.def);
		}
	}

	// =============================================================================
	// We cannot just add config objects to a list or vector because that would rely
	// on the vector's c-tor being called before the configs' c-tors. With global
	// variables we cannot assume that!! Therefore we need to use a C-style array here.
	// -----------------------------------------------------------------------------
	ConfigAdder::ConfigAdder (void* ptr, Type type, const char* name, QVariant def) {
		if (g_cfgDataCursor == 0)
			memset (g_configData, 0, sizeof g_configData);
		
		assert (g_cfgDataCursor < MAX_CONFIG);
		alias i = g_configData[g_cfgDataCursor++];
		i.ptr = ptr;
		i.type = type;
		i.name = name;
		i.def = def;
	}
}