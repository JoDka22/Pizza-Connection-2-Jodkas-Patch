# Local validation

The native tests include main.cpp to exercise the actual implementation. They use a hidden real Windows window, not a geometry mock. The optional image test requires the locally supplied GOG executable at ../pizza  connection 2/Pizza Connection 2/fastfood2.exe. This file is never executed or changed. SEC_IMAGE_NO_EXECUTE mapping and a private copy-on-write corruption test check loaded-code rejection.

After the Release build, in an x86 VS2022 Developer Command Prompt, from the repository root:

```bat
cl /nologo /std:c++17 /W4 /WX /MT /EHsc /I vendors\minhook-master\include tests\compatibility_tests.cpp pc2fix\Release\buffer.obj pc2fix\Release\hde32.obj pc2fix\Release\hook.obj pc2fix\Release\trampoline.obj /Foout\compatibility_tests.obj /Feout\compatibility_tests.exe /link user32.lib /LTCG
out\compatibility_tests.exe extra -windowed another
cl /nologo /W4 /WX /MT /EHsc tests\proxy_smoke.cpp /Foout\proxy_smoke.obj /Feout\Release\proxy_smoke.exe
```

For the proxy smoke test ONLY, copy your original Bink DLL to out/Release/binkw32o.dll (never commit it), then run out/Release/proxy_smoke.exe. It checks real forwarding and initialisation in an unsupported host. Inspect out/Release/pc2fix.log for exactly one bootstrap in that process and rejection before hooks. Do not mistake this for a game playtest.

Python export audit: install pefile into an ignored local directory or your own environment and run tools/inspect_pe.py against your locally owned files. No game binary is supplied by the repository.
