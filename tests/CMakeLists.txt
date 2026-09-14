find_package(Qt5 5.15.0 REQUIRED CONFIG COMPONENTS Test)
include(ECMAddTests)

ecm_add_test(LocalRepositoryTest.cpp
             ../src/LocalRepository.cpp
             ../src/DebFileArgument.cpp
    TEST_NAME LocalRepositoryTest
    LINK_LIBRARIES Qt5::Test Qt5::DBus KF5::I18n)
target_include_directories(LocalRepositoryTest PRIVATE ../src)
target_compile_definitions(LocalRepositoryTest PRIVATE
    -DKYDRA_LIBEXECDIR=\"${KDE_INSTALL_FULL_LIBEXECDIR}/kydra\")

# The window's id, the desktop file and the icon agreeing (the task manager's icon).
ecm_add_test(AppIdentityTest.cpp
    TEST_NAME AppIdentityTest
    LINK_LIBRARIES Qt5::Test Qt5::Gui KF5::CoreAddons)
target_include_directories(AppIdentityTest PRIVATE ../src)
target_compile_definitions(AppIdentityTest PRIVATE
    -DKYDRA_DATADIR=\"${CMAKE_SOURCE_DIR}/data\"
    -DKYDRA_ICONDIR=\"${CMAKE_SOURCE_DIR}/src/icons\")
set_tests_properties(AppIdentityTest PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

# The index helper, against packages the test builds for itself.
add_test(NAME kydra-repo-index
         COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/test_kydra_repo_index.sh
                 ${CMAKE_SOURCE_DIR}/src/kydra-repo-index)
