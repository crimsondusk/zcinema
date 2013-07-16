#include "config.h"
#include "ui_configbox.h"

ConfigBox::ConfigBox( QWidget* parent, Qt::WindowFlags f ) : QDialog( parent, f ) {
	ui = new Ui_ConfigBox;
	ui->setupUi( this );
}

ConfigBox::~ConfigBox() {
	delete ui;
}