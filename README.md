<img width="1024" height="682" alt="image" src="https://github.com/user-attachments/assets/8b1d8672-b92c-4344-b6e5-dbc49b11685b" />

# 🧬 Khaelita AMSI & DLL Hijacking Research

A step-by-step technical analysis of **DLL Hijacking** mechanisms and their limitations within the **Windows AMSI (Antimalware Scan Interface)** environment. This project documents why a vulnerability may fail in hardened systems and how this "failure" provides deep insights into operating system architecture.

## ⚙️ Overview

**Khaelita AMSI Research** is a security project designed to explore why standard DLL Hijacking attacks fail against modern Windows defensive layers (using PowerShell as a case study) and documents the diagnostic methods (such as ProcMon) used to identify these security boundaries. It covers file-based restrictions and potential risks in third-party software environments.

## 🛠️ Step-by-Step Research Process

### Adım 1: Initial Experiment (Theoretical Approach)
**Hypothesis:** By copying `powershell.exe` to a custom directory and placing a crafted `amsi.dll` next to it, Windows will prioritize the local DLL due to the "DLL Search Order" logic, granting us control over the AMSI mechanism.

* **Action:** Developed a fake DLL in C++ that mimics the `AmsiScanBuffer` function signature and logs execution.
* **Result:** **Failed.** Even though PowerShell launched successfully, it ignored the local DLL and continued using the original AMSI library from the system directory.

### Adım 2: Diagnosis & Deep Analysis (Why did it fail?)
To understand why the OS ignored the local DLL, **Process Monitor (ProcMon)** was used to capture the "behind-the-scenes" file system activity.

<img width="704" height="144" alt="image" src="https://github.com/user-attachments/assets/d5a5199a-4a8e-405c-b920-0f16b97c256e" />

* **Observation:** ProcMon traces revealed that PowerShell did not even attempt to look for `amsi.dll` in the local directory.
* **Technical Finding:** The analysis confirmed that `powershell.exe` uses a **Hardcoded/Absolute Path** to load the library directly from **`C:\Windows\System32\amsi.dll`**. Furthermore, the process utilizes security flags during `LoadLibraryEx` calls that explicitly disable the local directory for critical security components.

### Adım 3: Injection Experiment (Forced Loading Test)
**Question:** "If the system won't load it automatically, what happens if I manually inject this DLL into the process?"

* **Action:** Used the `LoadLibrary` API via a PowerShell script to force-map the fake DLL into the running process's memory space.
* **Result:** The DLL was successfully loaded into memory (verified via logs). However, because PowerShell had already "bound" the original AMSI library during its early startup phase, the fake functions remained dormant. This demonstrates how early and tightly modern Windows processes bind their security providers.

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
