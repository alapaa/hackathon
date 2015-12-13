#include <iostream>
#include <sstream>
#include <iomanip>

#include "appgui.h"

using std::cout;
using std::string;
using std::to_string;
using std::ostringstream;

string toHex(const string& s, bool upper_case /* = true */)
{
    ostringstream ret;

    for (string::size_type i = 0; i < s.length(); ++i)
        ret << std::hex << std::setfill('0') << std::setw(2) << (upper_case ?
                                                                 std::uppercase
                                                                 :
                                                                 std::nouppercase)
            << (int)s[i];

    return ret.str();
}


VoterGui::VoterGui()
{

    mainLayout = new QVBoxLayout();
    QWidget *widget = new QWidget();
    widget->setLayout(mainLayout);
    setCentralWidget(widget);
    personnrLineEdit = new QLineEdit();
    getTokenButton = new QPushButton("Get Token");
    getBitcoinAddressButton = new QPushButton("Get Bitcoin address");
    voteButton = new QPushButton("Send vote to blockchain!");
    riksdagComboBox = new QComboBox();
    riksdagComboBox->addItem("Vansterpartiet");
    riksdagComboBox->addItem("Socialdemokraterna");
    riksdagComboBox->addItem("Miljopartiet");
    riksdagComboBox->addItem("Liberalerna");
    riksdagComboBox->addItem("Centern");
    riksdagComboBox->addItem("Moderaterna");
    riksdagComboBox->addItem("Kristdemokraterna");
    riksdagComboBox->addItem("Sverigedemokraterna");

    QObject::connect(riksdagComboBox, SIGNAL(activated(int)),
                     this, SLOT(onActivated(int)));
    mainLayout->addWidget(personnrLineEdit);
    mainLayout->addWidget(getTokenButton);
    mainLayout->addWidget(riksdagComboBox);
    mainLayout->addWidget(getBitcoinAddressButton);
    mainLayout->addWidget(voteButton);

    exitAction = new QAction(tr("E&xit"), this);

    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(personnrLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(setText(QString)));
    connect(getTokenButton, SIGNAL (released()), this, SLOT(handleTokenButton()));
    connect(getBitcoinAddressButton, SIGNAL (released()), this, SLOT(handleAddressButton()));
    connect(voteButton, SIGNAL (released()), this, SLOT(handleVoteButton()));
    connect(riksdagComboBox, SIGNAL(activated(int)), this, SLOT(onActivated(int)));

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
    token = get_token(personnrLineEdit->text().toUtf8().constData());
    votingStatusBar->showMessage(QString::fromUtf8(token.c_str()), 2000 );
}

void VoterGui::handleAddressButton()
{
    votingStatusBar->showMessage("Getting bitcoin address...", 2000 );

    string getAddressCmd("/usr/local/src/bitcoin-0.9.3/src/bitcoin-cli -regtest -rpcconnect=193.15.170.34 -rpcport=18902 -rpcuser=hackathon7 -rpcpassword=rhyme-duct getnewaddress");
    bitcoinAddress = exec(getAddressCmd.c_str());
    votingStatusBar->showMessage(QString::fromUtf8(bitcoinAddress.c_str()), 2000 );
}

void VoterGui::handleVoteButton()
{
    votingStatusBar->showMessage("Sending vote to blockchain...", 2000 );
    string publish("python /home/alapaa/src/python-OP_RETURN/send-OP_RETURN.py ");
    publish += bitcoinAddress.substr(0, bitcoinAddress.length()-1);
    publish += " 0.001 \"";
    ssize_t pos = token.find("KEY-----");
    //string pubkey_in_hex = toHex(token.substr(pos+8, 15), true);
    //publish += pubkey_in_hex;
    publish += token.substr(pos+8, 30);
    publish += ' ';
    publish += to_string(riksdagComboBox->currentIndex());
    publish += "\" ";
    publish += "1";

    cout << publish << '\n';
    string result = exec(publish.c_str());
    cout << result << '\n';
    votingStatusBar->showMessage(QString::fromUtf8(publish.c_str()), 10000);
}

void VoterGui::quit()
{
    //cout << "Got text" << personnrLineEdit->text() << '\n';
    //

    QApplication::quit();
}

void VoterGui::onActivated(int index)
{
    int currentIndex = riksdagComboBox->currentIndex();
    qDebug("activated(%d), currentIndex() = %d", index, currentIndex);
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    VoterGui voterGui;

    voterGui.show();
    return app.exec();
}
