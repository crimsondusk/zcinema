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

#ifndef ZANDEMO_MAIN_H
#define ZANDEMO_MAIN_H

#define APPNAME "ZanDemo"
#define UNIXNAME "zandemo"
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 999
#define BUILD_ID BUILD_INTERNAL
#define RC_ID 0

#define BUILD_INTERNAL 0
#define BUILD_ALPHA    1
#define BUILD_BETA     2
#define BUILD_RC       3
#define BUILD_RELEASE  4

#define elif(A) else if (A)
#define alias   auto&

#include <QSettings>
#include "cfg.h"

struct VersionInfo;
static const std::nullptr_t null = nullptr;

QString versionString();

#endif // ZANDEMO_MAIN_H