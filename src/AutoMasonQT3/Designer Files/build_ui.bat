uic -o ..\Source\bbb_UI.h Layout_02.ui
uic -o ..\Source\bbb_UI.cpp -impl bbb_UI.h Layout_02.ui
rem moc ..\Source\bbb_UI.cpp -o ..\Source\bbb_UI_moc.cpp