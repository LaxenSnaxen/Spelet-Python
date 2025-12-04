# Spelet — Snabbstart

Förberedelser (installera om du inte redan har):

- Git — https://git-scm.com/ (ladda ner och installera)
- CMake (>= 3.10) — https://cmake.org/download/ (välj Windows-installatör)
- Visual Studio (rekommenderat, välj "Desktop development with C++") — https://visualstudio.microsoft.com/
- Alternativ: MinGW-w64 / MSYS2 (om du föredrar GCC) — https://www.msys2.org/ eller https://www.mingw-w64.org/
- BearLibTerminal (byggs från källkod i `build/`) — https://github.com/cfyzium/bearlibterminal
- miniaudio (single-file audio library) — https://github.com/mackron/miniaudio

Följ dessa exakta steg i PowerShell.

1) Klona repot (om du inte redan gjort det)

```powershell
git clone https://github.com/LaxenSnaxen/Spelet-Python.git
cd Spelet-Python
```

2) Skapa `build` och klona beroenden

```powershell
if (!(Test-Path build)) { New-Item -ItemType Directory -Path build }
Set-Location build
git clone https://github.com/cfyzium/bearlibterminal.git
git clone https://github.com/mackron/miniaudio.git
Set-Location ..
```


3) Generera byggfiler och bygg (Visual Studio rekommenderas)

Visual Studio 2022, x64 (rekommenderat):
```powershell
cmake -S . -B build-msvc -G "Visual Studio 17 2022" -A x64
cmake --build build-msvc --config Release
```


4) Kör programmet
- Leta efter exe i `build-msvc/bin` (Visual Studio) eller `build/bin` (MinGW).