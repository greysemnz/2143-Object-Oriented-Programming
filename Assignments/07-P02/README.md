```yaml
title: Program 2
description: Cmd Line Args
id: 07-P02
name: 07-P02
category: program
date_assigned: 2026-03-02 12:00
date_due: 2026-03-16 11:00
resources: []
```

# Program2: Build a Command-Line Args Parser (C++)

## Overview

You are writing a small command-line argument parser for an image processing tool called `imgtool`.

This assignment is not about fancy libraries. It’s about:

- reading `argc/argv`
- string handling
- validating inputs
- designing a clean class that packages parsing into a reusable component

You will create an `Args` class (or `struct`) and a parsing method that converts raw CLI tokens into a validated configuration object.

You will **not** perform actual image processing in this assignment. Your job is configuration and correctness.

---

## Program Usage Requirements

Your tool will be called like this:

```bash
./imgtool <input_image> <output_image> [options]
```

### Example

```bash
./imgtool in.png out.png --grayscale --brighten 20 --blur
```

- The first two positional arguments after the program name are required:
  - `input_image`
  - `output_image`

- Everything after that is optional flags and options.

---

## Required Options

Your parser must support these options:

### Flags (no value required)

- `--grayscale` (short form: `-g`)
- `--blur` (short form: `-l`) _(yes, that’s a weird letter; life is weird)_
- `--flipH` (short form: `-h`)
- `--flipV` (short form: `-v`)

### Options with values

- `--brighten N` (short form: `-b N`)
  - `N` is an integer in **[-255, 255]**
- `--rotate N` (short form: `-r N`)
  - `N` must be one of: **{0, 90, 180, 270}**

---

## Accepted Token Formats

Your parser must accept:

### Long option styles

- `--brighten 20`
- `--brighten=20` ✅ must be supported
- `--rotate 90`
- `--rotate=90`

### Short option styles

- `-b 20`
- `-r 90`
- `-g`
- `-l`

### Optional (extra credit): grouped short flags

- `-gl` should behave like `-g -l`  
  Only valid when all grouped options are **flags** (no values).

---

## What Your Parser Must Detect (Error Rules)

If anything is wrong, your parser must:

1. **Stop parsing**
2. **Produce a helpful error message**
3. **Show a usage line**
4. Return a failure status (or throw an exception that `main` catches)

### You must detect and reject:

#### 1) Missing required positional args

```bash
./imgtool in.png
```

Error: missing output file

#### 2) Unknown options

```bash
./imgtool in.png out.png --graycale
```

Error: unknown option `--graycale`

#### 3) Missing option value

```bash
./imgtool in.png out.png --brighten
```

Error: `--brighten` requires an integer value

#### 4) Value looks like another option

```bash
./imgtool in.png out.png --brighten --blur
```

Error: `--brighten` expected a value but got option `--blur`

#### 5) Invalid integer format

```bash
./imgtool in.png out.png --brighten twenty
```

Error: invalid integer for `--brighten`: `twenty`

#### 6) Out of range values

```bash
./imgtool in.png out.png --brighten 999
```

Error: brighten must be in [-255, 255]

#### 7) Invalid rotate values

```bash
./imgtool in.png out.png --rotate 45
```

Error: rotate must be one of {0, 90, 180, 270}

---

## “Whitespace Problems” Clarification

Your program receives arguments already split by the shell.

This means:

- `--brighten 20` arrives as two tokens
- `--brighten    20` still arrives as two tokens (extra spaces don’t matter)
- `--brighten 20 30` arrives as three tokens (and you must decide what to do with the extra token)

If a filename contains spaces, it must be quoted by the user:

```bash
./imgtool "my input.png" out.png --grayscale
```

You are not required to support unquoted filenames with spaces (that’s the shell’s job).

---

## Output Requirements (What You Print)

If parsing succeeds, print a clean summary like this:

```
INPUT  : in.png
OUTPUT : out.png
FLAGS  : grayscale blur flipH
PARAMS : brighten=20 rotate=90
```

Only print enabled flags/options.

If parsing fails, print:

```
Error: <message>
Usage: ./imgtool <input_image> <output_image> [options]
```

---

## Design Requirements (OOP Packaging)

You must implement:

### A) An `Args` type

Your `Args` must store:

- input filename
- output filename
- flag booleans
- option values (brighten, rotate) and whether they were provided

### B) A parsing method

You must provide **one** of these interfaces:

#### Option 1 (recommended): static factory method

<!-- ```cpp -->

~~static Args parse(int argc, char\* argv[]);~~

<!-- ``` -->

#### Option 2: parsing constructor

```cpp
Args(int argc, char* argv[]);
```

You choose. Be prepared to justify it.

---

## Suggested Data Members (Not Mandatory)

You may use something like:

- `std::string input, output;`
- `bool grayscale, blur, flipH, flipV;`
- `bool use_brighten; int brighten;`
- `bool use_rotate; int rotate;`

OR any equivalent design.

---

## Implementation Hints (Tools You’re Allowed to Use)

You may use:

- `std::string`
- `std::stoi` (with error handling!)
- `<vector>`, `<optional>` (optional)
- helper functions like:
  - `bool starts_with_dash(const std::string&)`
  - `bool is_option(const std::string&)`
  - `std::pair<std::string,std::string>` split on `=`

You may **not** use external CLI parsing libraries.

---

## Extra Credit Ideas

Pick one:

1. Support grouped short flags: `-glhv`
2. Suggest likely intended option for typos (simple version: check edit distance <= 2)
3. Collect all errors and print them all instead of failing on the first one

---

## Submission Requirements

Submit:

- `Args.h`
- `Args.cpp`
- `main.cpp` (that only calls parse and prints the summary)
- `README.md` describing:
  - how to build
  - supported options
  - 5 example commands (at least 2 invalid)

---

## Rubric (Logic > Syntax)

| Category                                  | Points  |
| ----------------------------------------- | ------- |
| Correct parsing of positional args        | 10      |
| Correct parsing of flags                  | 15      |
| Correct parsing of valued options         | 20      |
| Correct support for `--opt=value` format  | 10      |
| Error handling messages (clear + correct) | 20      |
| Range checking and validation rules       | 15      |
| Clean packaging / class design            | 10      |
| **Total**                                 | **100** |

Extra credit: +10 max
