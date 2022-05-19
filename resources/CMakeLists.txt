set(QML_IMPORT_PATHS "${CMAKE_CURRENT_SOURCE_DIR};${QML_IMPORT_PATHS}" PARENT_SCOPE)

# Use this pseudo target to list all qml, js files in QtCreator
file(GLOB_RECURSE guiFiles LIST_DIRECTORIES false "${CMAKE_CURRENT_SOURCE_DIR}/gui/*.qml" "${CMAKE_CURRENT_SOURCE_DIR}/gui/*.js")
add_custom_target(GUIfiles SOURCES ${guiFiles})
