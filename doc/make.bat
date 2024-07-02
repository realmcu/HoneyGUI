@ECHO OFF

pushd %~dp0

REM Command file for Sphinx documentation

if "%SPHINXBUILD%" == "" (
	set SPHINXBUILD=sphinx-build
)
set EN_SOURCEDIR=./en
set EN_BUILDDIR=_build/en
set EN_CONFDIR=./en
set CN_SOURCEDIR=./cn
set CN_BUILDDIR=_build/cn
set CN_CONFDIR=./cn

%SPHINXBUILD% >NUL 2>NUL
if errorlevel 9009 (
	echo.
	echo.The 'sphinx-build' command was not found. Make sure you have Sphinx
	echo.installed, then set the SPHINXBUILD environment variable to point
	echo.to the full path of the 'sphinx-build' executable. Alternatively you
	echo.may add the Sphinx directory to PATH.
	echo.
	echo.If you don't have Sphinx installed, grab it from
	echo.https://www.sphinx-doc.org/
	exit /b 1
)

if "%1" == "" goto help

REM sphinx-build -b html -c ./en ../ ./_build/en -D language=en
%SPHINXBUILD% -M %1 %EN_SOURCEDIR% %EN_BUILDDIR% -D language=en -c %EN_CONFDIR% %SPHINXOPTS% %O%
REM sphinx-build -b html -c ./cn ../ ./_build/cn -D language=zh_CN
%SPHINXBUILD% -M %1 %CN_SOURCEDIR% %CN_BUILDDIR% -D language=zh_CN -c %CN_CONFDIR% %SPHINXOPTS% %O%
goto end

:help
%SPHINXBUILD% -M help %SOURCEDIR% %BUILDDIR% %SPHINXOPTS% %O%

:end
popd
