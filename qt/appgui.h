#include <QtGui>
#include <QApplication>
#include <QMainWindow>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QAction>
#include <QMenuBar>
#include <QStatusBar>

class VoterGui : public QMainWindow
{
    Q_OBJECT

    public:
    VoterGui();

private slots:
    void quit();
    void setText(QString str);

private:
    QGridLayout* mainLayout;
    QStatusBar* votingStatusBar;
    QLineEdit* personnrLineEdit;
    QPushButton* quitButton;
    QAction* exitAction;
    QMenu* fileMenu;
};
