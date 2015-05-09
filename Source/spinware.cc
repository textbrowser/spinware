#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "spinware.h"

spinware::spinware(void):QMainWindow(0)
{
  m_ui.setupUi(this);
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
  connect(m_ui.output_select,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectDirectory(void)));
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

  show();
}

spinware::~spinware()
{
}

void spinware::slotAbort(void)
{
  m_future.cancel();
}

void spinware::slotAbout(void)
{
  QMessageBox::information
    (this, tr("spinware: Information"),
     "spinware: version 1.00.");
}

void spinware::slotFinished(const QString &widget_name, const bool ok)
{
  if(widget_name == "operation")
    {
      if(ok)
	m_ui.operation->append("<font color='green'>[SUCCESS]</font>");
      else
	m_ui.operation->append("<font color='red'>[FAILURE]</font>");
    }
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
  QApplication::instance()->quit();
}

void spinware::slotSelectDirectory(void)
{
  QPushButton *pushButton = qobject_cast<QPushButton *> (sender());

  if(!(m_ui.input_select == pushButton ||
       m_ui.output_select == pushButton))
    return;

  QFileDialog dialog(this);

  dialog.selectFile(m_ui.output->text());
  dialog.setConfirmOverwrite(true);
  dialog.setDirectory(QDir::homePath());

  if(m_ui.input_select == pushButton)
    dialog.setFileMode(QFileDialog::ExistingFile);
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

  dialog.selectFile(m_ui.output->text());
  dialog.setConfirmOverwrite(true);
  dialog.setDirectory(QDir::homePath());
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

void spinware::slotStatus(const QString &widget_name,
			  const QString &status)
{
  if(status.trimmed().isEmpty())
    return;

  if(widget_name == "list")
    m_ui.list->append(status.trimmed());
  else if(widget_name == "operation")
    m_ui.operation->append(status.trimmed());
}
