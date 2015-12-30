/*
** Copyright (c) Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from spinware without specific prior written permission.
**
** SPINWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** SPINWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _spinware_page_h_
#define _spinware_page_h_

#include <QColor>
#include <QFuture>
#include <QTimer>
#include <QWidget>
#if QT_VERSION >= 0x050000
#include <QtConcurrent>
#endif
#include <QtCore>

#include "ui_spinware_page.h"

class spinware_page: public QWidget
{
  Q_OBJECT

 public:
  spinware_page(QWidget *parent);
  ~spinware_page();
  bool isFinished(void) const;

 private:
  QFuture<bool> m_future;
  QFutureWatcher<bool> m_futureWatcher;
  QTimer m_timer;
  Ui_spinware_page m_ui;
  bool m_storeOperation;
  qint64 m_pid;
  bool list(const QString &device,
	    const QString &mt,
	    const QString &tar,
	    const bool compute_content_size);
  bool operation(const QString &device,
		 const QString &mt,
		 const QString &command);
  bool read(const QString &device,
	    const QString &mt,
	    const QString &output,
	    const QString &tar,
	    const int number);
  bool write(const QString &device,
	     const QString &input,
	     const QString &mt,
	     const QString &tar,
	     const bool individual);
  void appendStatus(const QColor &color,
		    const QString &operation,
		    const QString &status);

 private slots:
  void slotAbort(void);
  void slotColoredStatus(const QString &operation,
			 const QString &status);
  void slotExport(void);
  void slotFinished(const QString &operation, const bool ok);
  void slotFutureFinished(void);
  void slotHighlightPaths(void);
  void slotList(void);
  void slotOperation(void);
  void slotRead(void);
  void slotSchedule(void);
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
