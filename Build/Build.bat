cd ..\Build\
copy WogDll.ex_ heroes3.exe
installb 3 0 >>1
del "%HOMMWOGDIR%\WogT1.bak.exe"
move "%HOMMWOGDIR%\WogT1.exe" "%HOMMWOGDIR%\WogT1.bak.exe"
del "%HOMMWOGDIR%\WogT1.bak.exe"
del "%HOMMWOGDIR%\WogT1.exe"
copy heroes3.exe "%HOMMWOGDIR%\WogT1.exe"
del heroes3.exe
del 1
installb_entry addon.dat "%HOMMWOGDIR%\WogT1.exe"