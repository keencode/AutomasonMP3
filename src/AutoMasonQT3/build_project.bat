set QTDIR=c:\qt-3.3.8
set PATH=%QTDIR%\bin;%PATH%
set QMAKESPEC=win32-msvc2005

moc bbb_Form.h -o moc_bbb_Form.cpp
moc bbb_GLWidget.h -o moc_bbb_GLWidget.cpp

qmake -tp vc


