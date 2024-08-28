@echo off

rem 设置源目录和目标目录，可以是相对路径，也可以是绝对路径
set "LIB_DIR=%~dp0build"
set "TARGET_DIR=%~dp0..\..\ui_lib"


set "CURRENT_DIR=%~dp0"
set "REAL_GUI_SOURCE_DIR=%~dp0..\gui_engine"
set "REAL_GUI_TARGET_DIR=%~dp0..\..\ui_lib\real_gui"


set "LVGL_SOURCE_DIR=%~dp0..\lvgl"
set "LVGL_TARGET_DIR=%~dp0..\..\ui_lib\lvgl"






if exist "%TARGET_DIR%" (
    for /d %%D in ("%TARGET_DIR%\*") do rd /s /q "%%D"
    del /q "%TARGET_DIR%\*"
)
rem 创建目标目录（如果不存在的话）
if not exist "%TARGET_DIR%" (
    mkdir "%TARGET_DIR%"
)

rem 拷贝头文件到目标目录

xcopy "%LIB_DIR%\*.lib" "%TARGET_DIR%\"

xcopy "%CURRENT_DIR%\script\SConscript" "%TARGET_DIR%\"

xcopy "%REAL_GUI_SOURCE_DIR%\*.h" "%REAL_GUI_TARGET_DIR%\" /S /E /Y /I
xcopy "%LVGL_SOURCE_DIR%\*.h" "%LVGL_TARGET_DIR%\" /S /E /Y /I
xcopy "%CURRENT_DIR%\lv_conf.h" "%LVGL_TARGET_DIR%\"






exit /b %ERRORLEVEL%
