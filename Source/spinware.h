#ifndef _spinware_h_
#define _spinware_h_

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
  QTimer m_timer;
  Ui_spinware m_ui;

 private slots:
  void slotHighlightPaths(void);
  void slotSelectDirectory(void);
  void slotSelectExecutable(void);
};

#endif
