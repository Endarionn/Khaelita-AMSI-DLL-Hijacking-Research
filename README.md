<img width="1224" height="808" alt="image" src="https://github.com/user-attachments/assets/cf247c06-f6f7-4906-a431-41133ee75c02" />

# 🧬 Khaelita AMSI & DLL Hijacking Research

A step-by-step technical analysis of **DLL Hijacking** mechanisms and their limitations within the **Windows AMSI (Antimalware Scan Interface)** environment. This project documents why a vulnerability may fail in hardened systems and how this "failure" provides deep insights into operating system architecture.

## ⚙️ Overview

**Khaelita AMSI Research** is a security project designed to explore why standard DLL Hijacking attacks fail against modern Windows defensive layers (using PowerShell as a case study) and documents the diagnostic methods (such as ProcMon) used to identify these security boundaries. It covers file-based restrictions, **KnownDLLs** protections, **DotLocal (.local)** bypasses, and potential risks in third-party software environments.

## 🛠️ Step-by-Step Research Process

### Step 1: Initial Experiment (Theoretical Approach)
**Hypothesis:** By copying `powershell.exe` to a custom directory and placing a crafted `amsi.dll` next to it, Windows will prioritize the local DLL due to the "DLL Search Order" logic, granting us control over the AMSI mechanism.

* **Action:** Developed a fake DLL in C++ that mimics the `AmsiScanBuffer` function signature and logs execution.
* **Result:** **Failed.** Even though PowerShell launched successfully, it ignored the local DLL and continued using the original AMSI library from the system directory.

### Step 2: The ".local" (DotLocal) Redirection Attempt
**Hypothesis:** If the application uses absolute paths to bypass the search order, can we force it to look into the local directory anyway? Using the **"DotLocal" (.local)** redirection hint—a legacy Windows feature—we created a directory named `powershell.exe.local` next to the executable.

<img width="635" height="135" alt="image" src="https://github.com/user-attachments/assets/d7b2c252-d0bb-4ccf-9b66-82d34020968f" />

* **Action:** Created the folder `powershell.exe.local` and placed the fake `amsi.dll` inside it, attempting to trigger the Windows Loader's redirection mechanism.
* **Result:** **Failed (Mitigated).** Modern Windows components and DotLocal-aware applications ignore this redirection for core system components and processes loaded with hardened security flags.

### Step 3: Investigating the "KnownDLLs" Mechanism
**Hypothesis:** If the local directory and .local redirection are ignored, is it because the system marks this DLL as a "KnownDLL"?

* **Action:** Checked the Windows Registry key `HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Session Manager\KnownDLLs` to see if `amsi.dll` is listed.
* **Technical Finding:** While `amsi.dll` is not always in the static KnownDLLs list, the Windows Loader treats critical security components with similar integrity. Since the loader finds the DLL in the "Known" system path or through a hardened search, it completely bypasses the application's local directory to prevent binary planting.

### Step 4: Diagnosis & Deep Analysis (Why did it fail?)
To understand why the OS ignored all hijacking attempts, **Process Monitor (ProcMon)** was used to capture the "behind-the-scenes" file system activity.

<img width="708" height="149" alt="image" src="https://github.com/user-attachments/assets/ea99bd06-abb3-442c-9ced-1dbac97d8b32" />

* **Observation:** ProcMon traces revealed that PowerShell did not even attempt to look for `amsi.dll` in the local directory or the `.local` folder.
* **Technical Finding:** The analysis confirmed that `powershell.exe` uses a **Hardcoded/Absolute Path** to load the library directly from `C:\Windows\System32\amsi.dll`. Furthermore, the process utilizes security flags during `LoadLibraryEx` calls (like `LOAD_LIBRARY_SEARCH_SYSTEM32`) that explicitly disable the local directory for critical security components.

### Step 5: Injection Experiment (Forced Loading Test)
**Question:** "If the system won't load it automatically, what happens if I manually inject this DLL into the process?"

* **Action:** Used the `LoadLibrary` API via a PowerShell script to force-map the fake DLL into the running process's memory space.
* **Result:** The DLL was successfully loaded into memory (verified via logs). However, because PowerShell had already "bound" the original AMSI library during its early startup phase (Early Binding), the fake functions remained dormant.

---

## 🚀 Theoretical Attack Scenario: Moddable Game Environments

While hardened systems like PowerShell are resilient, this research highlights a significant security gap in **third-party applications.**

**Scenario:**
Imagine a game (e.g., *R.E.P.O.*) that supports community mods. To protect players, the game uses the AMSI infrastructure to scan external scripts for malicious content.

**How an Attacker Exploits This:**
1.  An attacker prepares a mod package and includes a fake **`amsi.dll`** that appears unrelated to the mod's function but is designed to manipulate `AmsiScanBuffer`.
2.  If the game engine loads the AMSI library by name only (Relative Path) rather than specifying an Absolute Path, Windows' default search mechanism takes over.
3.  When the user launches the game, the engine looks for `amsi.dll` in its own directory first. It finds the attacker’s fake DLL and loads it into memory, believing it to be the legitimate system library.
4.  From this point on, every scan request made by the game is intercepted by the fake DLL. The fake DLL returns `AMSI_RESULT_CLEAN` for every request, allowing the actual malicious payload hidden within the mod to execute without being flagged by the antivirus.

---

## 🛠️ Tools & Techniques
* **Compiler:** MSVC (cl.exe)
* **Dynamic Analysis:** Sysinternals Process Monitor (ProcMon)
* **Registry Analysis:** Regedit (KnownDLLs Check)
* **Proof-of-Concept:** C++ DLL & PowerShell Loader

## 🔡 How to Defend Against This
* **Use Absolute Paths:** Developers must always use `GetSystemDirectory` to construct full paths for system DLLs.
* **Signature Verification:** Implement digital signature checks on modules before executing sensitive security functions.
* **SetDefaultDllDirectories:** Call this API at the start of the process to restrict the search scope to trusted system folders.

## ⚠️ Legal Notice
This software is provided for **educational, ethical research, and awareness purposes only**. Understanding how Windows security architecture works is vital for developing more resilient software. The author assumes no responsibility for misuse.

## 📄 License
MIT License — Research, learn, and secure.

## 🧠 About
Created by **Yağız Atalay** under the **Khaelita** security research label. A study on Windows Internals, DLL Loading behavior, and the evolution of AMSI defenses.
