#include <QApplication>
#include <QSettings>
#include "types.h"
#include "config.h"

const list<str> g_zanVersions ({
	"1.1",
});

// =============================================================================
// -----------------------------------------------------------------------------
int main( int argc, char* argv[] ) {
	QApplication app( argc, argv );
	app.setApplicationName( UNIXNAME );
	app.setOrganizationName( UNIXNAME );
	app.setApplicationVersion( versionString() );
	
	print( "Settings path: %1\n", QSettings().fileName() );
	
	for( int i = 1; i < argc; ++i ) {
		str arg = argv[i];
		
		if( arg == "--config" ) {
			ConfigBox dlg;
			return dlg.exec();
		}
	}
	
	return app.exec();
}

// =============================================================================
// -----------------------------------------------------------------------------
str versionString() {
	str text = fmt( "v%1.%2", VERSION_MAJOR, VERSION_MINOR );
#if VERSION_PATCH != 0
	text += fmt( ".%1", VERSION_PATCH );
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