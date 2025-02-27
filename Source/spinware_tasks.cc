/*
** Copyright (c) Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
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

#include "spinware_page.h"

bool spinware_page::list(const QString &device,
			 const QString &mt,
			 const QString &tar,
			 const bool compute_content_size)
{
  QProcess process;
  quint64 content_size = 0;
  quint64 number = 0;

  emit status("list", tr("Loading %1...").arg(device));
  process.start(mt, QStringList() << "-f" << device << "load");
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
  m_pid = process.pid();
#else
  m_pid = process.processId();
#endif
  process.waitForFinished(-1);

  if(process.exitCode() != 0)
    {
      emit status("list", process.readAllStandardError());
      emit finished("list", false);
      return false;
    }
  else
    emit finished("list", true);

  emit status("list", tr("Rewinding %1...").arg(device));
  process.start(mt, QStringList() << "-f" << device << "rewind");
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
  m_pid = process.pid();
#else
  m_pid = process.processId();
#endif
  process.waitForFinished(-1);

  if(process.exitCode() != 0)
    {
      emit status("list", process.readAllStandardError());
      emit finished("list", false);
      return false;
    }
  else
    emit finished("list", true);

  if(compute_content_size)
    emit status
      ("list", tr("Computing content size of %1...").arg(device));
  else
    emit status("list", tr("Listing %1...").arg(device));

  do
    {
      if(m_future.isCanceled())
	break;

      process.start(tar, QStringList() << "-tvzf" << device);
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
      m_pid = process.pid();
#else
      m_pid = process.processId();
#endif
      process.waitForFinished(-1);

      if(process.exitCode() == 0)
	{
	  if(!compute_content_size)
	    {
	      number += 1;
	      emit coloredStatus
		("list", tr("***** File Number %1 *****").arg(number));
	    }

	  auto const bytes(process.readAllStandardOutput());

	  if(!compute_content_size)
	    emit status("list", bytes);

	  auto const list(QString(bytes.constData()).split('\n'));

	  for(int i = 0; i < list.size(); i++)
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
	    content_size += static_cast<quint64>
	      (list.at(i).split(' ', QString::SkipEmptyParts).
	       value(2).toLongLong());
#else
	    content_size += static_cast<quint64>
	      (list.at(i).split(' ', Qt::SkipEmptyParts).
	       value(2).toLongLong());
#endif
	}
      else
	break;

      process.start(mt, QStringList() << "-f" << device << "status");
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
      m_pid = process.pid();
#else
      m_pid = process.processId();
#endif
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
    ("list",
     tr("Content size... %1 MiB.").
     arg(QString::number(static_cast<double> (content_size) /
			 1048576.0, 'f', 1)));
  return true;
}

bool spinware_page::operation(const QString &device,
			      const QString &mt,
			      const QString &command)
{
  QProcess process;

  if(command.contains(" "))
    {
      emit status("operation", tr("Executing %1...").arg(command));
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
      process.start(command);
#else
      auto const list(command.split(" ", Qt::SkipEmptyParts));

      process.start(list.value(0), list.mid(1));
#endif
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
      m_pid = process.pid();
#else
      m_pid = process.processId();
#endif
    }
  else if(command == "bsfm")
    {
      emit status("operation", tr("Executing status..."));
      process.start(mt, QStringList() << "-f" << device << "status");
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
      m_pid = process.pid();
#else
      m_pid = process.processId();
#endif
      process.waitForFinished(-1);

      if(process.exitCode() != 0)
	{
	  emit status("operation", process.readAllStandardOutput());
	  emit finished("operation", false);
	  return false;
	}
      else
	emit finished("operation", true);

      auto const str(process.readAllStandardOutput().toLower());

      if(str.contains("file number=0"))
	return true;
      else if(str.contains("file number=1"))
	{
	  emit status("operation", tr("Executing rewind..."));
	  process.start(mt, QStringList() << "-f" << device << "rewind");
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
	  m_pid = process.pid();
#else
	  m_pid = process.processId();
#endif
	}
      else
	{
	  emit status("operation", tr("Executing %1...").arg(command));
	  process.start
	    (mt, QStringList() << "-f" << device << command << "2");
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
	  m_pid = process.pid();
#else
	  m_pid = process.processId();
#endif
	}
    }
  else
    {
      emit status("operation", tr("Executing %1...").arg(command));
      process.start(mt, QStringList() << "-f" << device << command);
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
      m_pid = process.pid();
#else
      m_pid = process.processId();
#endif
    }

  process.waitForFinished(-1);

  if(process.exitCode() == 0)
    emit status("operation", process.readAllStandardOutput());
  else
    emit status("operation", process.readAllStandardError());

  emit finished("operation", process.exitCode() == 0);
  return process.exitCode() == 0;
}

bool spinware_page::read(const QString &device,
			 const QString &mt,
			 const QString &output,
			 const QString &tar,
			 const int number)
{
  QProcess process;

  emit status("read", tr("Loading %1...").arg(device));
  process.start(mt, QStringList() << "-f" << device << "load");
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
  m_pid = process.pid();
#else
  m_pid = process.processId();
#endif
  process.waitForFinished(-1);

  if(process.exitCode() != 0)
    {
      emit status("read", process.readAllStandardError());
      emit finished("read", false);
      return false;
    }
  else
    emit finished("read", true);

  if(number == 0)
    {
      emit status("read", tr("Rewinding %1...").arg(device));
      process.start(mt, QStringList() << "-f" << device << "rewind");
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
      m_pid = process.pid();
#else
      m_pid = process.processId();
#endif
      process.waitForFinished(-1);

      if(process.exitCode() == 0)
	emit finished("read", true);
      else
	{
	  emit status("read", process.readAllStandardError());
	  emit finished("read", false);
	  return false;
	}
    }
  else if(number > 0)
    {
      emit status
	("read",
	 tr("Positioning %1 on file number %2...").arg(device).arg(number));
      process.start
	(mt, QStringList() << "-f" << device << "asf"
	                   << QString::number(number - 1));
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
      m_pid = process.pid();
#else
      m_pid = process.processId();
#endif
      process.waitForFinished(-1);

      if(process.exitCode() == 0)
	emit finished("read", true);
      else
	{
	  emit status("read", process.readAllStandardError());
	  emit finished("read", false);
	  return false;
	}
    }

  do
    {
      emit status
	("read", tr("Retrieving %1 into %2...").arg(device).arg(output));
      process.start
	(tar, QStringList() << "-C" << output << "-vxzf" << device);
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
      m_pid = process.pid();
#else
      m_pid = process.processId();
#endif
      process.waitForFinished(-1);

      if(process.exitCode() != 0)
	emit status("read", process.readAllStandardError());
      else
	emit status("read", process.readAllStandardOutput());

      emit finished("read", process.exitCode() == 0);

      if(m_future.isCanceled() || number > 0 || process.exitCode() != 0)
	break;

      process.start(mt, QStringList() << "-f" << device << "status");
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
      m_pid = process.pid();
#else
      m_pid = process.processId();
#endif
      process.waitForFinished(-1);

      if(process.exitCode() == 0)
	{
	  if(process.readAllStandardOutput().trimmed().contains("EOD"))
	    break;
	}
    }
  while(true);

  return process.exitCode() == 0;
}

void spinware_page::slotList(void)
{
  QFileInfo fileInfo(m_ui.device->text());
  QString error("");
  auto const device(m_ui.device->text());
  auto const mt(m_ui.mt->text());
  auto const tar(m_ui.tar->text());

  if(!m_future.isFinished())
    {
      error = tr("An operation is in progress.");
      goto done_label;
    }

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
  m_storeOperation = false;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  m_future = QtConcurrent::run(this,
			       &spinware_page::list,
			       device,
			       mt,
			       tar,
			       m_ui.compute == sender());
#else
  m_future = QtConcurrent::run(&spinware_page::list,
			       this,
			       device,
			       mt,
			       tar,
			       m_ui.compute == sender());
#endif
  m_futureWatcher.setFuture(m_future);

 done_label:

  if(!error.isEmpty())
    emit this->error(error);
}

void spinware_page::slotOperation(void)
{
  QString command("");
  auto toolButton = qobject_cast<QToolButton *> (sender());

  if(m_ui.backward == toolButton)
    command = "bsfm";
  else if(m_ui.eject == toolButton)
    command = "eject";
  else if(m_ui.end == toolButton)
    command = "eod";
  else if(m_ui.erase == toolButton)
    command = "erase";
  else if(m_ui.forward == toolButton)
    command = "fsf";
  else if(m_ui.load == toolButton)
    command = "load";
  else if(m_ui.rewind == toolButton)
    command = "rewind";
  else if(m_ui.status == toolButton)
    command = "status";
  else
    command = m_ui.operations_custom_command->text().trimmed();

  if(command.isEmpty())
    {
      if(!m_future.isFinished())
	emit error(tr("An operation is in progress."));
      else
	emit error(tr("Please specify a command."));

      return;
    }

  QFileInfo fileInfo(m_ui.device->text());
  QString error("");
  auto const device(m_ui.device->text());
  auto const mt(m_ui.mt->text());

  if(!m_future.isFinished())
    {
      error = tr("An operation is in progress.");
      goto done_label;
    }

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
	  mb.setWindowModality(Qt::ApplicationModal);

	  if(mb.exec() != QMessageBox::Yes)
	    return;
	}
    }
  else if(!toolButton)
    {
      QMessageBox mb(this);

      mb.setIcon(QMessageBox::Question);
      mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
      mb.setText(tr("Are you sure that you wish to execute "
		    "the specified command?"));
      mb.setWindowTitle(tr("spinware: Confirmation"));
      mb.setWindowModality(Qt::ApplicationModal);

      if(mb.exec() != QMessageBox::Yes)
	return;
    }

  m_pid = 0;
  m_storeOperation = false;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
  m_future = QtConcurrent::run(this,
			       &spinware_page::operation,
			       device,
			       mt,
			       command);
#else
  m_future = QtConcurrent::run(&spinware_page::operation,
			       this,
			       device,
			       mt,
			       command);
#endif
  m_futureWatcher.setFuture(m_future);

 done_label:

  if(!error.isEmpty())
    emit this->error(error);
}

bool spinware_page::write(const QString &device,
			  const QString &input,
			  const QString &mt,
			  const QString &tar,
			  const bool individual)
{
  QProcess process;

  emit status("write", tr("Loading %1...").arg(device));
  process.start(mt, QStringList() << "-f" << device << "load");
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
  m_pid = process.pid();
#else
  m_pid = process.processId();
#endif
  process.waitForFinished(-1);

  if(process.exitCode() != 0)
    {
      emit status("write", process.readAllStandardError());
      emit finished("write", false);
      return false;
    }
  else
    emit finished("write", true);

  emit status("write", tr("Executing eod..."));
  process.start(mt, QStringList() << "-f" << device << "eod");
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
  m_pid = process.pid();
#else
  m_pid = process.processId();
#endif
  process.waitForFinished(-1);

  if(process.exitCode() != 0)
    {
      emit status("write", process.readAllStandardError());
      emit finished("write", false);
      return false;
    }
  else
    emit finished("write", true);

  if(QFileInfo(input).isDir() && individual)
    {
      emit status
	("write", tr("Setting the working directory to %1...").arg(input));
      process.setWorkingDirectory(input);

      if(input != process.workingDirectory())
	{
	  emit status("write", tr("Unable to set the working directory."));
	  emit finished("write", false);
	  return false;
	}

      QDir dir(input);
      auto const list(dir.entryList(QDir::AllDirs |
				    QDir::Files |
				    QDir::NoDotAndDotDot));

      for(int i = 0; i < list.size(); i++)
	{
	  if(m_future.isCanceled())
	    break;

	  auto const str(list.at(i));

	  emit status
	    ("write", tr("Writing %1 into %2...").arg(str).arg(device));
	  process.start(tar, QStringList() << "-cvzf" << device << str);
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
	  m_pid = process.pid();
#else
	  m_pid = process.processId();
#endif
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

      return process.exitCode() == 0;
    }
  else
    {
      emit status("write", tr("Writing %1 into %2...").arg(input).arg(device));
      process.start
	(tar, QStringList() << "-C" << QFileInfo(input).path()
	                    << "-cvzf" << device
	                    << QFileInfo(input).fileName());
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
      m_pid = process.pid();
#else
      m_pid = process.processId();
#endif
      process.waitForFinished(-1);

      if(process.exitCode() == 0)
	emit status("write", process.readAllStandardOutput());

      emit finished("write", process.exitCode() == 0);
      return process.exitCode() == 0;
    }
}
