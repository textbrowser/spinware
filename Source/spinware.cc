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
#include <QMessageBox>
#include <QShortcut>

#include "spinware.h"
#include "spinware_page.h"

spinware::spinware(void):QMainWindow(nullptr)
{
  m_ui.setupUi(this);
  connect(m_ui.action_About,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotAbout(void)));
  connect(m_ui.action_New_Page,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotNewPage(void)));
  connect(m_ui.action_Quit,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotQuit(void)));
  connect(m_ui.tab,
	  SIGNAL(tabCloseRequested(int)),
	  this,
	  SLOT(slotCloseTab(int)));
#ifndef Q_OS_MACOS
  m_ui.tab->setDocumentMode(false);
#else
  m_ui.tab->setDocumentMode(true);
#endif
  m_ui.tab->setStyleSheet
    ("QTabBar::tear {"
     "image: none;"
     "}");
  m_ui.tab->setTabsClosable(true);
  new QShortcut(tr("Ctrl+W"),
		this,
		SLOT(slotCloseTab(void)));

  foreach(auto toolButton, m_ui.tab->findChildren <QToolButton *> ())
    toolButton->setStyleSheet("QToolButton {background-color: white; "
			      "border: 1px solid #c4c4c3; "
			      "margin-bottom: 0px;"
			      "}"
			      "QToolButton::menu-button {border: none;}");

  setWindowIcon(QIcon(":/spinware.png"));
  show();
  slotNewPage();
}

spinware::~spinware()
{
}

void spinware::closeEvent(QCloseEvent *event)
{
  if(event)
    {
      auto prompt = false;

      foreach(auto page, findChildren<spinware_page *> ())
	if(!page->isFinished())
	  {
	    prompt = true;
	    break;
	  }

      if(prompt)
	{
	  QMessageBox mb(this);

	  mb.setIcon(QMessageBox::Question);
	  mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
	  mb.setText(tr("An operation is active. "
			"Are you sure that you wish to exit?"));
	  mb.setWindowTitle(tr("spinware: Confirmation"));
	  mb.setWindowModality(Qt::ApplicationModal);

	  if(mb.exec() != QMessageBox::Yes)
	    {
	      event->ignore();
	      return;
	    }
	}
    }

  QMainWindow::closeEvent(event);
  QApplication::instance()->quit();
}

void spinware::slotAbout(void)
{
  QMessageBox mb(this);
  QPixmap pixmap(":/spinware.png");
  QString str("");

  pixmap = pixmap.scaled
    (QSize(48, 48), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  str = tr("Qt version %1.<br>spinware version %2, Guess Who.").
    arg(QT_VERSION_STR).arg(SPINWARE_VERSION_STR);
  mb.setIconPixmap(pixmap);
  mb.setStandardButtons(QMessageBox::Ok);
  mb.setText(str);
  mb.setTextFormat(Qt::RichText);
  mb.setWindowTitle(tr("spinware: About"));
  mb.exec();
}

void spinware::slotCloseTab(int index)
{
  auto page = qobject_cast<spinware_page *> (m_ui.tab->widget(index));

  if(page)
    {
      if(!page->isFinished())
	{
	  m_ui.tab->setCurrentWidget(page);

	  QMessageBox mb(this);

	  mb.setIcon(QMessageBox::Question);
	  mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
	  mb.setText(tr("An operation is active. Closing the page "
			"may not cancel the current operation. "
			"Are you sure that you wish to continue?"));
	  mb.setWindowTitle(tr("spinware: Confirmation"));
	  mb.setWindowModality(Qt::ApplicationModal);

	  if(mb.exec() != QMessageBox::Yes)
	    return;
	}

      page->deleteLater();
    }
}

void spinware::slotCloseTab(void)
{
  slotCloseTab(m_ui.tab->currentIndex());
}

void spinware::slotNewPage(void)
{
  auto page = new (std::nothrow) spinware_page(this);

  if(page)
    m_ui.tab->addTab(page, QIcon(":/spinware.png"), tr("Page"));
  else
    QMessageBox::critical(this,
			  tr("spinware: Error"),
			  tr("Memory failure."));
}

void spinware::slotQuit(void)
{
  close();
}
