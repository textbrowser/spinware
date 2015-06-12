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

#include <QMessageBox>
#include <QProcess>

#include "spinware.h"

void spinware::list(const QString &device,
		    const QString &mt,
		    const QString &tar,
		    const bool compute_content_size)
{
  QProcess process;
  quint64 content_size = 0;
  quint64 number = 0;

  emit status("list", QString("Loading %1...").arg(device));
  process.start(mt, QStringList() << "-f" << device << "load");
  m_pid = process.pid();
  process.waitForFinished(-1);

  if(process.exitCode() != 0)
    {
      emit status("list", process.readAllStandardError());
      emit finished("list", false);
      return;
    }
  else
    emit finished("list", true);

  emit status("list", QString("Rewinding %1...").arg(device));
  process.start(mt, QStringList() << "-f" << device << "rewind");
  m_pid = process.pid();
  process.waitForFinished(-1);

  if(process.exitCode() != 0)
    {
      emit status("list", process.readAllStandardError());
      emit finished("list", false);
      return;
    }
  else
    emit finished("list", true);

  if(compute_content_size)
    emit status
      ("list", QString("Computing content size of %1...").arg(device));
  else
    emit status("list", QString("Listing %1...").arg(device));

  do
    {
      if(m_future.isCanceled())
	break;

      process.start(tar, QStringList() << "-tvzf" << device);
      m_pid = process.pid();
      process.waitForFinished(-1);

      if(process.exitCode() == 0)
	{
	  if(!compute_content_size)
	    {
	      number += 1;
	      emit coloredStatus
		("list", QString("***** File Number %1 *****").arg(number));
	    }

	  QByteArray bytes(process.readAllStandardOutput());

	  if(!compute_content_size)
	    emit status("list", bytes);

	  QStringList list(QString(bytes.constData()).split('\n'));

	  for(int i = 0; i < list.size(); i++)
	    content_size += static_cast<quint64>
	      (list.at(i).split(' ', QString::SkipEmptyParts).
	       value(2).toLongLong());
	}
      else
	break;

      process.start(mt, QStringList() << "-f" << device << "status");
      m_pid = process.pid();
      process.waitForFinished(-1);

      if(process.exitCode() == 0)
	{
	  if(process.readAllStandardOutput().trimmed().contains("EOD"))
	    break;
	}
    }
  while(true);

  emit finished("list", true);
  emit status
    ("list", QString("Content size... %1 MiB.").
     arg(QString::number(static_cast<double> (content_size) /
			 1048576.0, 'f', 1)));
}

void spinware::operation(const QString &device,
			 const QString &mt,
			 const QString &command)
{
  QProcess process;

  if(command.contains(" "))
    {
      emit status("operation", QString("Executing %1...").arg(command));
      process.start(command);
      m_pid = process.pid();
    }
  else if(command == "bsfm")
    {
      emit status("operation", "Executing status...");
      process.start(mt, QStringList() << "-f" << device << "status");
      m_pid = process.pid();
      process.waitForFinished(-1);

      if(process.exitCode() != 0)
	{
	  emit status("operation", process.readAllStandardOutput());
	  emit finished("operation", false);
	  return;
	}
      else
	emit finished("operation", true);

      QString str(process.readAllStandardOutput().toLower());

      if(str.contains("file number=0"))
	return;
      else if(str.contains("file number=1"))
	{
	  emit status("operation", "Executing rewind...");
	  process.start(mt, QStringList() << "-f" << device << "rewind");
	  m_pid = process.pid();
	}
      else
	{
	  emit status("operation", QString("Executing %1...").arg(command));
	  process.start
	    (mt, QStringList() << "-f" << device << command << "2");
	  m_pid = process.pid();
	}
    }
  else
    {
      emit status("operation", QString("Executing %1...").arg(command));
      process.start(mt, QStringList() << "-f" << device << command);
      m_pid = process.pid();
    }

  process.waitForFinished(-1);

  if(process.exitCode() == 0)
    emit status("operation", process.readAllStandardOutput());
  else
    emit status("operation", process.readAllStandardError());

  emit finished("operation", process.exitCode() == 0);
}

