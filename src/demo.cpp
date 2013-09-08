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

#include <QFile>
#include <QDataStream>
#include <QMessageBox>
#include <QProcess>
#include "demo.h"
#include "misc.h"
#include "ui_demoprompt.h"
#include "prompts.h"

EXTERN_CONFIG (Map,  binaryPaths)
EXTERN_CONFIG (List, wadpaths)
EXTERN_CONFIG (Bool, noprompt)

static const uint32 g_demoSignature = makeByteID ('Z', 'C', 'L', 'D');

// =============================================================================
// -----------------------------------------------------------------------------
str uncolorize (const str& in) {
	str out;
	int skip = 0;
	
	for (const QChar& c : in) {
		if (skip-- > 0)
			continue;
		
		if (c == QChar ('\034')) {
			skip = 1;
			continue;
		}
		
		out += c;
	}
	
	return out;
}

// =============================================================================
// -----------------------------------------------------------------------------
static str tr (const char* msg) {
	return QObject::tr (msg);
}

// =============================================================================
// -----------------------------------------------------------------------------
static void error (str msg) {
	QMessageBox::critical (null, "Error", msg);
}

// =============================================================================
// -----------------------------------------------------------------------------
static bool isKnownVersion (str ver) {
	for (const QVariant& it : getVersions())
		if (it.toString() == ver || it.toString() + 'M' == ver)
			return true;
	
	return false;
}

// =============================================================================
// -----------------------------------------------------------------------------
static str findWAD (str name) {
	if (cfg::wadpaths.size() == 0) {
		error (tr ("No WAD paths configured!"));
		
		// Cannot just return an empty string here since that'd trigger
		// another error prompt - skip ahead and exit.
		exit (9);
	}
	
	for (const QVariant& it : cfg::wadpaths) {
		str fullpath = fmt ("%1/%2", it.toString(), name);
		QFile f (fullpath);
		
		if (f.exists())
			return fullpath;
	}
	
	return "";
}

// =============================================================================
// -----------------------------------------------------------------------------
str readString (QDataStream& stream) {
	str out;
	uint8 c;
	
	for (stream >> c; c != '\0'; stream >> c)
		out += c;
	
	return out;
}

