#include <QLabel>
#include <QFileDialog>
#include <QFormLayout>
#include "config.h"
#include "ui_configbox.h"
#include "misc.h"

// =============================================================================
// -----------------------------------------------------------------------------
class FindPathButton : public QPushButton {
public:
	explicit FindPathButton( QWidget* parent = null ) : QPushButton( parent ) {
		setText( "..." );
	}
	
	QLineEdit* editWidget() const { return m_editWidget; }
	void setEditWidget( QLineEdit* edit ) { m_editWidget = edit; }
	
private:
	QLineEdit* m_editWidget;
};

// =============================================================================
// -----------------------------------------------------------------------------
ConfigBox::ConfigBox( QWidget* parent, Qt::WindowFlags f ) : QDialog( parent, f ) {
	ui = new Ui_ConfigBox;
	ui->setupUi( this );
	QFormLayout* layout = new QFormLayout( ui->zandronumVersions );
	
	for( str ver : g_zanVersions ) {
		QLabel* lb = new QLabel( ver + ":" );
		QLineEdit* ledit = new QLineEdit;
		FindPathButton* btn = new FindPathButton;
		btn->setEditWidget( ledit );
		
		QWidget* wdg = new QWidget;
		QHBoxLayout* leditLayout = new QHBoxLayout( wdg );
		leditLayout->addWidget( ledit );
		leditLayout->addWidget( btn );
		
		m_zanBinaries << ledit;
		layout->addRow( lb, wdg );
		connect( btn, SIGNAL( clicked() ), this, SLOT( findZanBinary() ));
	}
	
	initFromSettings();
	
	connect( ui->wad_add, SIGNAL( clicked() ), this, SLOT( addPath() ));
	connect( ui->wad_pathEntry, SIGNAL( returnPressed() ), this, SLOT( addPath() ));
	connect( ui->wad_findPath, SIGNAL( clicked() ), this, SLOT( findPath() ));
	connect( ui->wad_del, SIGNAL( clicked() ), this, SLOT( delPath() ));
	connect( ui->buttonBox, SIGNAL( accepted() ), this, SLOT( okPressed() ));
	connect( ui->buttonBox, SIGNAL( rejected() ), this, SLOT( cancelPressed() ));
	setWindowTitle( fmt( APPNAME " %1", versionString()));
}

// =============================================================================
// -----------------------------------------------------------------------------
ConfigBox::~ConfigBox() {
	delete ui;
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::initFromSettings() {
	QSettings cfg;
	
	ui->wad_pathsList->clear();
	
	list<var> paths = cfg.value( "wads/paths", list<var>() ).toList();
	for( const var& it : paths )
		addPath( it.toString() );
	
	int i = 0;
	for( str ver : g_zanVersions )
		m_zanBinaries[i++]->setText( cfg.value( binaryConfigName( ver ), "" ).toString() );
	
	ui->noDemoPrompt->setChecked( cfg.value( "nodemoprompt", false ).toBool() );
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::addPath() {
	addPath( ui->wad_pathEntry->text() );
	ui->wad_pathEntry->clear();
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::addPath( str path ) {
	ui->wad_pathsList->addItem( path );
	QListWidgetItem* item = ui->wad_pathsList->item( ui->wad_pathsList->count() - 1 );
	item->setFlags( item->flags() | Qt::ItemIsEditable );
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::findPath() {
	str path = QFileDialog::getExistingDirectory( this );
	if( path.isEmpty() )
		return;
	
	ui->wad_pathEntry->setText( path );
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::delPath() {
	delete ui->wad_pathsList->currentItem();
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::findZanBinary() {
	FindPathButton* btn = dynamic_cast<FindPathButton*>( sender() );
	str path;
	
	if( !btn )
		return;
	
	str filter;
#ifdef _WIN32
	filter = "Zandronum Binaries (zandronum.exe)(zandronum.exe);;All files (*.*)(*.*)";
#else
	filter = "Zandronum Binaries (zandronum)(zandronum);;All files (*.*)(*.*)";
#endif
	
	if(( path = QFileDialog::getOpenFileName( this, QString(), QString(), filter )).isEmpty() )
		return;
	
	btn->editWidget()->setText( path );
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::okPressed() {
	QSettings cfg;
	list<var> wadPathList;
	
	for( int i = 0; i < ui->wad_pathsList->count(); ++i )
		wadPathList << ui->wad_pathsList->item( i )->text();
	
	cfg.setValue( "wads/paths", wadPathList );
	cfg.setValue( "nodemoprompt", ui->noDemoPrompt->isChecked() );
	
	int i = 0;
	for( str ver : g_zanVersions )
		cfg.setValue( binaryConfigName( ver ), m_zanBinaries[i++]->text() );
	
	accept();
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::cancelPressed() {
	reject();
}