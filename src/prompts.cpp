#include "prompts.h"
#include "ui_unknownversion.h"
#include "misc.h"
#include "build/moc_prompts.cpp"
#include "config.h"

// =============================================================================
// -----------------------------------------------------------------------------
UnknownVersionPrompt::UnknownVersionPrompt (
	str fileName,
	str binaryName,
	bool isRelease,
	QWidget* parent,
	Qt::WindowFlags f
) :
	QDialog (parent, f),
	m_binaryString (binaryName),
	m_isRelease (isRelease)
{
	ui = new Ui_UnknownVersion;
	ui->setupUi (this);
	
	// Replace the placeholders
	str text = ui->m_description->text();
	text.replace ("<DEMO>", basename (fileName));
	text.replace ("<VERSION>", binaryName);
	ui->m_description->setText (text);
	
	connect (ui->m_addVersion, SIGNAL (clicked(bool)), this, SLOT (addBinary()));
	connect (ui->m_findBinary, SIGNAL (clicked(bool)), this, SLOT (findBinary()));
}

// =============================================================================
// -----------------------------------------------------------------------------
UnknownVersionPrompt::~UnknownVersionPrompt() {
	delete ui;
}

// =============================================================================
// -----------------------------------------------------------------------------
void UnknownVersionPrompt::addBinary() {
	addVersion (m_binaryString, m_isRelease, ui->m_binaryPath->text());
	accept();
}

// =============================================================================
// -----------------------------------------------------------------------------
void UnknownVersionPrompt::findBinary() {
	str path = ConfigBox::getBinaryPath (this);
	
	if (path.isEmpty())
		return;
	
	ui->m_binaryPath->setText (path);
}