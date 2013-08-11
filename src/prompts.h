#ifndef PROMPTS_H
#define PROMPTS_H

#include <QDialog>
#include "main.h"
#include "types.h"

class QAbstractButton;
class Ui_UnknownVersion;

class UnknownVersionPrompt : public QDialog {
	Q_OBJECT
	
public:
	explicit UnknownVersionPrompt (str fileName, str binaryName, bool isRelease,
		QWidget* parent = null, Qt::WindowFlags f = 0);
	virtual ~UnknownVersionPrompt();
	
public slots:
	void findBinary();
	void addBinary();
	
private:
	Ui_UnknownVersion* ui;
	QString m_binaryString;
	bool m_isRelease;
};

#endif // PROMPTS_H