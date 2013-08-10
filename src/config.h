#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>
#include "main.h"
#include "types.h"

class QNetworkReply;
class QHBoxLayout;
class QLabel;
class QAbstractButton;
class QLineEdit;
class Ui_ConfigBox;
class QNetworkAccessManager;

class ConfigBox : public QDialog {
	Q_OBJECT
	
public:
	explicit ConfigBox( QWidget* parent = null, Qt::WindowFlags f = 0 );
	virtual ~ConfigBox();
	void addPath( str path );
	void initFromSettings();
	void saveSettings();
	void initVersions();
	
	static str getBinaryPath (QWidget* parent);
	
public slots:
	void addPath();
	void findPath();
	void delPath();
	void findZanBinary();
	void buttonPressed( QAbstractButton* btn );
	
private:
	Ui_ConfigBox* ui;
	list<QLineEdit*> m_zanBinaries;
	QNetworkAccessManager* m_nam;
	QNetworkReply* m_reply;
};

#endif // CONFIG_H