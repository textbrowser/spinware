#ifndef _spinware_h_
#define _spinware_h_

#include <QFuture>
#include <QMainWindow>
#include <QTimer>
#if QT_VERSION >= 0x050000
#include <QtConcurrent>
#endif
#include <QtCore>

#include "ui_spinware.h"

class spinware: public QMainWindow
{
  Q_OBJECT

 public:
  spinware(void);
  ~spinware();

 private:
  QFuture<void> m_future;
  QTimer m_timer;
  Ui_spinware m_ui;
  void list(const QString &device,
	    const QString &mt,
	    const QString &tar);
  void operation(const QString &device,
		 const QString &mt,
		 const QString &command);
  void read(const QString &device,
	    const QString &output,
	    const QString &tar);

 private slots:
  void slotAbort(void);
  void slotAbout(void);
  void slotFinished(const QString &widget_name, const bool ok);
  void slotHighlightPaths(void);
  void slotList(void);
  void slotOperation(void);
  void slotQuit(void);
  void slotRead(void);
  void slotSelectDirectory(void);
  void slotSelectExecutable(void);
  void slotStatus(const QString &widget_name,
		  const QString &status);

 signals:
  void finished(const QString &widget_name, const bool ok);
  void status(const QString &widget_name,
	      const QString &status);
};

#endif
