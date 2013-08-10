#include "misc.h"

uint32 makeByteID (uint8 a, uint8 b, uint8 c, uint8 d) {
	return a | (b << 8) | (c << 16) | (d << 24);
}

// =============================================================================
// -----------------------------------------------------------------------------
str binaryConfigName (str ver) {
	return fmt ("binaries/%1", ver);
}

// =============================================================================
// -----------------------------------------------------------------------------
list<var> getVersionsList() {
	QSettings cfg;
	return cfg.value ("binarynames", list<var>()).toList();
}

// =============================================================================
// -----------------------------------------------------------------------------
list<var> getReleasesList() {
	QSettings cfg;
	return cfg.value ("releasenames", list<var>()).toList();
}

// =============================================================================
// -----------------------------------------------------------------------------
void addVersion (str name, bool isRelease, str binaryPath) {
	QSettings cfg;
	list<var> versions = getVersionsList();
	versions << var (name);
	cfg.setValue ("binarynames", versions);
	cfg.setValue (binaryConfigName (name), binaryPath);
	
	if (isRelease) {
		versions = getReleasesList();
		versions << var (name);
		cfg.setValue ("releasenames", versions);
	}
	
	cfg.sync();
}

// =============================================================================
// -----------------------------------------------------------------------------
str basename (str path) {
	long lastpos = path.lastIndexOf ("/");
	
	if (lastpos != -1)
		return path.mid (lastpos + 1);
	
	return path;
}
