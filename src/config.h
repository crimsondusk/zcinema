#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>

class Ui_ConfigBox;

class ConfigBox : public QDialog {
public:
	explicit ConfigBox( QWidget* parent = 0, Qt::WindowFlags f = 0 );
	virtual ~ConfigBox();
	
private:
	Ui_ConfigBox* ui;
};

#endif // CONFIG_H