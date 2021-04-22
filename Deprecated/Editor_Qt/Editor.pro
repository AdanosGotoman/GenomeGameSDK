#-------------------------------------------------
#
# Project created by QtCreator 2016-07-07T21:21:30
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = "Directus3D"
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
        editor.cpp \
    DirectusPlayButton.cpp \
    AboutDialog.cpp \
    DirectusDirExplorer.cpp \
    DirectusFileExplorer.cpp \
    DirectusConsole.cpp \
    DirectusInspector.cpp \
    DirectusHierarchy.cpp \
    DirectusTransform.cpp \
    DirectusAdjustLabel.cpp \
    DirectusCamera.cpp \
    DirectusMeshRenderer.cpp \
    DirectusMaterial.cpp \
    DirectusLight.cpp \
    DirectusRigidBody.cpp \
    DirectusCollider.cpp \
    DirectusScript.cpp \
    DirectusAssetLoader.cpp \
    DirectusComboLabelText.cpp \
    DirectusComboSliderText.cpp \
    DirectusMeshFilter.cpp \
    DirectusColorPicker.cpp \
    DirectusDropDownButton.cpp \
    DirectusProgressBar.cpp \
    DirectusFileDialog.cpp \
    DirectusIconProvider.cpp \
    DirectusMaterialDropTarget.cpp \
    DirectusMaterialTextureDropTarget.cpp \
    DirectusAudioSource.cpp \
    DirectusAudioListener.cpp \
    DirectusAudioClipDropTarget.cpp \
    DirectusViewport.cpp \
    DirectusRenderFlags.cpp

HEADERS += editor.h \
    DirectusPlayButton.h \
    AboutDialog.h \
    DirectusDirExplorer.h \
    DirectusFileExplorer.h \
    DirectusConsole.h \
    DirectusInspector.h \
    DirectusHierarchy.h \
    DirectusTransform.h \
    DirectusAdjustLabel.h \
    DirectusCamera.h \
    DirectusMeshRenderer.h \
    DirectusMaterial.h \
    DirectusLight.h \
    DirectusRigidBody.h \
    DirectusCollider.h \
    DirectusScript.h \
    DirectusQVariantPacker.h \
    DirectusAssetLoader.h \
    DirectusComboLabelText.h \
    DirectusComboSliderText.h \
    DirectusMeshFilter.h \
    DirectusColorPicker.h \
    DirectusDropDownButton.h \
    DirectusProgressBar.h \
    DirectusFileDialog.h \
    DirectusIconProvider.h \
    DirectusMaterialDropTarget.h \
    DirectusMaterialTextureDropTarget.h \
    DirectusAudioSource.h \
    DirectusAudioListener.h \
    DirectusIComponent.h \
    DirectusAudioClipDropTarget.h \
    DirectusViewport.h \
    DirectusRenderFlags.h \
    DirectusUtilities.h

FORMS += editor.ui \
    AboutDialog.ui \
    AssetLoadingDialog.ui

# DESTINATION
release:DESTDIR = $$PWD/../Binaries/Release
debug:DESTDIR = $$PWD/../Binaries/Release

# ENGINE INCLUDE/LIBRARY
LIBS += "$$DESTDIR/Runtime.lib"
INCLUDEPATH += $$PWD/../Runtime/

# QT METADATA
RCC_DIR = "$$DESTDIR/../Qt_MetaData/Qt_RCCFiles"
UI_DIR = "$$DESTDIR/../Qt_MetaData/Qt_UICFiles"
MOC_DIR = "$$DESTDIR/../Qt_MetaData/Qt_MOCFiles"
OBJECTS_DIR = "$$DESTDIR/../Qt_MetaData/Qt_ObjFiles"

# RESOURCES
RESOURCES += \
    Images/images.qrc

# Set .exe icon
RC_ICONS += Images/icon.ico
