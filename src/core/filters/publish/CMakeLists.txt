PROJECT("Publishing Filter")

INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_BINARY_DIR}")

SET(
        sources
        ImageView.cpp ImageView.h
        Filter.cpp Filter.h
        Task.cpp Task.h
        CacheDrivenTask.cpp CacheDrivenTask.h
        OptionsWidget.cpp OptionsWidget.h
	Params.cpp Params.h
	ImageInfo.cpp ImageInfo.h
        Settings.cpp Settings.h
	DjbzDispatcher.cpp DjbzDispatcher.h
	OutputParams.cpp OutputParams.h
	DjVuPageGenerator.cpp DjVuPageGenerator.h
)

SOURCE_GROUP("Sources" FILES ${sources})
FILE(GLOB ui_files "ui/*.ui")
SOURCE_GROUP("UI Files" FILES ${ui_files})

QT5_WRAP_UI(ui_sources ${ui_files})

ADD_LIBRARY(publish STATIC ${sources} ${ui_sources})
QT5_USE_MODULES(publish Widgets Xml)
TRANSLATION_SOURCES(scantailor-universal ${sources} ${ui_files})
