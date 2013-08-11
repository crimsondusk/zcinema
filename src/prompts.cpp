/*
 *  ZanDemo: Zandronum demo launcher
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