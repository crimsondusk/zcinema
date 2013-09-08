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

#include <QApplication>
#include "config.h"
#include "types.h"
#include "demo.h"

// =============================================================================
// -----------------------------------------------------------------------------
int main (int argc, char* argv[]) {
	QApplication app (argc, argv);
	app.setApplicationName (UNIXNAME);
	app.setOrganizationName (UNIXNAME);
	app.setApplicationVersion (versionString());
	
	cfg::load();
	
	for (int i = 1; i < argc; ++i) {
		str arg = argv[i];
		
		if (arg == "--config") {
			ConfigBox dlg;
			return dlg.exec();
		}
	}
	
	if (argc != 2) {
		fprint (stderr, "Usage: %1 <demo>   - Launch a demo file\n", argv[0]);
		fprint (stderr, "       %1 --config - Configure " APPNAME "\n", argv[0]);
		return 255;
	}
	
	return launchDemo (argv[1]);
}

// =============================================================================
// -----------------------------------------------------------------------------
str versionString() {
	str text = fmt ("v%1.%2", VERSION_MAJOR, VERSION_MINOR);
#if VERSION_PATCH != 0
	text += fmt (".%1", VERSION_PATCH);
#endif
	
#if BUILD_ID == BUILD_INTERNAL
	text += "-intern";
#elif BUILD_ID == BUILD_ALPHA
	text += "-alpha";
#elif BUILD_ID == BUILD_BETA
	text += "-beta";
#elif BUILD_ID == BUILD_RC
	text += fmt ("-rc%1", RC_ID);
#elif BUILD_ID == BUILD_RELEASE
	text += "-rel";
#else
# error Invalid build code!
#endif // BUILD_ID
	
	return text;
}
