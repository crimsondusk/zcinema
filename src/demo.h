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

#ifndef ZCINEMA_DEMO_H
#define ZCINEMA_DEMO_H

#include "types.h"

enum {
	DemoLength,
	DemoVersion,
	DemoCVars,
	DemoUserInfo,
	DemoBodyStart,
	DemoTiccmd,
	DemoInvUse,
	DemoCenterView,
	DemoTaunt,
	DemoEnd,
	DemoWads
};

enum BuildType {
	OtherBuild    = 0,
	ReleaseBuild  = 1,
	InternalBuild = 2,
	PrivateBuild  = 3
};

struct VersionInfo {
	str shortVersion;
	str versionString;
	bool release;
};

int launchDemo (str path);

#endif // ZCINEMA_DEMO_H