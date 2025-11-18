import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.20 as Kirigami
import org.kde.kirigami.platform 2.20 as KirigamiPlatform
import org.kde.kirigamiaddons.labs.components 0.1 as KirigamiAddons

Kirigami.ApplicationWindow {
    id: root
    
    // Inherit system theme
    Kirigami.Theme.inherit: true
    
    // Search highlight properties
    property string searchTerm: ""
    property string selectedCategory: "all"
    
    // Functions for search enhancements
    function updateSearchHighlights(term) {
        searchTerm = term.toLowerCase()
    }
    
    function filterCategory(category) {
        selectedCategory = category
        // Uncheck other chips
        for (var i = 0; i < categoryChips.children.length; i++) {
            var child = categoryChips.children[i]
            if (child && child.checked !== undefined && child.text.toLowerCase() !== category) {
                child.checked = false
            }
        }
        // Apply category filter
        kydraBackend.filterByCategory(category)
    }
    
    function showPackageDetails() {
        // Hide main content and show details panel
        mainStack.visible = false
        detailsPanel.visible = true
        detailsPanel.updatePackageData(kydraBackend.selectedPackage)
        
        // Update context actions for details view
        contextDrawer.actions = [
            Kirigami.Action {
                text: i18n("Back to Packages")
                icon.name: "go-previous"
                onTriggered: hidePackageDetails()
            }
        ]
    }
    
    function hidePackageDetails() {
        // Show main content and hide details panel
        mainStack.visible = true
        detailsPanel.visible = false
        
        // Restore original context actions
        contextDrawer.actions = []
    }
    
    function highlightText(text, searchTerm) {
        if (!searchTerm || searchTerm.length === 0) return text
        
        var regex = new RegExp("(" + searchTerm.replace(/[.*+?^${}()|[\]\\]/g, '\\$&') + ")", "gi")
        return text.replace(regex, "<b>$1</b>")
    }
    
    // Window properties
    title: i18n("Kydra Package Manager")
    width: 1200
    height: 800
    minimumWidth: 800
    minimumHeight: 600
    
    // Global drawer for navigation
    globalDrawer: Kirigami.GlobalDrawer {
        title: i18n("Kydra")
        titleIcon: "package-x-generic"
        
        actions: [
            Kirigami.Action {
                text: i18n("Packages")
                icon.name: "package-x-generic"
                checked: true
                onTriggered: {
                    mainStack.currentIndex = 0
                }
            },
            Kirigami.Action {
                text: i18n("Updates")
                icon.name: "system-software-update"
                onTriggered: {
                    mainStack.currentIndex = 1
                }
            },
            Kirigami.Action {
                text: i18n("Installed")
                icon.name: "applications-utilities"
                onTriggered: {
                    mainStack.currentIndex = 2
                }
            },
            Kirigami.Action {
                text: i18n("Settings")
                icon.name: "configure"
                onTriggered: {
                    mainStack.currentIndex = 3
                }
            }
        ]
    }
    
    // Context drawer for additional actions
    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }
    
    // Main page with stack
    pageStack.initialPage: Kirigami.ScrollablePage {
        title: i18n("Package Manager")
        
        // Main content stack
        ColumnLayout {
            anchors.fill: parent
            
            // Enhanced floating search bar with visual improvements
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: searchContainer.height + Kirigami.Units.largeSpacing * 2
                
                // Background with shadow for floating effect
                Rectangle {
                    id: searchBackground
                    anchors {
                        fill: searchContainer
                        margins: -Kirigami.Units.smallSpacing
                    }
                    color: Kirigami.Theme.backgroundColor
                    opacity: 0.95
                    radius: Kirigami.Units.gridUnit
                    
                    // Subtle drop shadow
                    layer.enabled: true
                    layer.effect: DropShadow {
                        transparentBorder: true
                        radius: 8
                        samples: 16
                        color: Qt.rgba(0, 0, 0, 0.2)
                        verticalOffset: 2
                    }
                }
                
                // Search container
                ColumnLayout {
                    id: searchContainer
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                        margins: Kirigami.Units.largeSpacing
                    }
                    spacing: Kirigami.Units.smallSpacing
                    
                    // Enhanced search field with highlighting
                    Kirigami.SearchField {
                        id: searchField
                        Layout.fillWidth: true
                        placeholderText: i18n("Search packages...")
                        
                        // Visual enhancements
                        background: Rectangle {
                            color: Kirigami.Theme.backgroundColor
                            border.color: Kirigami.Theme.textColor
                            border.width: searchField.activeFocus ? 2 : 1
                            radius: Kirigami.Units.gridUnit / 2
                        }
                        
                        onTextChanged: {
                            // Emit signal to search
                            kydraBackend.searchPackages(text)
                            // Update highlight matches
                            updateSearchHighlights(text)
                        }
                    }
                    
                    // Smart chips for category filters
                    RowLayout {
                        id: categoryChips
                        Layout.fillWidth: true
                        spacing: Kirigami.Units.smallSpacing
                        visible: searchField.text.length > 0
                        
                        Kirigami.Chip {
                            text: i18n("All")
                            checked: true
                            onCheckedChanged: if (checked) filterCategory("all")
                        }
                        
                        Kirigami.Chip {
                            text: i18n("Applications")
                            icon.name: "applications-other"
                            onCheckedChanged: if (checked) filterCategory("applications")
                        }
                        
                        Kirigami.Chip {
                            text: i18n("Development")
                            icon.name: "applications-development"
                            onCheckedChanged: if (checked) filterCategory("development")
                        }
                        
                        Kirigami.Chip {
                            text: i18n("Graphics")
                            icon.name: "applications-graphics"
                            onCheckedChanged: if (checked) filterCategory("graphics")
                        }
                        
                        Kirigami.Chip {
                            text: i18n("Multimedia")
                            icon.name: "applications-multimedia"
                            onCheckedChanged: if (checked) filterCategory("multimedia")
                        }
                        
                        Kirigami.Chip {
                            text: i18n("System")
                            icon.name: "applications-system"
                            onCheckedChanged: if (checked) filterCategory("system")
                        }
                        
                        Item { Layout.fillWidth: true }
                    }
                    
                    // Search results summary
                    Kirigami.Label {
                        id: searchSummary
                        visible: searchField.text.length > 0
                        text: i18n("Found %1 packages", packageList.model.count)
                        font.pixelSize: Kirigami.Theme.smallFont
                        opacity: 0.7
                    }
                }
            }
            
            // Main stack for different views
            StackView {
                id: mainStack
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                initialItem: packagesView
            }
            
            // Package details panel (initially hidden)
            PackageDetailsPanel {
                id: detailsPanel
                visible: false
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
    
    // Packages view component
    Component {
        id: packagesView
        
        Kirigami.ScrollablePage {
            title: i18n("Available Packages")
            
            // Package list with cards
            Kirigami.CardsListView {
                id: packageList
                model: kydraBackend.packageModel
                
                delegate: Kirigami.Card {
                    id: packageCard
                    banner: Kirigami.Icon {
                        source: model.icon
                        width: 48
                        height: 48
                    }
                    
                    contentItem: ColumnLayout {
                        spacing: Kirigami.Units.smallSpacing
                        
                        Kirigami.Heading {
                            level: 3
                            text: {
                                if (searchTerm && searchTerm.length > 0) {
                                    return highlightText(model.name, searchTerm)
                                }
                                return model.name
                            }
                            elide: Text.ElideRight
                            textFormat: Text.RichText
                        }
                        
                        Kirigami.Label {
                            text: {
                                if (searchTerm && searchTerm.length > 0) {
                                    return highlightText(model.description, searchTerm)
                                }
                                return model.description
                            }
                            wrapMode: Text.WordWrap
                            maximumLineCount: 2
                            elide: Text.ElideRight
                            textFormat: Text.RichText
                        }
                        
                        RowLayout {
                            Kirigami.Label {
                                text: model.version
                                font.pixelSize: Kirigami.Theme.smallFont
                                opacity: 0.7
                            }
                            
                            Item { Layout.fillWidth: true }
                            
                            Kirigami.ActionButton {
                                icon.name: model.isInstalled ? "system-software-update" : "list-add"
                                text: model.isInstalled ? i18n("Update") : i18n("Install")
                                onClicked: {
                                    kydraBackend.togglePackage(model.packageId)
                                }
                            }
                        }
                    }
                    
                    // Make the card clickable to show details
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            kydraBackend.selectPackage(model.packageId)
                            showPackageDetails()
                        }
                        z: -1 // Behind the action button
                    }
                    
                    // Hover effect
                    Behavior on opacity {
                        NumberAnimation { duration: 150 }
                    }
                    opacity: packageCard.hovered ? 1.0 : 0.9
                }
            }
        }
    }
    
    // Updates view component
    Component {
        id: updatesView
        
        Kirigami.ScrollablePage {
            title: i18n("Available Updates")
            
            Kirigami.CardsListView {
                model: kydraBackend.updatesModel
                
                delegate: Kirigami.Card {
                    banner: Kirigami.Icon {
                        source: model.icon
                        width: 48
                        height: 48
                    }
                    
                    contentItem: ColumnLayout {
                        Kirigami.Heading {
                            level: 3
                            text: model.name
                        }
                        
                        Kirigami.Label {
                            text: i18n("Version %1 → %2", model.currentVersion, model.newVersion)
                        }
                        
                        RowLayout {
                            Item { Layout.fillWidth: true }
                            
                            Kirigami.ActionButton {
                                icon.name: "system-software-update"
                                text: i18n("Update")
                                onClicked: {
                                    kydraBackend.updatePackage(model.packageId)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Footer with actions
    footer: Kirigami.ActionToolBar {
        actions: [
            Kirigami.Action {
                icon.name: "document-preview-archive"
                text: i18n("Preview Changes")
                onTriggered: kydraBackend.previewChanges()
            },
            Kirigami.Action {
                icon.name: "dialog-ok-apply"
                text: i18n("Apply Changes")
                onTriggered: kydraBackend.applyChanges()
            },
            Kirigami.Action {
                icon.name: "system-software-update"
                text: i18n("Check for Updates")
                onTriggered: kydraBackend.checkForUpdates()
            }
        ]
    }
}