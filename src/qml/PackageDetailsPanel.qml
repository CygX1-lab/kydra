import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigami.platform 2.20 as KirigamiPlatform

Kirigami.ScrollablePage {
    id: detailsPanel
    
    property var package: null
    property string packageName: ""
    property string packageIcon: ""
    property string packageDescription: ""
    property string packageLongDescription: ""
    property string packageVersion: ""
    property string packageMaintainer: ""
    property string packageSection: ""
    property string packageOrigin: ""
    property bool packageIsInstalled: false
    property bool packageIsUpgradeable: false
    
    // Tab animation properties
    property int currentTabIndex: 0
    property real tabTransitionProgress: 1.0
    
    // Functions to update package data
    function updatePackageData(pkg) {
        package = pkg
        if (pkg) {
            packageName = pkg.name || ""
            packageIcon = pkg.icon || "package-x-generic"
            packageDescription = pkg.shortDescription || ""
            packageLongDescription = pkg.longDescription || ""
            packageVersion = pkg.version || ""
            packageMaintainer = pkg.maintainer || ""
            packageSection = pkg.section || ""
            packageOrigin = pkg.origin || ""
            packageIsInstalled = pkg.isInstalled || false
            packageIsUpgradeable = pkg.isUpgradeable || false
        }
    }
    
    // Header with gradient background
    header: Kirigami.Header {
        id: detailsHeader
        
        // Gradient background
        background: Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                GradientStop { position: 0.0; color: Kirigami.Theme.backgroundColor }
                GradientStop { position: 1.0; color: Qt.darker(Kirigami.Theme.backgroundColor, 1.1) }
            }
        }
        
        // Package icon and name
        RowLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.largeSpacing
            
            Kirigami.Icon {
                id: packageIconItem
                source: packageIcon
                Layout.preferredWidth: Kirigami.Units.iconSizes.huge
                Layout.preferredHeight: Kirigami.Units.iconSizes.huge
                Layout.alignment: Qt.AlignVCenter
            }
            
            ColumnLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing
                
                Kirigami.Heading {
                    level: 1
                    text: packageName
                    elide: Text.ElideRight
                    textFormat: Text.PlainText
                }
                
                Kirigami.Label {
                    text: packageDescription
                    opacity: 0.8
                    elide: Text.ElideRight
                    maximumLineCount: 2
                    wrapMode: Text.WordWrap
                }
                
                RowLayout {
                    spacing: Kirigami.Units.smallSpacing
                    
                    Kirigami.Label {
                        text: packageVersion
                        font.pixelSize: Kirigami.Theme.smallFont
                        opacity: 0.7
                    }
                    
                    Kirigami.Label {
                        text: packageSection
                        visible: packageSection !== ""
                        font.pixelSize: Kirigami.Theme.smallFont
                        opacity: 0.6
                    }
                    
                    Item { Layout.fillWidth: true }
                    
                    // Action buttons
                    Kirigami.ActionButton {
                        id: installButton
                        visible: !packageIsInstalled
                        icon.name: "list-add"
                        text: i18n("Install")
                        onClicked: detailsPanel.installPackage()
                    }
                    
                    Kirigami.ActionButton {
                        id: updateButton
                        visible: packageIsInstalled && packageIsUpgradeable
                        icon.name: "system-software-update"
                        text: i18n("Update")
                        onClicked: detailsPanel.updatePackage()
                    }
                    
                    Kirigami.ActionButton {
                        id: removeButton
                        visible: packageIsInstalled
                        icon.name: "edit-delete"
                        text: i18n("Remove")
                        onClicked: detailsPanel.removePackage()
                    }
                }
            }
        }
    }
    
    // Tab bar with animated transitions
    Kirigami.TabBar {
        id: tabBar
        currentIndex: detailsPanel.currentTabIndex
        
        onCurrentIndexChanged: {
            detailsPanel.currentTabIndex = currentIndex
            animateTabTransition()
        }
        
        Kirigami.TabButton {
            text: i18n("Overview")
            icon.name: "view-list-details"
        }
        Kirigami.TabButton {
            text: i18n("Files")
            icon.name: "folder"
        }
        Kirigami.TabButton {
            text: i18n("Dependencies")
            icon.name: "network-workgroup"
        }
        Kirigami.TabButton {
            text: i18n("Versions")
            icon.name: "view-choose"
        }
    }
    
    // Tab content with animated transitions
    StackLayout {
        id: tabContent
        anchors {
            left: parent.left
            right: parent.right
            top: tabBar.bottom
            bottom: parent.bottom
        }
        currentIndex: detailsPanel.currentTabIndex
        
        // Overview tab
        Kirigami.ScrollablePage {
            title: i18n("Overview")
            
            ColumnLayout {
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    margins: Kirigami.Units.largeSpacing
                }
                spacing: Kirigami.Units.largeSpacing
                
                // Long description with improved typography
                Kirigami.Card {
                    Layout.fillWidth: true
                    header: Kirigami.Heading {
                        level: 3
                        text: i18n("Description")
                    }
                    
                    contentItem: ColumnLayout {
                        spacing: Kirigami.Units.smallSpacing
                        
                        Kirigami.Label {
                            text: packageLongDescription
                            wrapMode: Text.WordWrap
                            font.pixelSize: Kirigami.Theme.defaultFont
                            lineHeight: 1.4
                        }
                        
                        // Support information
                        Kirigami.Label {
                            visible: package && package.isSupported !== undefined
                            text: package && package.isSupported ? 
                                i18n("Supported until %1", package.supportedUntil) :
                                i18n("Community supported")
                            font.pixelSize: Kirigami.Theme.smallFont
                            opacity: 0.7
                        }
                    }
                }
                
                // Technical details card
                Kirigami.Card {
                    Layout.fillWidth: true
                    header: Kirigami.Heading {
                        level: 3
                        text: i18n("Technical Details")
                    }
                    
                    contentItem: GridLayout {
                        columns: 2
                        columnSpacing: Kirigami.Units.largeSpacing
                        rowSpacing: Kirigami.Units.smallSpacing
                        
                        Kirigami.Label {
                            text: i18n("Maintainer:")
                            font.bold: true
                        }
                        Kirigami.Label {
                            text: packageMaintainer
                            elide: Text.ElideRight
                        }
                        
                        Kirigami.Label {
                            text: i18n("Origin:")
                            font.bold: true
                        }
                        Kirigami.Label {
                            text: packageOrigin
                            elide: Text.ElideRight
                        }
                        
                        Kirigami.Label {
                            text: i18n("Section:")
                            font.bold: true
                        }
                        Kirigami.Label {
                            text: packageSection
                            elide: Text.ElideRight
                        }
                    }
                }
                
                // Screenshots viewer (placeholder for now)
                Kirigami.Card {
                    Layout.fillWidth: true
                    visible: false // Will be implemented with Kirigami Gallery
                    header: Kirigami.Heading {
                        level: 3
                        text: i18n("Screenshots")
                    }
                    
                    contentItem: Kirigami.Label {
                        text: i18n("Screenshots will be available here")
                        opacity: 0.6
                    }
                }
            }
        }
        
        // Files tab
        Kirigami.ScrollablePage {
            title: i18n("Installed Files")
            
            Kirigami.Label {
                anchors.centerIn: parent
                visible: !packageIsInstalled
                text: i18n("Package is not installed")
                opacity: 0.6
            }
            
            Kirigami.CardsListView {
                id: filesList
                visible: packageIsInstalled
                model: package ? package.installedFilesList : []
                
                delegate: Kirigami.Card {
                    contentItem: Kirigami.Label {
                        text: modelData
                        font.family: "monospace"
                        font.pixelSize: Kirigami.Theme.smallFont
                    }
                }
            }
        }
        
        // Dependencies tab
        Kirigami.ScrollablePage {
            title: i18n("Dependencies")
            
            ColumnLayout {
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    margins: Kirigami.Units.largeSpacing
                }
                spacing: Kirigami.Units.largeSpacing
                
                Kirigami.Card {
                    Layout.fillWidth: true
                    header: Kirigami.Heading {
                        level: 3
                        text: i18n("Dependencies")
                    }
                    
                    contentItem: Kirigami.Label {
                        text: package ? package.dependencyList : i18n("No dependencies")
                        wrapMode: Text.WordWrap
                        font.pixelSize: Kirigami.Theme.smallFont
                        font.family: "monospace"
                    }
                }
                
                Kirigami.Card {
                    Layout.fillWidth: true
                    header: Kirigami.Heading {
                        level: 3
                        text: i18n("Reverse Dependencies")
                    }
                    
                    contentItem: Kirigami.Label {
                        text: package ? package.requiredByList : i18n("No reverse dependencies")
                        wrapMode: Text.WordWrap
                        font.pixelSize: Kirigami.Theme.smallFont
                        font.family: "monospace"
                    }
                }
            }
        }
        
        // Versions tab
        Kirigami.ScrollablePage {
            title: i18n("Available Versions")
            
            Kirigami.CardsListView {
                model: package ? package.availableVersions : []
                
                delegate: Kirigami.Card {
                    contentItem: RowLayout {
                        Kirigami.Label {
                            text: modelData
                            Layout.fillWidth: true
                        }
                        
                        Kirigami.Label {
                            text: model.index === 0 ? i18n("(current)") : ""
                            opacity: 0.6
                            font.italic: true
                        }
                    }
                }
            }
        }
    }
    
    // Tab transition animation
    function animateTabTransition() {
        tabTransitionProgress = 0.0
        
        var animation = Qt.createQmlObject("
            import QtQuick 2.15
            NumberAnimation {
                target: detailsPanel
                property: \"tabTransitionProgress\"
                from: 0.0
                to: 1.0
                duration: 200
                easing.type: Easing.InOutCubic
            }
        ", detailsPanel)
        
        animation.start()
    }
    
    // Package action functions
    function installPackage() {
        if (package) {
            kydraBackend.installPackage(package.packageId)
        }
    }
    
    function updatePackage() {
        if (package) {
            kydraBackend.updatePackage(package.packageId)
        }
    }
    
    function removePackage() {
        if (package) {
            kydraBackend.removePackage(package.packageId)
        }
    }
}