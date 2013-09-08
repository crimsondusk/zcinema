#ifndef ZANDEMO_VERSIONEDITOR_H
#define ZANDEMO_VERSIONEDITOR_H

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

#endif // ZANDEMO_VERSIONEDITOR_H