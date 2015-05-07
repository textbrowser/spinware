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
  connect(m_ui.input_select,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectDirectory(void)));
  connect(m_ui.output_select,
	  SIGNAL(clicked(void)),
	  this,
	  SLOT(slotSelectDirectory(void)));
  m_timer.start(2500);
  show();
}

spinware::~spinware()
{
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
