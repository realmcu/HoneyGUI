@echo off

rem 设置源目录和目标目录，可以是相对路径，也可以是绝对路径
set "SOURCE_DIR=%~dp0..\gui_engine"
set "TARGET_DIR=%~dp0output\include"

rem 创建目标目录（如果不存在的话）
if not exist "%TARGET_DIR%" (
    mkdir "%TARGET_DIR%"
)

rem 拷贝头文件到目标目录
xcopy "%SOURCE_DIR%\*.h" "%TARGET_DIR%\" /S /E /Y /I

exit /b %ERRORLEVEL%
