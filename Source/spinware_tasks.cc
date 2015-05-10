#include <QMessageBox>
#include <QProcess>
#if QT_VERSION >= 0x050000
#include <QtConcurrent>
#endif
#include <QtCore>

#include "spinware.h"

void spinware::list(const QString &device,
		    const QString &mt,
		    const QString &tar)
{
  bool ok = true;

  emit status("list", QString("Loading %1...").arg(device));

  {
    QProcess process;

    process.start(mt, QStringList() << "-f" << device << "load");
    process.waitForFinished(-1);

    if(process.exitCode() != 0)
      {
	ok = false;
	goto done_label;
      }
  }

  emit status("list", QString("Rewinding %1...").arg(device));

  {
    QProcess process;

    process.start(mt, QStringList() << "-f" << device << "rewind");
    process.waitForFinished(-1);

    if(process.exitCode() != 0)
      {
	ok = false;
	goto done_label;
      }
  }

  do
    {
      if(m_future.isCanceled())
	break;

      {
	QProcess process;

	process.start(tar, QStringList() << "-tvzf" << device);
	process.waitForFinished(-1);

	if(process.exitCode() == 0)
	  emit status("list", process.readAllStandardOutput());
	else
	  break;
      }

      {
	QProcess process;

	process.start(mt, QStringList() << "-f" << device << "status");
	process.waitForFinished(-1);

	if(process.exitCode() == 0)
	  {
	    if(process.readAllStandardOutput().trimmed().contains("EOD"))
	      break;
	  }
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

  emit status("operation", QString("Executing %1...").arg(command));

  if(command.contains(" "))
    process.start(command);
  else
    process.start(mt, QStringList() << "-f" << device << command);

  process.waitForFinished(-1);

  if(process.exitCode() == 0)
    emit status("operation", process.readAllStandardOutput());
  else
    emit status("operation", process.readAllStandardError());

  emit finished("operation", process.exitCode() == 0);
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
    error = tr("Device is not readable.");

  if(!error.isEmpty())
    goto done_label;

  fileInfo.setFile(mt);

  if(!(fileInfo.isExecutable() && fileInfo.isReadable()))
    error = tr("MT must be a readable executable.");

  if(!error.isEmpty())
    goto done_label;

  fileInfo.setFile(tar);

  if(!(fileInfo.isExecutable() && fileInfo.isReadable()))
    error = tr("TAR must be a readable executable.");

  if(!error.isEmpty())
    goto done_label;

  m_future = QtConcurrent::run(this,
			       &spinware::list,
			       device,
			       mt,
			       tar);

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
    command = "bsf";
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
    error = tr("Device is not readable.");

  if(!error.isEmpty())
    goto done_label;

  fileInfo.setFile(mt);

  if(!(fileInfo.isExecutable() && fileInfo.isReadable()))
    error = tr("MT must be a readable executable.");

  if(!error.isEmpty())
    goto done_label;

  m_future = QtConcurrent::run(this,
			       &spinware::operation,
			       device,
			       mt,
			       command);

 done_label:

  if(!error.isEmpty())
    QMessageBox::critical(this, tr("%1: Error").
			  arg("spinware"), error);
}
