#include <QFile>
#include <QDataStream>
#include <QMessageBox>
#include <QProcess>
#include "demo.h"
#include "bytestream.h"
#include "misc.h"
#include "ui_demoprompt.h"
#include "prompts.h"

static const uint32 g_demoSignature = makeByteID ('Z', 'C', 'L', 'D');

// =============================================================================
// -----------------------------------------------------------------------------
str uncolorize (const str& in) {
	str out;
	int skip = 0;
	
	for (const qchar& c : in) {
		if (skip-- > 0)
			continue;
		
		if (c.toAscii() == '\034') {
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
	QSettings cfg;
	list<var> versions = getVersionsList();
	
	for (const var& it : versions)
		if (it.toString() == ver || it.toString() + 'M' == ver)
			return true;
	
	return false;
}

// =============================================================================
// -----------------------------------------------------------------------------
static str findWAD (str name) {
	QSettings cfg;
	list<var> paths = cfg.value ("wads/paths", list<var>()).toList();
	
	if (paths.size() == 0) {
		error (tr ("No WAD paths configured!"));
		
		// Cannot just return an empty string here since that'd trigger
		// another error prompt - skip ahead and exit.
		exit (9);
	}
	
	for (const var& it : paths) {
		str fullpath = fmt ("%1/%2", it.toString(), name);
		QFile f (fullpath);
		
		if (f.exists())
			return fullpath;
	}
	
	return "";
}

// =============================================================================
// -----------------------------------------------------------------------------
int launchDemo (str path) {
	FILE* fp = fopen (path.toStdString().c_str(), "r");
	
	if (!fp) {
		error (fmt (tr ("Couldn't open '%1' for reading: %2"), path, strerror (errno)));
		return 1;
	}
	
	fseek (fp, 0, SEEK_END);
	const size_t fsize = ftell (fp);
	rewind (fp);
	
	char* buf = new char[fsize];
	
	if (fread (buf, 1, fsize, fp) != fsize) {
		error (tr ("I/O error"));
		delete[] buf;
		return 2;
	}

	Bytestream s (buf, fsize);
	delete[] buf;
	
	uint8 offset;
	uint32 length;
	
	struct {
		str netname, skin, className;
		uint8 gender, handicap, unlagged, respawnOnFire, ticsPerUpdate, connectionType;
		uint32 color, aimdist, railcolor;
	} userinfo;
	
	// Check signature
	{
		uint32 sig;
		
		if (!s.readLong (sig) || sig != g_demoSignature) {
			error (fmt (tr ("'%1' is not a Zandronum demo file!"), path));
			return 3;
		}
	}
	
	// Zandronum stores CLD_DEMOLENGTH after the signature. This is also the
	// first demo enumerator, so we can determine the offset (which is variable!)
	// from this byte
	s.readByte (offset);
	s.readLong (length);

	uint16 zanversionID, numWads;
	uint32 longSink;
	str zanversion;
	list<str> wads;
	bool ready = false;
	uint8 buildID;
	
	// Read the demo header and get data
	for (;;) {
		uint8 header;
		
		if (!s.readByte (header))
			break;
		
		if (header == DemoBodyStart + offset) {
			ready = true;
			break;
		} elif (header == DemoVersion + offset) {
			s.readShort (zanversionID);
			s.readString (zanversion);
			
			if (zanversion.left (4) != "1.1-" && zanversion.left (6) != "1.1.1-")
				s.readByte (buildID);
			else
				buildID = 1;
			
			s.readLong (longSink);  // rng seed - we don't need it
		} elif (header == DemoUserInfo + offset) {
			s.readString (userinfo.netname);
			s.readByte (userinfo.gender);
			s.readLong (userinfo.color);
			s.readLong (userinfo.aimdist);
			s.readString (userinfo.skin);
			s.readLong (userinfo.railcolor);
			s.readByte (userinfo.handicap);
			s.readByte (userinfo.unlagged);
			s.readByte (userinfo.respawnOnFire);
			s.readByte (userinfo.ticsPerUpdate);
			s.readByte (userinfo.connectionType);
			s.readString (userinfo.className);
		} elif (header == DemoWads + offset) {
			str sink;
			s.readShort (numWads);
			
			for (uint8 i = 0; i < numWads; ++i) {
				str wad;
				s.readString (wad);
				wads << wad;
			}
			
			// The demo has two checksum strings. We're not interested
			// in them though.
			for (int i = 0; i < 2; ++i)
				s.readString (sink);
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
		UnknownVersionPrompt* prompt = new UnknownVersionPrompt (path, zanversion, (buildID == 1));
		if (!prompt->exec())
			return 6;
		
		if (!isKnownVersion (zanversion)) {
			error (tr ("Failure in configuration! This shouldn't happen."));
			return 6;
		}
	}

	QSettings cfg;
	str binarypath = cfg.value (binaryConfigName (zanversion)).toString();

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
	
	if (!cfg.value ("nodemoprompt", false).toBool()) {
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
		dlg->setWindowTitle (fmt (APPNAME " %1", versionString()));
		
		if (!dlg->exec())
			return 1;
	}

	QStringList cmdlineList ( {
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