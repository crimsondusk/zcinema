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

#ifndef ZCINEMA_VERSIONEDITOR_H
#define ZCINEMA_VERSIONEDITOR_H

#include <QDialog>
#include "types.h"

class QCheckBox;
class Ui_VersionEditor;
class Ui_AddVersion;

class VersionEditor : public QDialog {
	Q_OBJECT
	
public:
	enum {
		NameColumn,
		ReleaseColumn,
		PathColumn
	};
	
	explicit VersionEditor (QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~VersionEditor();
	
public slots:
	void add();
	void edit();
	void remove();
	void clear();
	void saveChanges();
	void selectionChanged (int row);
	
private:
	Ui_VersionEditor* m_ui;
	
	void initVersions();
	void addVersion (str name, str path, bool isRelease);
	QCheckBox* getReleaseCheckbox (int i);
};

class AddVersionPrompt : public QDialog {
	Q_OBJECT
	
public:
	explicit AddVersionPrompt (QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~AddVersionPrompt();
	
public:
	str name();
	str path();
	bool release();
	void setName (const str& a);
	void setPath (const str& a);
	void setRelease (bool a);
	
public slots:
	void findPath();
	void fieldsChanged();
	
private:
	Ui_AddVersion* m_ui;
};

#endif // ZCINEMA_VERSIONEDITOR_H