#include "versionEditor.h"
#include "ui_versionEditor.h"
#include "ui_addversion.h"
#include "config.h"
#include "misc.h"
#include "build/moc_versionEditor.cpp"

EXTERN_CONFIG (Map,  binaryPaths)
EXTERN_CONFIG (List, devBuildNames)
EXTERN_CONFIG (List, releaseNames)

// =============================================================================
// -----------------------------------------------------------------------------
VersionEditor::VersionEditor (QWidget* parent, Qt::WindowFlags f) :
	QDialog (parent, f),
	m_ui (new Ui_VersionEditor)
{
	m_ui->setupUi (this);
	
	connect (m_ui->m_versions, SIGNAL (currentCellChanged (int, int, int, int)),
	         this, SLOT (selectionChanged (int)));
	
	connect (m_ui->m_add,    SIGNAL (clicked (bool)), this, SLOT (add()));
	connect (m_ui->m_edit,   SIGNAL (clicked (bool)), this, SLOT (edit()));
	connect (m_ui->m_remove, SIGNAL (clicked (bool)), this, SLOT (remove()));
	connect (m_ui->m_clear,  SIGNAL (clicked (bool)), this, SLOT (clear()));
	
	initVersions();
}

// =============================================================================
// -----------------------------------------------------------------------------
VersionEditor::~VersionEditor() {
	delete m_ui;
}

// =============================================================================
// -----------------------------------------------------------------------------
void VersionEditor::initVersions() {
	int i = 0;
	
	for (const QVariant& ver : (cfg::releaseNames + cfg::devBuildNames)) {
		bool isRelease = (i < cfg::releaseNames.size());
		addVersion (ver.toString(), cfg::binaryPaths[ver.toString()].toString(), isRelease);
		
		++i;
	}
}

// =============================================================================
// -----------------------------------------------------------------------------
void VersionEditor::add() {
	AddVersionPrompt* dlg = new AddVersionPrompt (this);
	if (!dlg->exec())
		return;
	
	addVersion (dlg->name(), dlg->path(), dlg->release());
}

// =============================================================================
// -----------------------------------------------------------------------------
void VersionEditor::edit() {
	QTableWidget* const vers = m_ui->m_versions;
	int row = vers->currentRow();
	
	if (row < 0)
		return;
	
	AddVersionPrompt* dlg = new AddVersionPrompt (this);
	dlg->setName (vers->item (row, NameColumn)->text());
	dlg->setPath (vers->item (row, PathColumn)->text());
	dlg->setRelease (getReleaseCheckbox(row)->isChecked());
	
	if (!dlg->exec())
		return;
	
	addVersion (dlg->name(), dlg->path(), dlg->release());
}

// =============================================================================
// -----------------------------------------------------------------------------
void VersionEditor::remove() {
	QTableWidget* const vers = m_ui->m_versions;
	
	str name = vers->item (vers->currentRow(), NameColumn)->text();
	if (!confirm (fmt (tr ("Really remove version %1?"), name)))
		return;
	
	vers->removeRow (vers->currentRow());
}

// =============================================================================
// -----------------------------------------------------------------------------
void VersionEditor::clear() {
	if (!confirm (tr ("Really remove all versions?")))
		return;
	
	m_ui->m_versions->clear();
}

// =============================================================================
// -----------------------------------------------------------------------------
void VersionEditor::addVersion (str name, str path, bool isRelease) {
	QTableWidget* const vers = m_ui->m_versions;
	
	int row = vers->rowCount();
	vers->insertRow (row);
	vers->setItem (row, NameColumn, new QTableWidgetItem);
	vers->setItem (row, PathColumn, new QTableWidgetItem);
	vers->item (row, NameColumn)->setText (name);
	vers->item (row, PathColumn)->setText (path);
	
	QCheckBox* cb = new QCheckBox;
	cb->setChecked (isRelease);
	vers->setCellWidget (row, ReleaseColumn, cb);
}

// =============================================================================
// -----------------------------------------------------------------------------
void VersionEditor::saveChanges() {
	QTableWidget* const vers = m_ui->m_versions;
	
	cfg::devBuildNames.clear();
	cfg::releaseNames.clear();
	cfg::binaryPaths.clear();
	
	for (int i = 0; i < vers->rowCount(); ++i) {
		const QCheckBox* cb = getReleaseCheckbox (i);
		bool isRelease = cb->isChecked();
		const str name = vers->item (i, NameColumn)->text(),
		          path = vers->item (i, PathColumn)->text();
		
		if (isRelease)
			cfg::releaseNames << QVariant (name);
		else
			cfg::devBuildNames << QVariant (name);
		
		cfg::binaryPaths[name] = path;
	}
	
	cfg::save();
}

// =============================================================================
// -----------------------------------------------------------------------------
void VersionEditor::selectionChanged (int row) {
	bool ok = (row != -1);
	m_ui->m_edit->setEnabled (ok);
	m_ui->m_remove->setEnabled (ok);
}

// =============================================================================
// -----------------------------------------------------------------------------
QCheckBox* VersionEditor::getReleaseCheckbox (int i) {
	return static_cast<QCheckBox*> (m_ui->m_versions->cellWidget (i, ReleaseColumn));
}

// =============================================================================
// -----------------------------------------------------------------------------
AddVersionPrompt::AddVersionPrompt (QWidget* parent, Qt::WindowFlags f) :
	QDialog (parent, f),
	m_ui (new Ui_AddVersion)
{
	m_ui->setupUi (this);
	connect (m_ui->m_binaryName, SIGNAL (textChanged (QString)), this, SLOT (fieldsChanged()));
	connect (m_ui->m_binaryPath, SIGNAL (textChanged (QString)), this, SLOT (fieldsChanged()));
	connect (m_ui->m_findBinary, SIGNAL (clicked (bool)), this, SLOT (findPath()));
}

// =============================================================================
// -----------------------------------------------------------------------------
AddVersionPrompt::~AddVersionPrompt() {
	delete m_ui;
}

// =============================================================================
// -----------------------------------------------------------------------------
void AddVersionPrompt::findPath() {
	str path = ConfigBox::getBinaryPath (this);
	if (!path.isEmpty())
		m_ui->m_binaryPath->setText (path);
}

// =============================================================================
// -----------------------------------------------------------------------------
str AddVersionPrompt::name() {
	return m_ui->m_binaryName->text();
}

// =============================================================================
// -----------------------------------------------------------------------------
str AddVersionPrompt::path() {
	return m_ui->m_binaryPath->text();
}

// =============================================================================
// -----------------------------------------------------------------------------
bool AddVersionPrompt::release() {
	return m_ui->m_release->isChecked();
}

// =============================================================================
// -----------------------------------------------------------------------------
void AddVersionPrompt::setName (const str& a) {
	m_ui->m_binaryName->setText (a);
}

// =============================================================================
// -----------------------------------------------------------------------------
void AddVersionPrompt::setPath (const str& a) {
	m_ui->m_binaryPath->setText (a);
}

// =============================================================================
// -----------------------------------------------------------------------------
void AddVersionPrompt::setRelease (bool a) {
	m_ui->m_release->setChecked (a);
}

// =============================================================================
// -----------------------------------------------------------------------------
void AddVersionPrompt::fieldsChanged() {
	bool ok = (!m_ui->m_binaryName->text().isEmpty()) && (!m_ui->m_binaryPath->text().isEmpty());
	m_ui->buttonBox->button (QDialogButtonBox::Ok)->setEnabled (ok);
}