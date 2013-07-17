#include <QFile>
#include <QDataStream>
#include <QMessageBox>
#include <QProcess>
#include "demo.h"
#include "bytestream.h"
#include "misc.h"
#include "ui_demoprompt.h"

static const uint32 g_demoSignature = makeByteID( 'Z', 'C', 'L', 'D' );

// =============================================================================
// -----------------------------------------------------------------------------
static str tr( const char* msg ) {
	return QObject::tr( msg );
}

// =============================================================================
// -----------------------------------------------------------------------------
static void error( str msg ) {
	QMessageBox::critical( null, "Error", msg );
}

// =============================================================================
// -----------------------------------------------------------------------------
static int getVersionIndex( str ver ) {
	int i;
	
	for( i = 0; i < g_zanVersions.size(); ++i )
		if( g_zanVersions[i] == ver )
			return i;
	
	return -1;
}

// =============================================================================
// -----------------------------------------------------------------------------
static str findWAD( str name ) {
	QSettings cfg;
	list<var> paths = cfg.value( "wads/paths", list<var>() ).toList();
	
	if( paths.size() == 0 ) {
		error( tr( "No WAD paths configured!" ));
		exit( 9 );
	}
	
	for( var it : paths ) {
		str fullpath = fmt( "%1/%2", it.toString(), name );
		QFile f( fullpath );
		
		if( f.exists() )
			return fullpath;
	}
	
	return "";
}

// =============================================================================
// -----------------------------------------------------------------------------
int launchDemo( str path ) {
	FILE* fp = fopen( path.toStdString().c_str(), "r" );
	
	if( !fp ) {
		error( fmt( tr( "Couldn't open '%1' for reading: %2" ), path, strerror( errno )));
		return 1;
	}
	
	fseek( fp, 0, SEEK_END );
	const size_t fsize = ftell( fp );
	rewind( fp );
	
	char* buf = new char[fsize];
	
	if( fread( buf, 1, fsize, fp ) != fsize ) {
		error( tr( "I/O error" ));
		delete[] buf;
		return 2;
	}
	
	Bytestream s( buf, fsize );
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
		
		if( !s.readLong( sig ) || sig != g_demoSignature ) {
			error( fmt( tr( "'%1' is not a Zandronum demo file!" ), path ));
			return 3;
		}
	}
	
	// Zandronum stores CLD_DEMOLENGTH after the signature. This is also the
	// first demo enumerator, so we can determine the offset (which is variable!)
	// from this byte
	s.readByte( offset );
	s.readLong( length );
	
	uint16 zanversionID, numWads;
	uint32 longSink;
	str zanversion;
	list<str> wads;
	bool ready = false;
	
	for( ;; ) {
		uint8 header;
		if( !s.readByte( header ))
			break;
		
		if( header == DemoBodyStart + offset ) {
			ready = true;
			break;
		} elif( header == DemoVersion + offset ) {
			s.readShort( zanversionID );
			s.readString( zanversion );
			s.readLong( longSink ); // rng seed - we don't need it
		} elif( header == DemoUserInfo + offset ) {
			s.readString( userinfo.netname );
			s.readByte( userinfo.gender );
			s.readLong( userinfo.color );
			s.readLong( userinfo.aimdist );
			s.readString( userinfo.skin );
			s.readLong( userinfo.railcolor );
			s.readByte( userinfo.handicap );
			s.readByte( userinfo.unlagged );
			s.readByte( userinfo.respawnOnFire );
			s.readByte( userinfo.ticsPerUpdate );
			s.readByte( userinfo.connectionType );
			s.readString( userinfo.className );
		} elif( header == DemoWads + offset ) {
			s.readShort( numWads );
			
			for( uint8 i = 0; i < numWads; ++i ) {
				str wad;
				s.readString( wad );
				wads << wad;
			}
			
			// The demo has two checksum strings. We're not interested
			// in them though.
			str sink;
			for( int i = 0; i < 2; ++i )
				s.readString( sink );
		} else {
			error( fmt( tr( "Unknown header %1!\n" ), (int) header ));
			return 4;
		}
	}
	
	if( !ready ) {
		error( fmt( tr( "Incomplete demo header in '%s'!" ), path ));
		return 5;
	}
	
	int i = getVersionIndex( zanversion );
	if( i == -1 ) {
		error( fmt( tr( "Unknown Zandronum version %1!\n" ), zanversion ));
		return 6;
	}
	
	QSettings cfg;
	str binarypath = cfg.value( binaryConfigName( zanversion )).toString();
	
	if( binarypath.isEmpty() ) {
		error( fmt( tr( "No binary path specified for Zandronum version %1!\n" ), zanversion ));
		return 7;
	}
	
	str iwad, iwadpath;
	list<str> pwads, pwadpaths;
	
	// Find the WADs
	for( const str& wad : wads ) {
		str path = findWAD( wad );
		
		// IWAD names can appear in uppercase too. Linux is case-sensitive, so..
		if( &wad == &wads[0] && path.isEmpty() )
			path = findWAD( wad.toUpper() );
		
		if( path.isEmpty() ) {
			error( fmt( tr( "Couldn't find %1!\n" ), wad ));
			return 8;
		}
		
		if( &wad == &wads[0] ) {
			iwadpath = path;
			iwad = wad;
		} else {
			pwadpaths << path;
			pwads << wad;
		}
	}
	
	if( !cfg.value( "nodemoprompt", false ).toBool() ) {
		str pwadtext;
		for( const str& pwad : pwads ) {
			if( !pwadtext.isEmpty() )
				pwadtext += "<br />";
			
			pwadtext += pwad;
		}
		
		QDialog* dlg = new QDialog;
		Ui_DemoPrompt ui;
		ui.setupUi( dlg );
		ui.demoNameLabel->setText( basename( path ));
		ui.demoRecorder->setText( userinfo.netname );
		ui.versionLabel->setText( zanversion );
		ui.iwadLabel->setText( wads[0] );
		ui.pwadsLabel->setText( pwadtext );
		
		if( !dlg->exec() )
			return 1;
	}
	
	print( "binary: %1\n", binarypath );
	print( "iwad: %1\npwads: %2\n", iwadpath, pwadpaths );
	
	QStringList cmdlineList ({
		"-playdemo", path,
		"-iwad", iwadpath,
	});
	
	if( pwadpaths.size() > 0 ) {
		cmdlineList << "-file";
		cmdlineList << pwadpaths;
	}
	
	print( "commandline: %1 %2\n", binarypath, cmdlineList.join( " " ));
	QProcess* proc = new QProcess;
	proc->start( binarypath, cmdlineList );
	proc->waitForFinished( -1 );
	return 0;
}