/*
 *  ZCinema: Zandronum demo launcher
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

#include <QLabel>
#include <QFileDialog>
#include <QFormLayout>
#include <QProgressBar>
#include <QMessageBox>
#include "config.h"
#include "ui_configbox.h"
#include "misc.h"
#include "demo.h"
#include "build/moc_config.cpp"
#include "versionEditor.h"

CONFIG (Bool, noprompt,      false)
CONFIG (List, devBuildNames, cfg::List())
CONFIG (List, releaseNames,  cfg::List())
CONFIG (List, wadpaths,      cfg::List())
CONFIG (Map,  binaryPaths,   cfg::Map())

// =============================================================================
// -----------------------------------------------------------------------------
class FindPathButton : public QPushButton {
public:
	explicit FindPathButton (QWidget* parent = null) : QPushButton (parent) {
		setText ("...");
	}

	QLineEdit* editWidget() const {
		return m_editWidget;
	}
	void setEditWidget (QLineEdit* edit) {
		m_editWidget = edit;
	}

private:
	QLineEdit* m_editWidget;
};

// =============================================================================
// -----------------------------------------------------------------------------
ConfigBox::ConfigBox (QWidget* parent, Qt::WindowFlags f) :
	QDialog (parent, f),
	ui (new Ui_ConfigBox),
	m_releaseLayout (null),
	m_testLayout (null)
{
	ui->setupUi (this);
	
	initVersions();
	initFromSettings();
	
	connect (ui->wad_add, SIGNAL (clicked()), this, SLOT (addPath()));
	connect (ui->wad_pathEntry, SIGNAL (returnPressed()), this, SLOT (addPath()));
	connect (ui->wad_findPath, SIGNAL (clicked()), this, SLOT (findPath()));
	connect (ui->wad_del, SIGNAL (clicked()), this, SLOT (delPath()));
	connect (ui->buttonBox, SIGNAL (clicked (QAbstractButton*)), this,
	         SLOT (buttonPressed (QAbstractButton*)));
	connect (ui->m_editVersions, SIGNAL (clicked()), this, SLOT (editBinaries()));
	connect (ui->m_editVersions_2, SIGNAL (clicked()), this, SLOT (editBinaries()));
	setWindowTitle (versionSignature());
}

// =============================================================================
// -----------------------------------------------------------------------------
ConfigBox::~ConfigBox() {
	delete ui;
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::initVersions() {
	if (m_releaseLayout == null) {
		m_releaseLayout = new QFormLayout;
		m_testLayout = new QFormLayout;
		ui->zandronumVersions->setLayout (m_releaseLayout);
		ui->betaVersions->setLayout (m_testLayout);
	} else {
		// re-init, clear the layouts
		for (QWidget* w : m_binaryLayoutWidgets)
			delete w;
		
		m_binaryLayoutWidgets.clear();
		m_zanBinaries.clear();
	}
	
	for (const QVariant& ver : cfg::devBuildNames)
		addVersion (ver.toString(), false);
	
	for (const QVariant& rel : cfg::releaseNames)
		addVersion (rel.toString(), true);
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::addVersion (const str& name, bool isRelease) {
	QLabel* lb = new QLabel (name + ":");
	QLineEdit* ledit = new QLineEdit;
	FindPathButton* btn = new FindPathButton;
	btn->setEditWidget (ledit);
	
	QWidget* wdg = new QWidget;
	QHBoxLayout* leditLayout = new QHBoxLayout (wdg);
	leditLayout->addWidget (ledit);
	leditLayout->addWidget (btn);
	
	m_zanBinaries << ledit;
	connect (btn, SIGNAL (clicked()), this, SLOT (findZanBinary()));
	
	if (isRelease)
		m_releaseLayout->addRow (lb, wdg);
	else
		m_testLayout->addRow (lb, wdg);
	
	m_binaryLayoutWidgets << ledit << btn << wdg << lb;
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::initFromSettings() {
	ui->wad_pathsList->clear();
	
	for (const QVariant& it : cfg::wadpaths)
		addPath (it.toString());
	
	int i = 0;
	
	for (const QVariant& ver : getVersions())
		m_zanBinaries[i++]->setText (cfg::binaryPaths[ver.toString()].toString());
	
	ui->noDemoPrompt->setChecked (cfg::noprompt);
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::saveSettings() {
	QList<QVariant> wadPathList;
	
	for (int i = 0; i < ui->wad_pathsList->count(); ++i)
		wadPathList << ui->wad_pathsList->item (i)->text();
	
	cfg::wadpaths = wadPathList;
	cfg::noprompt = ui->noDemoPrompt->isChecked();
	
	int i = 0;
	for (const QVariant& ver : getVersions())
		cfg::binaryPaths[ver.toString()] = m_zanBinaries[i++]->text();
	
	cfg::save();
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::addPath() {
	addPath (ui->wad_pathEntry->text());
	ui->wad_pathEntry->clear();
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::addPath (str path) {
	ui->wad_pathsList->addItem (path);
	QListWidgetItem* item = ui->wad_pathsList->item (ui->wad_pathsList->count() - 1);
	item->setFlags (item->flags() | Qt::ItemIsEditable);
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::findPath() {
	str path = QFileDialog::getExistingDirectory (this);

	if (path.isEmpty())
		return;

	ui->wad_pathEntry->setText (path);
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::delPath() {
	delete ui->wad_pathsList->currentItem();
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::findZanBinary() {
	FindPathButton* btn = dynamic_cast<FindPathButton*> (sender());
	
	if (!btn)
		return;
	
	str path = getBinaryPath (this);
	if (path.isEmpty())
		return;
	
	btn->editWidget()->setText (path);
}

// =============================================================================
// -----------------------------------------------------------------------------
str ConfigBox::getBinaryPath (QWidget* parent) {
	str path;
	const str filter =
#ifdef _WIN32
		"Zandronum Binaries (zandronum.exe)(zandronum.exe);;All files (*.*)(*.*)";
#else
		"Zandronum Binaries (zandronum)(zandronum);;All files (*.*)(*.*)";
#endif
	
	return QFileDialog::getOpenFileName (parent, QString(), QString(), filter);
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::buttonPressed (QAbstractButton* btn) {
	if (btn == ui->buttonBox->button (QDialogButtonBox::Ok)) {
		saveSettings();
		accept();
	} elif (btn == ui->buttonBox->button (QDialogButtonBox::Cancel))
		reject();
	elif (btn == ui->buttonBox->button (QDialogButtonBox::Apply))
		saveSettings();
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::editBinaries() {
	VersionEditor* dlg = new VersionEditor (this);
	
	if (dlg->exec()) {
		dlg->saveChanges();
		initVersions();
	}
}