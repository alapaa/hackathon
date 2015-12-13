#include <QtGui>
#include <QApplication>
#include <QMainWindow>
#include <QBoxLayout>
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
    void handleTokenButton();

private:
    QVBoxLayout* mainLayout;
    QStatusBar* votingStatusBar;
    QLineEdit* personnrLineEdit;
    QPushButton* getTokenButton;
    QAction* exitAction;
    QMenu* fileMenu;
};
