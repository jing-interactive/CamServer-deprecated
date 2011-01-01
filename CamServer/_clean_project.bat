del *.ncb /Q
del *.opt /Q
del *.plg /Q
del *.ilk
del *.orig
del *._xe
del *.map
del *.pbi
del *.pbo
del *.pbt
del *.pdb
del *.suo

rmdir Debug /Q /S
REM rmdir Release /Q /S

cd CamServer
rmdir Debug /Q /S
rmdir Release /Q /S