void spinware::read(const QString &device,
		    const QString &mt,
		    const QString &output,
		    const QString &tar,
		    const int number)
{
  QProcess process;

  emit status("read", QString("Loading %1...").arg(device));
  process.start(mt, QStringList() << "-f" << device << "load");
  m_pid = process.pid();
  process.waitForFinished(-1);

  if(process.exitCode() != 0)
    {
      emit status("read", process.readAllStandardError());
      emit finished("read", false);
      return;
    }
  else
    emit finished("read", true);

  if(number == 0)
    {
      emit status("read", QString("Rewinding %1...").arg(device));
      process.start(mt, QStringList() << "-f" << device << "rewind");
      m_pid = process.pid();
      process.waitForFinished(-1);

      if(process.exitCode() == 0)
	emit finished("read", true);
      else
	{
	  emit status("read", process.readAllStandardError());
	  emit finished("read", false);
	  return;
	}
    }
  else if(number > 0)
    {
      emit status
	("read",
	 QString("Positioning %1 on file number %2...").
	 arg(device).arg(number));
      process.start
	(mt, QStringList() << "-f" << device << "asf"
	                   << QString::number(number - 1));
      m_pid = process.pid();
      process.waitForFinished(-1);

      if(process.exitCode() == 0)
	emit finished("read", true);
      else
	{
	  emit status("read", process.readAllStandardError());
	  emit finished("read", false);
	  return;
	}
    }

  do
    {
      emit status("read", QString("Retrieving %1 into %2...").arg(device).
		  arg(output));
      process.start
	(tar, QStringList() << "-C" << output << "-vxzf" << device);
      m_pid = process.pid();
      process.waitForFinished(-1);

      if(process.exitCode() != 0)
	emit status("read", process.readAllStandardError());
      else
	emit status("read", process.readAllStandardOutput());

      emit finished("read", process.exitCode() == 0);

      if(m_future.isCanceled() || number > 0 || process.exitCode() != 0)
	break;

      process.start(mt, QStringList() << "-f" << device << "status");
      m_pid = process.pid();
      process.waitForFinished(-1);

      if(process.exitCode() == 0)
	{
	  if(process.readAllStandardOutput().trimmed().contains("EOD"))
	    break;
	}
    }
  while(true);
}

void spinware::slotList(void)
{
  if(!m_future.isFinished())
    return;

  QFileInfo fileInfo(m_ui.device->text());
  QString device(m_ui.device->text());
  QString error("");
  QString mt(m_ui.mt->text());
  QString tar(m_ui.tar->text());

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

  fileInfo.setFile(tar);

  if(!(fileInfo.isExecutable() && fileInfo.isReadable()))
    {
      error = tr("TAR must be a readable executable.");
      goto done_label;
    }

  m_pid = 0;
  m_future = QtConcurrent::run(this,
			       &spinware::list,
			       device,
			       mt,
			       tar,
			       m_ui.compute == sender());
  m_futureWatcher.setFuture(m_future);

 done_label:

  if(!error.isEmpty())
    QMessageBox::critical(this, tr("spinware: Error"), error);
}

