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

#include "misc.h"
#include <QMessageBox>

EXTERN_CONFIG (List, devBuildNames)
EXTERN_CONFIG (List, releaseNames)
EXTERN_CONFIG (Map, binaryPaths)

uint32 makeByteID (uint8 a, uint8 b, uint8 c, uint8 d) {
	return a | (b << 8) | (c << 16) | (d << 24);
}

// =============================================================================
// -----------------------------------------------------------------------------
QList<QVariant> getVersions() {
	return cfg::devBuildNames + cfg::releaseNames;
}

// =============================================================================
// -----------------------------------------------------------------------------
str binaryConfigName (str ver) {
	return fmt ("binaries/%1", ver);
}

// =============================================================================
// -----------------------------------------------------------------------------
void addVersion (str name, bool isRelease, str binaryPath) {
	cfg::binaryPaths[name] = binaryPath;
	
	if (isRelease)
		cfg::releaseNames << QVariant (name);
	else
		cfg::devBuildNames << QVariant (name);
	
	cfg::save();
}

// =============================================================================
// -----------------------------------------------------------------------------
str basename (str path) {
	long lastpos = path.lastIndexOf ("/");
	
	if (lastpos != -1)
		return path.mid (lastpos + 1);
	
	return path;
}

// =============================================================================
// -----------------------------------------------------------------------------
bool confirm (str text) {
	return QMessageBox::question (null, QObject::tr ("Confirm"), text,
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::No;
}