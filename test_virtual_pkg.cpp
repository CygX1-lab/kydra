#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include "src/PackageModel/LocalPackageManager.h"
#include "src/PackageModel/VirtualPackage.h"
#include "src/PackageModel/PackageModel.h"

// Mock for QApt::Backend since we can't easily link against the full app
// We just need enough to make it compile/link for this test

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "Starting Virtual Package Verification Test...";
    
    // 1. Test VirtualPackage class
    qDebug() << "Testing VirtualPackage class...";
    LocalPackageInfo info;
    info.name = "test-package";
    info.version = "1.0.0";
    info.arch = "amd64";
    info.desc = "A test package";
    info.path = "/tmp/test-package.deb";
    
    VirtualPackage vPkg(info);
    
    if (vPkg.name() != "test-package") {
        qCritical() << "FAIL: Name mismatch. Expected 'test-package', got" << vPkg.name();
        return 1;
    }
    
    if (vPkg.version() != "1.0.0") {
        qCritical() << "FAIL: Version mismatch. Expected '1.0.0', got" << vPkg.version();
        return 1;
    }
    
    if (vPkg.isInstalled()) {
        qCritical() << "FAIL: Virtual package should report as not installed";
        return 1;
    }
    
    qDebug() << "VirtualPackage class test PASSED";
    
    // 2. Test PackageModel virtual package handling
    qDebug() << "Testing PackageModel virtual package handling...";
    PackageModel model;
    
    QList<VirtualPackage> vPackages;
    vPackages.append(vPkg);
    
    model.setVirtualPackages(vPackages);
    
    if (model.rowCount() != 1) { // Assuming 0 APT packages + 1 virtual package
        qCritical() << "FAIL: Model row count mismatch. Expected 1, got" << model.rowCount();
        return 1;
    }
    
    QModelIndex idx = model.index(0, 0);
    if (!model.isVirtualPackage(idx)) {
        qCritical() << "FAIL: Index 0 should be identified as virtual package";
        return 1;
    }
    
    VirtualPackage retrievedPkg = model.virtualPackageAt(idx);
    if (retrievedPkg.name() != "test-package") {
        qCritical() << "FAIL: Retrieved package name mismatch";
        return 1;
    }
    
    // Test data() method
    QString displayName = model.data(idx, PackageModel::NameRole).toString();
    if (!displayName.contains("test-package") || !displayName.contains("[Local]")) {
        qCritical() << "FAIL: Display name format incorrect. Got:" << displayName;
        return 1;
    }
    
    qDebug() << "PackageModel test PASSED";
    
    qDebug() << "All tests PASSED";
    return 0;
}
