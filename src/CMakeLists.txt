add_subdirectory(icons)

# QML files for Kirigami UI
set(kydra_QML_SRCS
    qml/KydraKirigami.qml
    qml/PackageDetailsPanel.qml
)

set(kydra_SRCS
    main.cpp
    MainWindow.cpp
    ManagerWidget.cpp
    ReviewWidget.cpp
    DetailsTabs/DetailsTab.cpp
    DetailsTabs/MainTab.cpp
    DetailsTabs/ChangelogTab.cpp
    DetailsTabs/DependsTab.cpp
    DetailsTabs/InstalledFilesTab.cpp
    DetailsTabs/TechnicalDetailsTab.cpp
    DetailsTabs/VersionTab.cpp
    DetailsWidget.cpp
    EnhancedDetailsWidget.cpp
    DownloadModel/DownloadModel.cpp
    DownloadModel/DownloadDelegate.cpp
    FilterWidget/ArchitectureFilter.cpp
    FilterWidget/CategoryFilter.cpp
    FilterWidget/FilterModel.cpp
    FilterWidget/FilterWidget.cpp
    FilterWidget/OriginFilter.cpp
    FilterWidget/StatusFilter.cpp
    PackageModel/PackageModel.cpp
    PackageModel/PackageProxyModel.cpp
    PackageModel/PackageView.cpp
    PackageModel/PackageViewHeader.cpp
    PackageModel/PackageDelegate.cpp
    PackageModel/PackageWidget.cpp
    PackageModel/PackageIconExtractor.cpp
    StatusWidget.cpp
    TransactionWidget.cpp
    config/ManagerSettingsDialog.cpp
    config/GeneralSettingsPage.cpp
    config/RepositorySettingsPage.cpp
    settings/SettingsPageBase.cpp
    DonateDialog.cpp
    KirigamiBackend.cpp
    
    muonapt/ChangesDialog.cpp
    muonapt/MuonStrings.cpp
    muonapt/QAptActions.cpp
    muonapt/HistoryView/HistoryView.h
    muonapt/HistoryView/HistoryProxyModel.h
    muonapt/HistoryView/HistoryView.cpp
    muonapt/HistoryView/HistoryProxyModel.cpp

)

kconfig_add_kcfg_files(kydra_SRCS GENERATE_MOC config/MuonSettings.kcfgc)
install(FILES config/muon.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})

add_executable(kydra ${kydra_SRCS})

target_compile_definitions(kydra PRIVATE -DCMAKE_INSTALL_FULL_LIBEXECDIR_KF5=\"${CMAKE_INSTALL_FULL_LIBEXECDIR_KF5}\")
if(DebconfKDE_FOUND)
    target_compile_definitions(kydra PRIVATE -DHAVE_DEBCONFKDE)
    target_link_libraries(kydra DebconfKDE::Main)
endif()
target_link_libraries(kydra KF5::KIOWidgets
                           KF5::DBusAddons
                           KF5::I18n
                           KF5::IconThemes
                           KF5::XmlGui
                           Qt5::Concurrent
                           QApt::Main)

if(WITH_KIRIGAMI)
    target_link_libraries(kydra KF5::Kirigami2 Qt5::Qml Qt5::Quick)
    target_compile_definitions(kydra PRIVATE -DHAVE_KIRIGAMI)
endif()

install(TARGETS kydra ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS ../data/org.kydra.app.desktop  DESTINATION ${KDE_INSTALL_APPDIR} )
install( FILES ../data/org.kydra.app.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR} )
install( FILES muonui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/kydra RENAME kydraui.rc )

# Install QML files
install(FILES ${kydra_QML_SRCS} DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kydra)
