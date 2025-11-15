# Install application icons manually since ECM icon installation has naming issues
install(FILES
    org.kydra.app.svg
    64-kydra.svg
    128-kydra.svg
    256-kydra.svg
    kydra-symbolic.svg
    kydra-symbolic-light.svg
    DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps
)
