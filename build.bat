@echo off

IF NOT DEFINED VCINSTALLDIR (
	echo [91;4mMSVC not initialized![0m
	echo You must execute vcvarsall.bat before executing this batch file.
	echo It is contained inside your VS install folder in a path similar to
	echo C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat
	EXIT /B
)

IF NOT EXIST build mkdir build

pushd build

SET IncludeFolders=
SET IgnoredWarnings=
SET Libraries=

SET CompilerFlags=/nologo /MTd /fp:fast /GR- /EHa- /Od /Oi /WX /W4 /FC /Zi %IncludeFolders% %Parameters% %IgnoredWarnings%
SET LinkerFlags=/nologo /incremental:no /opt:ref %Libraries%

cl %CompilerFlags% ..\main.cpp /link %LinkerFlags%

popd