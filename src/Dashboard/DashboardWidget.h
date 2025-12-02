#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>

#include <QApt/Backend>

class QLabel;
class QGridLayout;
class QPushButton;

class DashboardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardWidget(QWidget *parent = nullptr);
    void setBackend(QApt::Backend *backend);

signals:
    void categorySelected(const QString &category);
    void packageSelected(const QString &packageName);
    void searchRequested(const QString &text);
    void showUpdates();

private slots:
    void refreshUpdates();
    void populateCategories();

private:
    void setupUi();
    void updateSystemStatus();
    
    QApt::Backend *m_backend;
    QWidget *m_updatesWidget;
    QLabel *m_updatesLabel;
    QPushButton *m_updateButton;
    
    QWidget *m_categoryGridWidget;
    QGridLayout *m_categoryLayout;
};

#endif // DASHBOARDWIDGET_H
