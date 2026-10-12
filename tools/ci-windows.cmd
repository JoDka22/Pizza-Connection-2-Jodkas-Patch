@echo off
setlocal
set "pc2_vsroot="
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "pc2_vsroot=%%i"
if not defined pc2_vsroot exit /b 1
call "%pc2_vsroot%\VC\Auxiliary\Build\vcvars32.bat" || exit /b 1
msbuild pc2fix.sln /p:Configuration=Release /p:Platform=x86 /m /v:minimal || exit /b 1
msbuild pc2fix.sln /p:Configuration=Debug /p:Platform=x86 /m /v:minimal || exit /b 1
cl /nologo /std:c++17 /W4 /WX /MT /EHsc tests\display_tests.cpp /Foout\display_tests.obj /Feout\display_tests.exe /link user32.lib || exit /b 1
out\display_tests.exe || exit /b 1
cl /nologo /std:c++17 /W4 /WX /MT /EHsc tests\presentation_tests.cpp /Foout\presentation_tests.obj /Feout\presentation_tests.exe /link user32.lib gdi32.lib || exit /b 1
out\presentation_tests.exe || exit /b 1
cl /nologo /O2 /std:c++17 /W4 /WX /MT /EHsc tests\presentation_benchmark.cpp /Foout\presentation_benchmark.obj /Feout\presentation_benchmark.exe /link user32.lib gdi32.lib || exit /b 1
out\presentation_benchmark.exe --verify-only || exit /b 1
cl /nologo /O2 /std:c++17 /W4 /WX /MT /EHsc tests\rgb565_benchmark.cpp /Foout\rgb565_benchmark.obj /Feout\rgb565_benchmark.exe /link user32.lib gdi32.lib || exit /b 1
out\rgb565_benchmark.exe --verify-only || exit /b 1
cl /nologo /std:c++17 /W4 /WX /MT /EHsc tests\performance_probe_tests.cpp /Foout\performance_probe_tests.obj /Feout\performance_probe_tests.exe || exit /b 1
out\performance_probe_tests.exe || exit /b 1
cl /nologo /std:c++17 /W4 /WX /MT /EHsc tests\presentation_lifetime_tests.cpp /Foout\presentation_lifetime_tests.obj /Feout\presentation_lifetime_tests.exe /link user32.lib || exit /b 1
out\presentation_lifetime_tests.exe || exit /b 1
cl /nologo /std:c++17 /W4 /WX /MT /EHsc tests\frame_pipeline_tests.cpp /Foout\frame_pipeline_tests.obj /Feout\frame_pipeline_tests.exe /link user32.lib || exit /b 1
out\frame_pipeline_tests.exe || exit /b 1
cl /nologo /std:c++17 /W4 /WX /MT /EHsc tests\camera_bounds_tests.cpp /Foout\camera_bounds_tests.obj /Feout\camera_bounds_tests.exe || exit /b 1
out\camera_bounds_tests.exe || exit /b 1
cl /nologo /std:c++17 /W4 /WX /MT /EHsc /I vendors\minhook-master\include tests\widescreen_safety_tests.cpp out\display-obj\Release\buffer.obj out\display-obj\Release\hde32.obj out\display-obj\Release\hook.obj out\display-obj\Release\trampoline.obj /Foout\widescreen_safety_tests.obj /Feout\widescreen_safety_tests.exe /link user32.lib gdi32.lib /LTCG || exit /b 1
out\widescreen_safety_tests.exe || exit /b 1
exit /b 0
