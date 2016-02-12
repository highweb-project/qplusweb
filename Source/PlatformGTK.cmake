add_subdirectory(${WEBCORE_DIR}/platform/gtk/po)

# This allows exposing a 'gir' target which builds all GObject introspection files.
add_custom_target(gir ALL DEPENDS ${GObjectIntrospectionTargets})

set(DocumentationDependencies
    "${CMAKE_SOURCE_DIR}/Source/WebKit/gtk/docs/webkitenvironment.xml"
)

if (ENABLE_WEBKIT)
    list(APPEND DocumentationDependencies
        WebKit
        "${CMAKE_SOURCE_DIR}/Source/WebKit/gtk/docs/webkitgtk-docs.sgml"
        "${CMAKE_SOURCE_DIR}/Source/WebKit/gtk/docs/webkitgtk-sections.txt"
    )
    install(DIRECTORY ${CMAKE_BINARY_DIR}/Documentation/webkitgtk/html/
            DESTINATION ${CMAKE_INSTALL_DATADIR}/gtk-doc/html/webkitgtk
    )
endif ()

if (ENABLE_WEBKIT2)
    list(APPEND DocumentationDependencies
        WebKit2
        "${CMAKE_SOURCE_DIR}/Source/WebKit2/UIProcess/API/gtk/docs/webkit2gtk-docs.sgml"
        "${CMAKE_SOURCE_DIR}/Source/WebKit2/UIProcess/API/gtk/docs/webkit2gtk-sections.txt"
    )
    install(DIRECTORY ${CMAKE_BINARY_DIR}/Documentation/webkit2gtk/html/
            DESTINATION ${CMAKE_INSTALL_DATADIR}/gtk-doc/html/webkit2gtk
    )
endif ()

add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/docs-build.stamp"
    DEPENDS ${DocumentationDependencies}
    COMMAND CC="${CMAKE_C_COMPILER}" ${CMAKE_SOURCE_DIR}/Tools/gtk/generate-gtkdoc
    COMMAND touch docs-build.stamp
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_custom_target(fake-docs-target ALL
    DEPENDS "${CMAKE_BINARY_DIR}/docs-build.stamp"
)

if (ENABLE_WEBKIT)
    add_dependencies(fake-docs-target WebKit)
endif ()

if (ENABLE_WEBKIT2)
    add_dependencies(fake-docs-target WebKit2)
endif ()
