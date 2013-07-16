#include "types.h"

int main() {
	print ("Hello world! This is " APPNAME " %1\n", versionString() );
}

QString versionString() {
	str text = fmt( "v%1.%2", VERSION_MAJOR, VERSION_MINOR );
#if VERSION_PATCH != 0
	text += fmt( ".%3", VERSION_PATCH );
#endif
	
#if BUILD_ID == BUILD_INTERNAL
	text += "-intern";
#elif BUILD_ID == BUILD_ALPHA
	text += "-alpha";
#elif BUILD_ID == BUILD_BETA
	text += "-beta";
#elif BUILD_ID == BUILD_RC
	text += fmt( "-rc%1", RC_ID );
#elif BUILD_ID == BUILD_RELEASE
	text += "-rel";
#else
# error Invalid build code!
#endif // BUILD_ID
	
	return text;
}