// =============================================================================
// -----------------------------------------------------------------------------
int launchDemo (str path) {
	QFile f (path);
	
	if (!f.open (QIODevice::ReadOnly)) {
		error (fmt (tr ("Couldn't open '%1' for reading: %2"), path, strerror (errno)));
		return 1;
	}
	
	QDataStream stream (&f);
	stream.setByteOrder (QDataStream::LittleEndian);
	
	uint8 offset;
	uint32 length;
	uint16 zanversionID, numWads;
	uint32 longSink;
	str zanversion;
	list<str> wads;
	BuildType buildID;
	bool ready = false;
	
	struct {
		str netname, skin, className;
		uint8 gender, handicap, unlagged, respawnOnFire, ticsPerUpdate, connectionType;
		uint32 color, aimdist, railcolor;
	} userinfo;
	
	// Check signature
	{
		uint32 sig;
		
		stream >> sig;
		if (sig != g_demoSignature) {
			error (fmt (tr ("'%1' is not a Zandronum demo file!"), path));
			return 3;
		}
	}
	
	// Zandronum stores CLD_DEMOLENGTH after the signature. This is also the
	// first demo enumerator, so we can determine the offset (which is variable!)
	// from this byte
	stream >> offset
	       >> length;
	
	// Read the demo header and get data
	for (;;) {
		uint8 header;
		stream >> header;
		
		if (header == DemoBodyStart + offset) {
			ready = true;
			break;
		} elif (header == DemoVersion + offset) {
			stream >> zanversionID;
			zanversion = readString (stream);
			
			if (!zanversion.startsWith ("1.1-") && !zanversion.startsWith ("1.1.1-")) {
				uint8 a;
				stream >> a;
				buildID = (BuildType) a;
			} else {
				// Assume a release build if not supplied. The demo only got the
				// "ZCLD" signature in the 1.1 release build, 1.1.1 had no testing
				// binaries and the build ID is included in 1.2 onward.
				buildID = ReleaseBuild;
			}
			
			stream >> longSink; // rng seed - we don't need it
		} elif (header == DemoUserInfo + offset) {
			userinfo.netname = readString (stream);
			stream >> userinfo.gender
			       >> userinfo.color
			       >> userinfo.aimdist;
			userinfo.skin = readString (stream);
			stream >> userinfo.railcolor
			       >> userinfo.handicap
			       >> userinfo.unlagged
			       >> userinfo.respawnOnFire
			       >> userinfo.ticsPerUpdate
			       >> userinfo.connectionType;
			userinfo.className = readString (stream);
		} elif (header == DemoWads + offset) {
			str sink;
			stream >> numWads;
			
			for (uint8 i = 0; i < numWads; ++i) {
				str wad = readString (stream);
				wads << wad;
			}
			
			// The demo has two checksum strings. We're not interested
			// in them though. Down the sink they go...
			for (int i = 0; i < 2; ++i)
				sink = readString (stream);
		} else {
			error (fmt (tr ("Unknown header %1!\n"), (int) header));
			return 3;
		}
	}

	if (!ready) {
		error (fmt (tr ("Incomplete demo header in '%s'!"), path));
		return 3;
	}
	
	if (!isKnownVersion (zanversion)) {
		UnknownVersionPrompt* prompt = new UnknownVersionPrompt (path, zanversion,
			(buildID == ReleaseBuild));
		
		if (!prompt->exec())
			return 6;
		
		if (!isKnownVersion (zanversion)) {
			error (tr ("Failure in configuration! This shouldn't happen."));
			return 6;
		}
	}
	
	str binarypath = cfg::binaryPaths [zanversion].toString();
	
	if (binarypath.isEmpty()) {
		error (fmt (tr ("No binary path specified for Zandronum version %1!"), zanversion));
		return 7;
	}
	
	str iwadpath;
	list<str> pwadpaths;
	
	// Find the WADs
	for (const str& wad : wads) {
		str path = findWAD (wad);
		
		// IWAD names can appear in uppercase too. Linux is case-sensitive, so..
		if (&wad == &wads[0] && path.isEmpty())
			path = findWAD (wad.toUpper());
		
		if (path.isEmpty()) {
			error (fmt (tr ("Couldn't find %1!"), wad));
			return 8;
		}
		
		if (&wad == &wads[0])
			iwadpath = path;
		else
			pwadpaths << path;
	}
	
	if (!cfg::noprompt) {
		str pwadtext;
		
		for (const str& pwad : wads) {
			if (&pwad == &wads[0])
				continue; // skip the IWAD
			
			if (!pwadtext.isEmpty())
				pwadtext += "<br />";
			
			pwadtext += pwad;
		}
		
		QDialog* dlg = new QDialog;
		Ui_DemoPrompt ui;
		ui.setupUi (dlg);
		ui.demoNameLabel->setText (basename (path));
		ui.demoRecorder->setText (uncolorize (userinfo.netname));
		ui.versionLabel->setText (zanversion);
		ui.iwadLabel->setText (wads[0]);
		ui.pwadsLabel->setText (pwadtext);
		dlg->setWindowTitle (str (APPNAME) + " " + versionString());
		
		if (!dlg->exec())
			return 1;
	}

	QStringList cmdlineList ({
		"-playdemo", path,
		"-iwad", iwadpath,
	});
	
	if (pwadpaths.size() > 0) {
		cmdlineList << "-file";
		cmdlineList << pwadpaths;
	}
	
	print ("Executing: %1 %2\n", binarypath, cmdlineList.join (" "));
	QProcess* proc = new QProcess;
	proc->start (binarypath, cmdlineList);
	proc->waitForFinished (-1);
	return 0;
}