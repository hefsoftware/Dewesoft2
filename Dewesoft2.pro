QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += testlib
CONFIG += c++17
CONFIG += object_parallel_to_source
CONFIG += no_batch # above doesn't properly work with nmake without this

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Datafile/Test/source2dtestfile.cpp \
    Datafile/datasource2d.cpp \
    Datafile/datasource2doptimized.cpp \
    Datafile/entry.cpp \
    Datafile/escape.cpp \
    Datafile/file.cpp \
    Datafile/filesystem.cpp \
    Datafile/opener.cpp \
    Dewesoft/DWLoadLib.c \
    Dewesoft/Unused.cpp \
    Dewesoft/dw7data.cpp \
    Dewesoft/dw7datafile.cpp \
    Dewesoft/dw7util.cpp \
    Mathlib/basicfunctions.cpp \
    Mathlib/floating.cpp \
    Mathlib/function.cpp \
    Mathlib/range.cpp \
    Mathlib/rangemap.cpp \
    Mathlib/sum.cpp \
    PlotLib/scrollmanager.cpp \
    PlotLib/tickalgorithmbasic.cpp \
    PlotLib/ticklist.cpp \
    Plotlib/Area/Impl/plotarea.cpp \
    Plotlib/Area/Impl/plotarea2d.cpp \
    Plotlib/Area/Impl/plotareagrid.cpp \
    Plotlib/Band/band.cpp \
    Plotlib/Band/label.cpp \
    Plotlib/Band/placer.cpp \
    Plotlib/Band/plotareaband.cpp \
    Plotlib/Band/scale.cpp \
    Plotlib/Manager/Impl/managerbasic.cpp \
    Plotlib/Manager/Impl/manageritem.cpp \
    Plotlib/Manager/event.cpp \
    Plotlib/Manager/manager.cpp \
    Plotlib/Manager/placer.cpp \
    Plotlib/Manager/singlearea.cpp \
    Plotlib/Visualization/visualization.cpp \
    Plotlib/Visualization/visualizationwidget.cpp \
    SQLiteData/sqlite3.c \
    SQLiteData/sqlitedata.cpp \
    Test/tests.cpp \
    Test/tickalgorithm.cpp \
    filewidget.cpp \
    main.cpp \
    mainwindow.cpp \
    scalewidget.cpp \
    testscalewidget.cpp \
    testscalewidget2.cpp

HEADERS += \
    Datafile/Test/source2dtestfile.h \
    Datafile/datasource2d.h \
    Datafile/datasource2doptimized.h \
    Datafile/entry.h \
    Datafile/error.h \
    Datafile/escape.h \
    Datafile/file.h \
    Datafile/filesystem.h \
    Datafile/opener.h \
    Dewesoft/DWLoadLib.h \
    Dewesoft/dw7data.h \
    Dewesoft/dw7datafile.h \
    Dewesoft/dw7util.h \
    Mathlib/basicfunctions.h \
    Mathlib/floating.h \
    Mathlib/function.h \
    Mathlib/helper.h \
    Mathlib/range.h \
    Mathlib/rangemap.h \
    Mathlib/sum.h \
    PlotLib/scrollmanager.h \
    PlotLib/tickalgorithmbasic.h \
    PlotLib/ticklist.h \
    Plotlib/Area/Impl/plotarea.h \
    Plotlib/Area/Impl/plotarea2d.h \
    Plotlib/Area/Impl/plotareagrid.h \
    Plotlib/Area/area.h \
    Plotlib/Area/grid.h \
    Plotlib/Area/placer.h \
    Plotlib/Band/band.h \
    Plotlib/Band/label.h \
    Plotlib/Band/placer.h \
    Plotlib/Band/plotareaband.h \
    Plotlib/Band/scale.h \
    Plotlib/Manager/Impl/managerbasic.h \
    Plotlib/Manager/Impl/manageritem.h \
    Plotlib/Manager/basic.h \
    Plotlib/Manager/event.h \
    Plotlib/Manager/item.h \
    Plotlib/Manager/manager.h \
    Plotlib/Manager/placer.h \
    Plotlib/Manager/singlearea.h \
    Plotlib/Visualization/visualization.h \
    Plotlib/Visualization/visualizationwidget.h \
    SQLiteData/sqlite3.h \
    SQLiteData/sqlitedata.h \
    Test/tests.h \
    Test/tickalgorithm.h \
    filewidget.h \
    mainwindow.h \
    scalewidget.h \
    testscalewidget.h \
    testscalewidget2.h

FORMS += \
    filewidget.ui \
    mainwindow.ui \
    testscalewidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
