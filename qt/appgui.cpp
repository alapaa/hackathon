#include <iostream>
#include "appgui.h"

using std::cout;

VoterGui::VoterGui()
{

    mainLayout = new QVBoxLayout();
    QWidget *widget = new QWidget();
    widget->setLayout(mainLayout);
    setCentralWidget(widget);
    personnrLineEdit = new QLineEdit();
    getTokenButton = new QPushButton("Get Token");
    mainLayout->addWidget(personnrLineEdit);
    mainLayout->addWidget(getTokenButton);
    //personnrLineEdit->setFocus();
    //fgpersonnrLineEdit->setLayout(mainLayout);


    //openAction = new QAction(tr("&Open"), this);
    //saveAction = new QAction(tr("&Save"), this);
    exitAction = new QAction(tr("E&xit"), this);

    //connect(openAction, SIGNAL(triggered()), this,
    //        SLOT(open()));
    //connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(personnrLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(setText(QString)));
    connect(getTokenButton, SIGNAL (released()), this, SLOT(handleTokenButton()));
    fileMenu = menuBar()->addMenu(tr("&File"));

    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);


    votingStatusBar = this->statusBar();


    setWindowTitle(tr("Val 2018"));
}

void VoterGui::setText(QString str)
{
    votingStatusBar->showMessage(str, 2000 );
}

void VoterGui::handleTokenButton()
{
    votingStatusBar->showMessage("Getting token...", 2000 );
}

void VoterGui::quit()
{
    //cout << "Got text" << personnrLineEdit->text() << '\n';
    //

    QApplication::quit();
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    VoterGui voterGui;

    voterGui.show();
    return app.exec();
}
