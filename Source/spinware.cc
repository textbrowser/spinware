#include <QFileDialog>
#include <QFileInfo>

#include "spinware.h"

spinware::spinware(void):QMainWindow(0)
{
  m_ui.setupUi(this);
  connect(&m_timer,
	  SIGNAL(timeout(void)),
	  this,
	  SLOT(slotHighlightPaths(void)));
  connect(m_ui.action_Quit,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotQuit(void)));
  connect(m_ui.input_select,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectDirectory(void)));
  connect(m_ui.list_abort,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotAbortList(void)));
  connect(m_ui.list_list,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotList(void)));
  connect(m_ui.output_select,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectDirectory(void)));
  connect(this,
	  SIGNAL(finished(const QString &)),
	  this,
	  SLOT(slotFinished(const QString &)));
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

  m_ui.list_abort->setEnabled(false);
  show();
}

spinware::~spinware()
{
}

void spinware::slotAbortList(void)
{
  emit status("list", "Aborting the list process...");
  m_listFuture.cancel();
}

void spinware::slotFinished(const QString &widget_name)
{
  if(widget_name == "list")
    m_ui.list_abort->setEnabled(false);
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
    m_ui.output->setText(dialog.selectedFiles().value(0));
}

void spinware::slotSelectExecutable(void)
{
}

void spinware::slotStatus(const QString &widget_name,
			  const QString &status)
{
  if(widget_name == "list")
    m_ui.list->append(status);
}
