#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>

class DashboardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardWidget(QWidget *parent = nullptr);

signals:
    void categorySelected(const QString &category);
    void packageSelected(const QString &packageName);

private:
    void setupUi();
};

#endif // DASHBOARDWIDGET_H
