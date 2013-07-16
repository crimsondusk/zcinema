#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>
#include "main.h"
#include "types.h"

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
	
private:
	Ui_ConfigBox* ui;
};

#endif // CONFIG_H