#include <QMessageBox>
#include <QProcess>

#include "spinware.h"

void spinware::list(const QString &device,
		    const QString &mt,
		    const QString &tar)
{
  QProcess process;
  bool ok = true;

  emit status("list", QString("Loading %1...").arg(device));
  process.start(mt, QStringList() << "-f" << device << "load");
  m_pid = process.pid();
  process.waitForFinished(-1);

  if(process.exitCode() != 0)
    {
      ok = false;
      goto done_label;
    }
  else
    emit finished("list", true);

  emit status("list", QString("Rewinding %1...").arg(device));
  process.start(mt, QStringList() << "-f" << device << "rewind");
  m_pid = process.pid();
  process.waitForFinished(-1);

  if(process.exitCode() != 0)
    {
      ok = false;
      goto done_label;
    }
  else
    emit finished("list", true);

  do
    {
      if(m_future.isCanceled())
	break;

      process.start(tar, QStringList() << "-tvzf" << device);
      m_pid = process.pid();
      process.waitForFinished(-1);

      if(process.exitCode() == 0)
	emit status("list", process.readAllStandardOutput());
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

 done_label:
  emit finished("list", ok);
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
	goto done_label;
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

 done_label:

  if(process.exitCode() == 0)
    emit status("operation", process.readAllStandardOutput());
  else
    emit status("operation", process.readAllStandardError());

  emit finished("operation", process.exitCode() == 0);
}

void spinware::read(const QString &device,
		    const QString &output,
		    const QString &tar)
{
  QProcess process;

  emit status("read", QString("Retrieving %1 into %2...").arg(device).
	      arg(output));
  process.start
    (tar, QStringList() << "-C" << output << "-xvzf" << device);
  m_pid = process.pid();
  process.waitForFinished(-1);
  emit finished("read", process.exitCode() == 0);
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
			       tar);
  m_futureWatcher.setFuture(m_future);

 done_label:

  if(!error.isEmpty())
    QMessageBox::critical(this, tr("%1: Error").
			  arg("spinware"), error);
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

  m_pid = 0;
  m_future = QtConcurrent::run(this,
			       &spinware::operation,
			       device,
			       mt,
			       command);
  m_futureWatcher.setFuture(m_future);

 done_label:

  if(!error.isEmpty())
    QMessageBox::critical(this, tr("%1: Error").
			  arg("spinware"), error);
}
