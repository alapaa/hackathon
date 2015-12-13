#include <iostream>
#include "appgui.h"

using std::cout;

VoterGui::VoterGui()
{

    mainLayout = new QGridLayout();
    personnrLineEdit = new QLineEdit();
    personnrLineEdit->setFocus();
    personnrLineEdit->setLayout(mainLayout);
    setCentralWidget(personnrLineEdit);

    //openAction = new QAction(tr("&Open"), this);
    //saveAction = new QAction(tr("&Save"), this);
    exitAction = new QAction(tr("E&xit"), this);

    //connect(openAction, SIGNAL(triggered()), this,
    //        SLOT(open()));
    //connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(personnrLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(setText(QString)));

    fileMenu = menuBar()->addMenu(tr("&File"));

    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);


    votingStatusBar = this->statusBar();


    setWindowTitle(tr("VoterGui"));
}

void VoterGui::setText(QString str)
{
    votingStatusBar->showMessage(str, 2000 );
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