void spinware::slotOperation(void)
{
  if(!m_future.isFinished())
    return;

  QPushButton *pushButton = qobject_cast<QPushButton *> (sender());
  QString command("");

  if(m_ui.backward == pushButton)
    command = "bsfm";
  else if(m_ui.eject == pushButton)
    command = "eject";
  else if(m_ui.end == pushButton)
    command = "eod";
  else if(m_ui.erase == pushButton)
    command = "erase";
  else if(m_ui.forward == pushButton)
    command = "fsf";
  else if(m_ui.load == pushButton)
    command = "load";
  else if(m_ui.rewind == pushButton)
    command = "rewind";
  else if(m_ui.status == pushButton)
    command = "status";
  else
    command = m_ui.operations_custom_command->text().trimmed();

  if(command.isEmpty())
    {
      QMessageBox::critical(this, tr("spinware: Error"),
			    tr("Please specify a command."));
      return;
    }

  QFileInfo fileInfo(m_ui.device->text());
  QString device(m_ui.device->text());
  QString error("");
  QString mt(m_ui.mt->text());

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

  if(command == "erase")
    {
      QStringList list;

      list << tr("Are you sure that you wish to erase %1? "
		 "This action may be irreversible!").arg(device)
	   << tr("Are you absolutely sure that you wish to "
		 "erase %1?").arg(device);

      for(int i = 0; i < list.size(); i++)
	{
	  QMessageBox mb(this);

	  mb.setIcon(QMessageBox::Question);
	  mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
	  mb.setText(list.at(i));
	  mb.setWindowTitle(tr("spinware: Confirmation"));
	  mb.setWindowModality(Qt::WindowModal);

	  if(mb.exec() != QMessageBox::Yes)
	    return;
	}
    }
  else if(!pushButton)
    {
      QMessageBox mb(this);

      mb.setIcon(QMessageBox::Question);
      mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
      mb.setText(tr("Are you sure that you wish to execute "
		    "the specified command?"));
      mb.setWindowTitle(tr("spinware: Confirmation"));
      mb.setWindowModality(Qt::WindowModal);

      if(mb.exec() != QMessageBox::Yes)
	return;
    }

  m_pid = 0;
  m_future = QtConcurrent::run(this,
			       &spinware::operation,
			       device,
			       mt,
			       command);
  m_futureWatcher.setFuture(m_future);

 done_label:

  if(!error.isEmpty())
    QMessageBox::critical(this, tr("spinware: Error"), error);
}

void spinware::write(const QString &device,
		     const QString &input,
		     const QString &mt,
		     const QString &tar,
		     const bool individual)
{
  QProcess process;

  emit status("write", QString("Loading %1...").arg(device));
  process.start(mt, QStringList() << "-f" << device << "load");
  m_pid = process.pid();
  process.waitForFinished(-1);

  if(process.exitCode() != 0)
    {
      emit status("write", process.readAllStandardError());
      emit finished("write", false);
      return;
    }
  else
    emit finished("write", true);

  emit status("write", "Executing eod...");
  process.start(mt, QStringList() << "-f" << device << "eod");
  m_pid = process.pid();
  process.waitForFinished(-1);

  if(process.exitCode() != 0)
    {
      emit status("write", process.readAllStandardError());
      emit finished("write", false);
      return;
    }
  else
    emit finished("write", true);

  if(QFileInfo(input).isDir() && individual)
    {
      QDir dir(input);
      QStringList list(dir.entryList(QDir::AllDirs |
				     QDir::Files |
				     QDir::NoDotAndDotDot));

      emit status
	("write",
	 QString("Setting the working directory to %1...").arg(input));
      process.setWorkingDirectory(input);

      if(input != process.workingDirectory())
	{
	  emit status("write", "Unable to set the working directory.");
	  emit finished("write", false);
	  return;
	}

      while(!list.isEmpty())
	{
	  if(m_future.isCanceled())
	    break;

	  QString str(list.takeFirst());

	  emit status("write", QString("Writing %1 into %2...").arg(str).
		      arg(device));
	  process.start
	    (tar, QStringList() << "-cvzf" << device
	                        << str);
	  m_pid = process.pid();
	  process.waitForFinished(-1);

	  if(process.exitCode() != 0)
	    {
	      emit status("write", process.readAllStandardError());
	      emit finished("write", false);
	      break;
	    }
	  else
	    emit finished("write", true);
	}

      return;
    }
  else
    {
      emit status("write", QString("Writing %1 into %2...").arg(input).
		  arg(device));
      process.start
	(tar, QStringList() << "-C" << QFileInfo(input).path()
	                    << "-cvzf" << device
	                    << QFileInfo(input).fileName());
      m_pid = process.pid();
      process.waitForFinished(-1);

      if(process.exitCode() == 0)
	emit status("write", process.readAllStandardOutput());

      emit finished("write", process.exitCode() == 0);
    }
}
