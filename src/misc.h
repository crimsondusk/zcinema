#ifndef MISC_H
#define MISC_H

#include "types.h"

uint32 makeByteID (uint8 a, uint8 b, uint8 c, uint8 d);
str binaryConfigName (str ver);
str basename (str path);
list<var> getVersionsList();
list<var> getReleasesList();
void addVersion (str name, bool isRelease, str binaryPath);

// -----------------------------------------------------------------------------
// Templated clamp
template<class T> static inline T clamp (T a, T min, T max) {
	return (a > max) ? max : (a < min) ? min : a;
}

// Templated minimum
template<class T> static inline T min (T a, T b) {
	return (a < b) ? a : b;
}

// Templated maximum
template<class T> static inline T max (T a, T b) {
	return (a > b) ? a : b;
}

// Templated absolute value
template<class T> static inline T abs (T a) {
	return (a >= 0) ? a : -a;
}

#endif // MISC_H