#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>
#include "main.h"
#include "types.h"

class QLineEdit;
class Ui_ConfigBox;

class ConfigBox : public QDialog {
	Q_OBJECT
	
public:
	explicit ConfigBox( QWidget* parent = null, Qt::WindowFlags f = 0 );
	virtual ~ConfigBox();
	void addPath( str path );
	void initFromSettings();
	
public slots:
	void addPath();
	void findPath();
	void delPath();
	void findZanBinary();
	void okPressed();
	void cancelPressed();
	
private:
	Ui_ConfigBox* ui;
	list<QLineEdit*> m_zanBinaries;
};

#endif // CONFIG_H