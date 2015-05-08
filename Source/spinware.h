#ifndef _spinware_h_
#define _spinware_h_

#include <QFuture>
#include <QMainWindow>
#include <QTimer>

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

 private slots:
  void slotAbort(void);
  void slotFinished(const QString &widget_name);
  void slotHighlightPaths(void);
  void slotList(void);
  void slotQuit(void);
  void slotSelectDirectory(void);
  void slotSelectExecutable(void);
  void slotStatus(const QString &widget_name,
		  const QString &status);

 signals:
  void finished(const QString &widget_name);
  void status(const QString &widget_name,
	      const QString &status);
};

#endif
