#ifndef _spinware_h_
#define _spinware_h_

#include <QColor>
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
  QFutureWatcher<void> m_futureWatcher;
  QTimer m_timer;
  Ui_spinware m_ui;
  qint64 m_pid;
  void appendStatus(const QColor &color,
		    const QString &operation,
		    const QString &status);
  void list(const QString &device,
	    const QString &mt,
	    const QString &tar);
  void operation(const QString &device,
		 const QString &mt,
		 const QString &command);
  void read(const QString &device,
	    const QString &output,
	    const QString &tar);
  void write(const QString &device,
	     const QString &input,
	     const QString &mt,
	     const QString &tar);

 private slots:
  void slotAbort(void);
  void slotAbout(void);
  void slotColoredStatus(const QString &operation,
			 const QString &status);
  void slotFinished(const QString &operation, const bool ok);
  void slotFutureFinished(void);
  void slotHighlightPaths(void);
  void slotList(void);
  void slotOperation(void);
  void slotQuit(void);
  void slotRead(void);
  void slotSelectDirectory(void);
  void slotSelectExecutable(void);
  void slotStatus(const QString &operation,
		  const QString &status);
  void slotStore(void);

 signals:
  void coloredStatus(const QString &operation,
		     const QString &status);
  void finished(const QString &operation, const bool ok);
  void status(const QString &operation,
	      const QString &status);
};

#endif
