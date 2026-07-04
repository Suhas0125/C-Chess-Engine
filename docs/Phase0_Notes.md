# Phase0_Notes.md

# Phase 0 – Project Setup

## Objective

Prepare a clean and reproducible development environment for the chess engine before writing any chess logic.

---

## Starting Point

At the beginning of this phase:

* I had no prior experience with Git, Makefiles, or C project organization.
* I had already created the basic project folders locally.
* I had previously run into Git history conflicts after creating a GitHub repository and a local repository independently.
* To start clean, I deleted both the old local Git repository (`.git`) and the GitHub repository.

This phase began from a completely clean state.

---

## Project Structure

Current project structure:

```text
C_Chess_Engine/
├── assets/
│   ├── fonts/
│   │   └── .gitkeep
│   └── pieces/
│       └── .gitkeep
├── engine/
│   └── .gitkeep
├── tests/
│   └── .gitkeep
├── ui/
│   └── .gitkeep
├── .vscode/
│   └── c_cpp_properties.json
├── .gitignore
├── Makefile
└── main.c
```

---

## Development Tools

Development tools are kept **outside** the project.

```text
Coding/
├── C_Chess_Engine/
└── DevTools/
    ├── Raylib/
    └── w64DevKit/
```

Reason:

* The project repository should contain only project files.
* Compilers and libraries can be reused by multiple projects.
* The Git repository stays clean and lightweight.

---

## Files Introduced

### `.gitignore`

Purpose:

Prevent generated files and development-specific files from being committed.

Currently ignores:

* Executables
* Object files
* Build outputs
* Tool folders

---

### `Makefile`

Purpose:

Automates the build process.

Instead of typing a long GCC command every time, the project can now be built using:

```bash
make
```

and executed with:

```bash
make run
```

---

### `.gitkeep`

Git does not track empty folders.

Empty directories that should exist in the repository contain a `.gitkeep` file so Git includes them.

---

## VS Code Configuration

Configured IntelliSense so VS Code can locate `raylib.h`.

This removes editor errors while providing autocompletion and navigation.

---

## Build Verification

Verified that:

* GCC works correctly.
* Raylib links successfully.
* The project builds using the Makefile.
* The executable launches and displays the "Hello Raylib!" window.

---

## Git Progress

Completed:

* Initialized the local Git repository using `git init`.
* Verified repository status using `git status`.
* Added `.gitkeep` files so empty project folders are tracked.

Remaining:

* Create the GitHub repository.
* Connect the remote.
* Make the initial commit.
* Push the repository.

---

## Problems Encountered

### GCC not found

Cause:

The compiler was not being invoked correctly from PowerShell.

Resolution:

Verified GCC directly and configured the correct executable path.

---

### VS Code could not find `raylib.h`

Cause:

IntelliSense include paths were not configured.

Resolution:

Added the Raylib include directory to `c_cpp_properties.json`.

---

### Makefile failed because of spaces in the path

Cause:

The Windows username contains a space (`Suhas Gundla`).

Resolution:

Quoted the paths used by the Makefile.

---

### Empty folders were not appearing in Git

Cause:

Git tracks files, not directories.

Resolution:

Added `.gitkeep` files to every empty folder.

---

## What I Learned

* A Git repository tracks files, not empty folders.
* `.gitkeep` is a common convention for preserving empty directories.
* Development tools should be kept separate from the project source.
* A `Makefile` automates the compilation process.
* VS Code IntelliSense configuration is separate from the actual compiler.
* Build tools on Windows often require quoted paths when directories contain spaces.

---

## Phase 0 Deliverable

The project now has:

* Working GCC toolchain
* Working Raylib installation
* Clean project organization
* Working Makefile
* VS Code configured
* Local Git repository initialized
* Ready to create the first GitHub commit

The development environment is prepared for implementing the chess engine.