#include <QLabel>
#include <QFileDialog>
#include <QFormLayout>
#include <QProgressBar>
#include <QMessageBox>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "config.h"
#include "ui_configbox.h"
#include "misc.h"
#include "demo.h"
#include "build/moc_config.cpp"

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
ConfigBox::ConfigBox (QWidget* parent, Qt::WindowFlags f) : QDialog (parent, f),
	m_nam (new QNetworkAccessManager (this)) {
	ui = new Ui_ConfigBox;
	ui->setupUi (this);
	ui->updateProgress->hide();
	ui->updateLabel->hide();

	initVersions();
	initFromSettings();

	connect (ui->wad_add, SIGNAL (clicked()), this, SLOT (addPath()));
	connect (ui->wad_pathEntry, SIGNAL (returnPressed()), this, SLOT (addPath()));
	connect (ui->wad_findPath, SIGNAL (clicked()), this, SLOT (findPath()));
	connect (ui->wad_del, SIGNAL (clicked()), this, SLOT (delPath()));
	connect (ui->buttonBox, SIGNAL (clicked (QAbstractButton*)), this,
			 SLOT (buttonPressed (QAbstractButton*)));
	setWindowTitle (fmt (APPNAME " %1", versionString()));
}

// =============================================================================
// -----------------------------------------------------------------------------
ConfigBox::~ConfigBox() {
	delete ui;
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::initVersions() {
	QFormLayout* releaseLayout = new QFormLayout (ui->zandronumVersions),
	*testLayout = new QFormLayout (ui->betaVersions);
	list<var> versions = getVersionsList(),
		releases = getReleasesList();
	
	for (const var& ver : versions) {
		str verstring = ver.toString();
		
		bool isRelease = false;
		for (const var& rel : releases) {
			if (rel.toString() == verstring) {
				isRelease = true;
				break;
			}
		}
		
		QLabel* lb = new QLabel (verstring + ":");
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
			releaseLayout->addRow (lb, wdg);
		else
			testLayout->addRow (lb, wdg);
	}
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::initFromSettings() {
	QSettings cfg;
	ui->wad_pathsList->clear();
	list<var> paths = cfg.value ("wads/paths", list<var>()).toList();
	
	for (const var & it : paths)
		addPath (it.toString());
	
	int i = 0;
	
	list<var> versions = getVersionsList();
	for (const var& ver : versions)
		m_zanBinaries[i++]->setText (cfg.value (binaryConfigName (ver.toString()), "").toString());
	
	ui->noDemoPrompt->setChecked (cfg.value ("nodemoprompt", false).toBool());
}

// =============================================================================
// -----------------------------------------------------------------------------
void ConfigBox::saveSettings() {
	QSettings cfg;
	list<var> wadPathList;
	
	for (int i = 0; i < ui->wad_pathsList->count(); ++i)
		wadPathList << ui->wad_pathsList->item (i)->text();
	
	cfg.setValue ("wads/paths", wadPathList);
	cfg.setValue ("nodemoprompt", ui->noDemoPrompt->isChecked());
	
	int i = 0;
	list<var> versions = getVersionsList();
	for (const var& ver : versions)
		cfg.setValue (binaryConfigName (ver.toString()), m_zanBinaries[i++]->text());
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