#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include "src/PackageModel/LocalPackageManager.h"

// Mock QApt::Backend
namespace QApt {
    class Package;
    typedef QList<Package*> PackageList;

    class Backend : public QObject {
        Q_OBJECT
    public:
        Backend(QObject *parent = nullptr) : QObject(parent) {}
        Package* package(const QString&) { return nullptr; }
        PackageList availablePackages() { return PackageList(); }
    };
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    if (argc < 2) {
        qDebug() << "Usage: test_parse_deb <path_to_deb>";
        return 1;
    }
    
    QString debPath = argv[1];
    qDebug() << "Testing parseDebFile with:" << debPath;
    
    LocalPackageManager manager(nullptr, nullptr);
    LocalPackageInfo info;
    
    bool result = manager.parseDebFile(debPath, info);
    
    if (result) {
        qDebug() << "SUCCESS: Parsed package:" << info.packageName << "Version:" << info.version;
        return 0;
    } else {
        qDebug() << "FAILURE: Could not parse .deb file";
        return 1;
    }
}
