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

extern "C"
{
#include <signal.h>
#include <sys/types.h>
}

#include <QCloseEvent>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "spinware.h"

spinware::spinware(void):QMainWindow(0)
{
  m_ui.setupUi(this);
  m_pid = 0;
  connect(&m_futureWatcher,
	  SIGNAL(finished(void)),
	  this,
	  SLOT(slotFutureFinished(void)));
  connect(&m_timer,
	  SIGNAL(timeout(void)),
	  this,
	  SLOT(slotHighlightPaths(void)));
  connect(m_ui.abort,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAbort(void)));
  connect(m_ui.action_About,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotAbout(void)));
  connect(m_ui.action_Quit,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotQuit(void)));
  connect(m_ui.backward,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotOperation(void)));
  connect(m_ui.compute,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotList(void)));
  connect(m_ui.device_select,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectExecutable(void)));
  connect(m_ui.eject,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotOperation(void)));
  connect(m_ui.end,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotOperation(void)));
  connect(m_ui.erase,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotOperation(void)));
  connect(m_ui.export_invoice,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotExport(void)));
  connect(m_ui.export_store,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotExport(void)));
  connect(m_ui.forward,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotOperation(void)));
  connect(m_ui.input_select,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectDirectory(void)));
  connect(m_ui.list_list,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotList(void)));
  connect(m_ui.load,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotOperation(void)));
  connect(m_ui.mt_select,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectExecutable(void)));
  connect(m_ui.operations_custom_command,
	  SIGNAL(returnPressed(void)),
	  this,
	  SLOT(slotOperation(void)));
  connect(m_ui.output_select,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectDirectory(void)));
  connect(m_ui.read,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotRead(void)));
  connect(m_ui.rewind,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotOperation(void)));
  connect(m_ui.status,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotOperation(void)));
  connect(m_ui.tar_select,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectExecutable(void)));
  connect(m_ui.write,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotStore(void)));
  connect(this,
	  SIGNAL(coloredStatus(const QString &, const QString &)),
	  this,
	  SLOT(slotColoredStatus(const QString &, const QString &)));
  connect(this,
	  SIGNAL(finished(const QString &, const bool)),
	  this,
	  SLOT(slotFinished(const QString &, const bool)));
  connect(this,
	  SIGNAL(status(const QString &,
			const QString &)),
	  this,
	  SLOT(slotStatus(const QString &,
			  const QString &)));
  m_timer.start(2500);

  /*
  ** Prepare compress algorithms.
  */

  QStringList list;

  list << "/bin/gzip"
       << "/usr/local/gzip";

  for(int i = 0; i < list.size(); i++)
    {
      QFileInfo fileInfo(list.at(i));

      if(fileInfo.isExecutable() && fileInfo.isReadable())
	m_ui.compression_algorithms->addItem(list.at(i));
    }

  if(m_ui.compression_algorithms->count() == 0)
    m_ui.compression_algorithms->addItem("n/a");

  m_ui.output->setText
    (QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
  m_ui.splitter_3->setStretchFactor(0, 0);
  m_ui.splitter_3->setStretchFactor(1, 1);
  setWindowIcon(QIcon(":/spinware.png"));
  show();
}

spinware::~spinware()
{
}

void spinware::appendStatus(const QColor &color,
			    const QString &operation,
			    const QString &status)
{
  if(status.trimmed().isEmpty())
    return;

  QTextEdit *widget = 0;

  if(operation == "list")
    widget = m_ui.list;
  else if(operation == "operation")
    widget = m_ui.operation;
  else if(operation == "read")
    widget = m_ui.retrieve;
  else if(operation == "write")
    widget = m_ui.store;

  if(!widget)
    return;

  widget->append(QDateTime::currentDateTime().toString());

  if(color.isValid())
    widget->append
      (QString("<font color='%1'>%2</font>").arg(color.name()).
       arg(status.trimmed()));
  else
    widget->append(status.trimmed());
}

void spinware::closeEvent(QCloseEvent *event)
{
  if(event)
    if(!m_future.isFinished())
      {
	QMessageBox mb(this);

	mb.setIcon(QMessageBox::Question);
	mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
	mb.setText(tr("An operation is active. Are you sure that "
		      "you wish to exit?"));
	mb.setWindowTitle(tr("spinware: Confirmation"));
	mb.setWindowModality(Qt::WindowModal);

	if(mb.exec() != QMessageBox::Yes)
	  {
	    event->ignore();
	    return;
	  }
      }

  QMainWindow::closeEvent(event);
  QApplication::instance()->quit();
}

