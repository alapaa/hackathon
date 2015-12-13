#include <string>

#include <QtGui>
#include <QApplication>
#include <QMainWindow>
#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QAction>
#include <QMenuBar>
#include <QStatusBar>
#include <QComboBox>

class VoterGui : public QMainWindow
{
    Q_OBJECT

    public:
    VoterGui();

private slots:
    void quit();
    void setText(QString str);
    void handleTokenButton();
    void handleAddressButton();
    void handleVoteButton();
    void onActivated(int index);

private:
    QVBoxLayout* mainLayout;
    QStatusBar* votingStatusBar;
    QLineEdit* personnrLineEdit;
    QPushButton* getTokenButton;
    QPushButton* getBitcoinAddressButton;
    QPushButton* voteButton;
    QAction* exitAction;
    QMenu* fileMenu;
    QComboBox* riksdagComboBox;

    std::string token; // Public RSA key
    std::string bitcoinAddress;
};

std::string get_token(std::string personnr);
std::string exec(const char* cmd);
