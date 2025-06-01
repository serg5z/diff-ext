# Contributing to diff-ext

Thank you for your interest in contributing to **diff-ext**! Your involvement helps improve this Windows shell extension, which integrates file comparison tools directly into the context menu.

## Table of Contents

* [Getting Started](#getting-started)
* [How to Contribute](#how-to-contribute)

  * [Reporting Bugs](#reporting-bugs)
  * [Suggesting Enhancements](#suggesting-enhancements)
  * [Submitting Changes](#submitting-changes)
* [Development Setup](#development-setup)
* [Coding Guidelines](#coding-guidelines)
* [Code of Conduct](#code-of-conduct)
* [Contact](#contact)

## Getting Started

To get started:

1. **Fork the Repository**: Click the "Fork" button at the top right of the repository page.
2. **Clone Your Fork**: Clone your forked repository to your local machine:

   ```powershell
   git clone https://github.com/your-username/diff-ext.git
   ```
3. **Create a Branch**: Create a new branch for your changes:

   ```powershell
   git checkout -b feature/your-feature-name
   ```

## How to Contribute

### Reporting Bugs

If you encounter a bug:

* **Check Existing Issues**: Ensure the bug hasn't already been reported.
* **Create a New Issue**: Provide a clear and descriptive title.
* **Include Details**:

  * Steps to reproduce the issue.
  * Expected and actual behavior.
  * Screenshots, if applicable.
  * Your environment (OS version, diff-ext version).

### Suggesting Enhancements

To suggest a new feature or improvement:

* **Search Existing Issues**: Ensure the enhancement hasn't already been suggested.
* **Open a New Issue**: Provide a detailed description of the enhancement.
* **Explain the Benefit**: Describe how the enhancement would improve diff-ext.

### Submitting Changes

To submit code changes:

1. **Commit Your Changes**: Write clear and concise commit messages.
2. **Push to Your Fork**: Push your changes to your forked repository.
3. **Open a Pull Request**: Navigate to the original repository and open a pull request from your branch.
4. **Describe Your Changes**: In the pull request, explain what changes you've made and why.

## Development Setup

To set up the development environment:

1. **Install Prerequisites**:

   * [Visual Studio](https://visualstudio.microsoft.com/) with C++ support.
   * [CMake](https://cmake.org/download/).
   * [Inno Setup](https://jrsoftware.org/isinfo.php) for installer creation.

2. **Configure the Project**:

   ```powershell
   cmake -S . -B build -A x64
   ```

3. **Build the Project**:

   ```powershell
   cmake --build build --config Release
   ```

## Coding Guidelines

* **Code Style**: Follow the existing code style and conventions.
* **Clang-Tidy**: Use `clang-tidy` for static code analysis.
* **Commit Messages**:

  * Use the present tense ("Add feature" not "Added feature").
  * Be concise but descriptive.

## Contact

If you have questions or need assistance, feel free to open an issue or contact the maintainers directly.

---

Thank you for contributing to diff-ext!

---
