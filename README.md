# 🧩 diff-ext – File Comparison Context Menu Extension

**diff-ext** is a Windows context menu extension that enables quick file comparisons via a configurable diff tool. It includes both classic and modern extensions and packages into an MSIX installer.

---

## ✅ Prerequisites

* **Windows 10/11**, version ≥ `22621` (for modern shell support)
* **Visual Studio 2022** with:

  * Desktop development with C++
  * CMake tools for Windows
* **CMake ≥ 3.15**
* **Windows SDK** (for `makeappx.exe`)
* **PowerShell**

---

## 📁 Project Structure

```text
/
├── classic/             # Classic shell extension (IContextMenu)
├── modern/              # Modern ExplorerCommand extension
├── shared/              # Shared logic (MRU handling etc.)
├── diff-ext-setup/      # GUI setup tool
├── installer/           # InnoSetup project file
├── packaging/           # MSIX manifest and icons
├── CMakeLists.txt       # Main CMake config
└── build.ps1            # Optional multi-arch build script
```

---

## ⚙️ Build Instructions

### Step 1: Configure

Open **Developer PowerShell for VS 2022**, then:

```powershell
mkdir build-x64
cd build-x64
cmake .. -A x64 -DCMAKE_BUILD_TYPE=Release
```

Replace `x64` with `x86` or `ARM64` if needed.

### Step 2: Build

```powershell
cmake --build . --config Release
```

Binaries will appear in:

```
build-x64/bin/
```

---

## 💠 GUI Setup

Run the configuration tool to set your preferred diff tool and MRU list capacity:

```powershell
build-x64\bin\diff-ext-setup.exe
```

You can set:

* External diff tool executable path
* MRU list size (up to 64 entries)

---

## 📆 MSIX Packaging

MSIX generation runs as part of the build process:

```text
<build-dir>\msix\diff-ext_x64.msix
```

To package manually:

```powershell
makeappx.exe pack /d msix_build /p msix/diff-ext_x64.msix /overwrite
```

---

## 🔐 Code Signing (Optional)

To sign the MSIX package:

```powershell
signtool sign /fd SHA256 /a /f path\to\YourCertificate.pfx /p yourPassword /tr http://timestamp.digicert.com /td SHA256 diff-ext_x64.msix
```

---

## 🧪 Testing & Registration

Classic DLLs must be registered manually (for test installs):

```powershell
regsvr32 build-x64\bin\diff-ext-classic.dll
```

For modern shell extensions, install the MSIX package by double-clicking it.

---

## 🛉 Uninstall

To clean up:

```powershell
regsvr32 /u build-x64\bin\diff-ext-classic.dll
```

Or uninstall the MSIX from **Apps & Features**.

---

## 📄 License

BSD license.

---

## 🧵 Credits

Created by Sergey Zorin