void spinware::slotAbort(void)
{
  if(m_future.isFinished())
    return;

  QMessageBox mb(this);

  mb.setIcon(QMessageBox::Question);
  mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
  mb.setText(tr("Are you sure that you wish to interrupt "
		"the current operation?"));
  mb.setWindowTitle(tr("spinware: Confirmation"));
  mb.setWindowModality(Qt::WindowModal);

  if(mb.exec() != QMessageBox::Yes)
    return;

  if(m_future.isRunning())
    if(m_pid > 0)
      ::kill(static_cast<pid_t> (m_pid), SIGTERM);

  m_future.cancel();
}

void spinware::slotAbout(void)
{
  QMessageBox::information
    (this, tr("spinware: Information"),
     QString("Qt version %1.\n"
	     "spinware: version 2015.06.12.").arg(QT_VERSION_STR));
}

void spinware::slotColoredStatus(const QString &operation,
				 const QString &status)
{
  appendStatus(QColor(111, 0, 255), operation, status);
}

void spinware::slotExport(void)
{
  QFileDialog dialog(this);

  if(m_ui.export_invoice == sender())
    dialog.selectFile("spinware-list-export.txt");
  else
    dialog.selectFile("spinware-store-export.txt");

  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setDirectory
    (QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
  dialog.setConfirmOverwrite(true);

  if(dialog.exec() == QDialog::Accepted)
    {
      QFile file(dialog.selectedFiles().value(0));

      if(file.open(QIODevice::Text | QIODevice::Truncate |
		   QIODevice::WriteOnly))
	{
	  if(m_ui.export_invoice == sender())
	    file.write(m_ui.list->toPlainText().toUtf8().constData());
	  else
	    file.write(m_ui.store->toPlainText().toUtf8().constData());

	  file.flush();
	}

      file.close();
    }
}

void spinware::slotFinished(const QString &operation, const bool ok)
{
  QTextEdit *widget = 0;

  if(operation == "list")
    widget = m_ui.list;
  else if(operation == "operation")
    widget = m_ui.operation;
  else if(operation == "read")
    widget = m_ui.retrieve;
  else if(operation == "write")
    widget = m_ui.store;

  if(widget)
    {
      if(ok)
	widget->append("<font color='green'>[SUCCESS]</font>");
      else
	widget->append("<font color='red'>[FAILURE]</font>");
    }
}

void spinware::slotFutureFinished(void)
{
  m_pid = 0;
}

void spinware::slotHighlightPaths(void)
{
  QList<QLineEdit *> widgets;
  QStringList list;

  list << m_ui.device->text()
       << m_ui.input->text()
       << m_ui.mt->text()
       << m_ui.output->text()
       << m_ui.tar->text();
  widgets << m_ui.device
	  << m_ui.input
	  << m_ui.mt
	  << m_ui.output
	  << m_ui.tar;

  for(int i = 0; i < list.size(); i++)
    {
      QColor approved(144, 238, 144);
      QColor color;
      QColor declined(240, 128, 128);
      QFileInfo fileInfo(widgets.at(i)->text());
      QPalette palette;

      if(m_ui.device == widgets.at(i))
	{
	  if(fileInfo.isReadable() && fileInfo.isWritable())
	    color = approved;
	  else
	    color = declined;
	}
      else if(m_ui.input == widgets.at(i))
	{
	  if(fileInfo.isDir())
	    {
	      if(fileInfo.isExecutable() && fileInfo.isReadable())
		color = approved;
	      else
		color = declined;
	    }
	  else if(fileInfo.isReadable())
	    color = approved;
	  else
	    color = declined;
	}
      else if(m_ui.output == widgets.at(i))
	{
	  if(fileInfo.isDir() &&
	     fileInfo.isExecutable() &&
	     fileInfo.isWritable())
	    color = approved;
	  else
	    color = declined;
	}
      else if(m_ui.mt == widgets.at(i) ||
	      m_ui.tar == widgets.at(i))
	{
	  if(fileInfo.isExecutable() && fileInfo.isReadable())
	    color = approved;
	  else
	    color = declined;
	}

      palette.setColor(widgets.at(i)->backgroundRole(), color);
      widgets.at(i)->setPalette(palette);
    }
}

void spinware::slotQuit(void)
{
  close();
}

void spinware::slotRead(void)
{
  if(!m_future.isFinished())
    return;

  QFileInfo fileInfo(m_ui.device->text());
  QString device(m_ui.device->text());
  QString error("");
  QString mt(m_ui.mt->text());
  QString output(m_ui.output->text());
  QString tar(m_ui.tar->text());
  int number = m_ui.number->value();

  if(!fileInfo.isReadable())
    {
      error = tr("Device is not readable.");
      goto done_label;
    }

  fileInfo.setFile(mt);

  if(!(fileInfo.isExecutable() && fileInfo.isReadable()))
    {
      error = tr("MT must be a readable executable.");
      goto done_label;
    }

  fileInfo.setFile(output);

  if(!(fileInfo.isExecutable() && fileInfo.isWritable()))
    {
      error = tr("Output must be executable and writable.");
      goto done_label;
    }

  fileInfo.setFile(tar);

  if(!(fileInfo.isExecutable() && fileInfo.isReadable()))
    {
      error = tr("TAR must be a readable executable.");
      goto done_label;
    }

  m_pid = 0;
  m_future = QtConcurrent::run(this,
			       &spinware::read,
			       device,
			       mt,
			       output,
			       tar,
			       number);
  m_futureWatcher.setFuture(m_future);

 done_label:

  if(!error.isEmpty())
    QMessageBox::critical(this, tr("spinware: Error"), error);
}

void spinware::slotSelectDirectory(void)
{
  QPushButton *pushButton = qobject_cast<QPushButton *> (sender());

  if(!(m_ui.input_select == pushButton ||
       m_ui.output_select == pushButton))
    return;

  QFileDialog dialog(this);

  if(m_ui.input_select == pushButton)
    {
      dialog.setDirectory(QDir::homePath());
      dialog.selectFile(m_ui.input->text());
    }
  else
    {
      dialog.selectFile(m_ui.output->text());
      dialog.setConfirmOverwrite(true);
    }

  if(m_ui.input_select == pushButton)
    dialog.setFileMode(QFileDialog::Directory);
  else
    {
      dialog.setFileMode(QFileDialog::Directory);
      dialog.setOption(QFileDialog::ShowDirsOnly, true);
    }

  if(dialog.exec() == QDialog::Accepted)
    {
      if(m_ui.input_select == pushButton)
	m_ui.input->setText(dialog.selectedFiles().value(0));
      else
	m_ui.output->setText(dialog.selectedFiles().value(0));
    }
}

void spinware::slotSelectExecutable(void)
{
  QPushButton *pushButton = qobject_cast<QPushButton *> (sender());

  if(!(m_ui.device_select == pushButton ||
       m_ui.mt_select == pushButton ||
       m_ui.tar_select == pushButton))
    return;

  QFileDialog dialog(this);

  if(m_ui.device_select == pushButton)
    dialog.selectFile(m_ui.device->text());
  else if(m_ui.mt_select == pushButton)
    dialog.selectFile(m_ui.mt->text());
  else
    dialog.selectFile(m_ui.tar->text());

  dialog.setConfirmOverwrite(true);
  dialog.setFileMode(QFileDialog::ExistingFile);

  if(dialog.exec() == QDialog::Accepted)
    {
      if(m_ui.device_select == pushButton)
	m_ui.device->setText(dialog.selectedFiles().value(0));
      else if(m_ui.mt_select == pushButton)
	m_ui.mt->setText(dialog.selectedFiles().value(0));
      else
	m_ui.tar->setText(dialog.selectedFiles().value(0));
    }
}

void spinware::slotStatus(const QString &operation,
			  const QString &status)
{
  appendStatus(QColor(), operation, status);
}

void spinware::slotStore(void)
{
  if(!m_future.isFinished())
    return;

  QFileInfo fileInfo(m_ui.device->text());
  QMessageBox mb(this);
  QString device(m_ui.device->text());
  QString error("");
  QString input(m_ui.input->text());
  QString mt(m_ui.mt->text());
  QString tar(m_ui.tar->text());

  if(!fileInfo.isWritable())
    {
      error = tr("Device is not writable.");
      goto done_label;
    }

  fileInfo.setFile(input);

  if(!fileInfo.isReadable())
    {
      error = tr("Input must be readable.");
      goto done_label;
    }

  fileInfo.setFile(mt);

  if(!(fileInfo.isExecutable() && fileInfo.isReadable()))
    {
      error = tr("MT must be a readable executable.");
      goto done_label;
    }

  fileInfo.setFile(tar);

  if(!(fileInfo.isExecutable() && fileInfo.isReadable()))
    {
      error = tr("TAR must be a readable executable.");
      goto done_label;
    }

  mb.setIcon(QMessageBox::Question);
  mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
  mb.setText(tr("Are you sure that you wish to store "
		"the contents of %1?").arg(input));
  mb.setWindowTitle(tr("spinware: Confirmation"));
  mb.setWindowModality(Qt::WindowModal);

  if(mb.exec() != QMessageBox::Yes)
    return;

  m_pid = 0;
  m_future = QtConcurrent::run(this,
			       &spinware::write,
			       device,
			       input,
			       mt,
			       tar,
			       m_ui.individual->isChecked());
  m_futureWatcher.setFuture(m_future);

 done_label:

  if(!error.isEmpty())
    QMessageBox::critical(this, tr("spinware: Error"), error);
}
