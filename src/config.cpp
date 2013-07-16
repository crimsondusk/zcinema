#include <QFileDialog>
#include "config.h"
#include "ui_configbox.h"

ConfigBox::ConfigBox( QWidget* parent, Qt::WindowFlags f ) : QDialog( parent, f ) {
	ui = new Ui_ConfigBox;
	ui->setupUi( this );
	setWindowTitle( fmt( APPNAME " %1", versionString()));
	
	connect( ui->wad_add, SIGNAL( clicked() ), this, SLOT( addPath() ));
	connect( ui->wad_pathEntry, SIGNAL( returnPressed() ), this, SLOT( addPath() ));
	connect( ui->wad_findPath, SIGNAL( clicked() ), this, SLOT( findPath() ));
	connect( ui->wad_del, SIGNAL( clicked() ), this, SLOT( delPath() ));
}

ConfigBox::~ConfigBox() {
	delete ui;
}

void ConfigBox::initFromSettings() {
	ui->wad_pathsList->clear();
	
	list<var> paths = cfg->value( "wads/paths", list<var>() ).toList();
	for( const var& it : paths )
		addPath( it.toString() );
}

void ConfigBox::addPath() {
	addPath( ui->wad_pathEntry->text() );
	ui->wad_pathEntry->clear();
}

void ConfigBox::addPath( str path ) {
	ui->wad_pathsList->addItem( path );
}

void ConfigBox::findPath() {
	str path = QFileDialog::getExistingDirectory( this );
	if( path.isEmpty() )
		return;
	
	ui->wad_pathEntry->setText( path );
}

void ConfigBox::delPath() {
	delete ui->wad_pathsList->currentItem();
}
