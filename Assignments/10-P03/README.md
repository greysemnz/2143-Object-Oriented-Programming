```yaml
title: Program 3
description: Image Filter Pipeline
id: 10-P03
name: 10-P03
category: program
date_assigned: 2026-04-06 12:00
date_due: 2026-04-20 11:00
resources: []
```

# Program 3: Image Filter Pipeline (C++)

## Overview

You now have a working command-line parser from P02. In this assignment you'll use it — `Args` becomes the configuration that drives an actual processing pipeline.

You will build a small image filter engine using the **Strategy pattern**: each filter operation is a class that inherits from a common `Filter` interface. A `Pipeline` owns a sequence of filters and applies them in order to an image.

No external libraries. No OpenCV. Your "image" is a plain-text **PPM file** — dead simple to read and write, and any image viewer can open it.

---

## Usage

Your program is called exactly like P02's `imgtool`:

```bash
./imgtool <input_image> <output_image> [options]
```

### Example Commands

**Convert to grayscale:**
```bash
./imgtool photo.ppm out.ppm --grayscale
```

**Brighten, then flip horizontally:**
```bash
./imgtool photo.ppm out.ppm --brighten 40 --flipH
```

**Full pipeline — order matters:**
```bash
./imgtool photo.ppm out.ppm --grayscale --brighten 30 --blur --rotate 90
```

**Short flags work too:**
```bash
./imgtool photo.ppm out.ppm -g -b 20 -l
```

**Error cases still caught by Args:**
```bash
./imgtool photo.ppm out.ppm --brighten 999
# Error: brighten must be in [-255, 255]

./imgtool photo.ppm out.ppm --rotate 45
# Error: rotate must be one of {0, 90, 180, 270}
```

### Expected Output (success)

```
INPUT   : photo.ppm
OUTPUT  : out.ppm
FILTERS : grayscale -> brighten(30) -> blur -> rotate(90)
Done. Output written to out.ppm
```

---

## The Image Format: PPM (P3)

PPM is ASCII. A valid file looks like:

```
P3
4 3
255
255 0 0   0 255 0   0 0 255   255 255 0
128 0 128  0 128 128  255 128 0  64 64 64
0 0 0     255 255 255  128 128 128  32 32 200
```

- Line 1: magic number `P3`
- Line 2: `width height`
- Line 3: max value (always `255` for this assignment)
- Remaining: `R G B` triples, left-to-right, top-to-bottom

You must implement `Image::load(filename)` and `Image::save(filename)`.

Your `Image` class should store pixels as a 2D structure of RGB triples:

```cpp
struct Pixel { int r, g, b; };
using Row    = std::vector<Pixel>;
using Grid   = std::vector<Row>;
```

---

## The Filter Interface

```cpp
class Filter {
public:
    virtual void apply(Grid& pixels) = 0;
    virtual std::string name() const = 0;
    virtual ~Filter() = default;
};
```

That's it. Every filter takes a `Grid&` and modifies it in place.

See `Filter.h` and `Grayscale.h` / `Grayscale.cpp` for starter stubs.

---

## Required Filters

Implement all six. The math is straightforward — correctness matters more than cleverness.

| Class | Flag | What it does |
|---|---|---|
| `Grayscale` | `--grayscale` | For each pixel: `avg = (r+g+b)/3`, set `r=g=b=avg` |
| `Brighten` | `--brighten N` | Add `N` to each channel, clamp result to `[0, 255]` |
| `FlipH` | `--flipH` | Reverse each row |
| `FlipV` | `--flipV` | Reverse the row order |
| `Blur` | `--blur` | 3×3 box blur: each pixel becomes the average of its neighbors (clamp indices at edges — do not wrap) |
| `Rotate` | `--rotate N` | Rotate 90°, 180°, or 270° clockwise. Note: 90° and 270° swap width and height. |

---

## The Pipeline Class

```cpp
class Pipeline {
public:
    void add(Filter* f);
    void run(Grid& pixels);
    void printSteps() const;   // print ordered list: grayscale -> blur -> ...
private:
    std::vector<Filter*> filters;
};
```

`run()` calls `apply()` on each filter in order. **Order matters** — `--grayscale --brighten 50` produces a different result than `--brighten 50 --grayscale`.

---

## Wiring It Together

`main.cpp` should look roughly like this:

```cpp
int main(int argc, char* argv[]) {
    Args args = Args::parse(argc, argv);

    Image img;
    img.load(args.input);

    Pipeline pipeline;
    // build pipeline from args — see design note below
    for (const std::string& op : args.order) {
        if (op == "grayscale") pipeline.add(new Grayscale());
        if (op == "brighten")  pipeline.add(new Brighten(args.brighten));
        if (op == "blur")      pipeline.add(new Blur());
        if (op == "flipH")     pipeline.add(new FlipH());
        if (op == "flipV")     pipeline.add(new FlipV());
        if (op == "rotate")    pipeline.add(new Rotate(args.rotate));
    }

    pipeline.printSteps();
    pipeline.run(img.pixels);
    img.save(args.output);

    return 0;
}
```

---

## Design Note: Order Matters

This is the interesting design challenge. In P02, flags were stored as plain booleans — order was lost. Now order matters. You have a few options:

1. Add a `std::vector<std::string> order` field to `Args` that records flag names as they are parsed (shown above).
2. Store the pipeline directly in `Args` (less clean — mixes concerns).
3. Accept a fixed order (grayscale → brighten → blur → flip → rotate) and document it clearly.

**Pick one and justify it in your README.**

---

## What to Submit

- `Args.h / Args.cpp` (updated from P02 if needed)
- `Filter.h` (base class — provided as stub)
- `Grayscale.h / Grayscale.cpp` (provided as stub — complete this one first)
- `Brighten.h / Brighten.cpp`
- `FlipH.h / FlipH.cpp`
- `FlipV.h / FlipV.cpp`
- `Blur.h / Blur.cpp`
- `Rotate.h / Rotate.cpp`
- `Pipeline.h / Pipeline.cpp`
- `Image.h / Image.cpp`
- `main.cpp`
- `README.md` with: build instructions, 5 example commands, and your ordering justification
- At least 3 test PPM images

---

## Rubric

| Category | Points |
|---|---|
| PPM load/save correct | 15 |
| `Filter` interface + clean inheritance | 10 |
| All 6 filters implemented correctly | 30 |
| `Pipeline` runs filters in correct order | 15 |
| Correct wiring from `Args` to `Pipeline` | 15 |
| Filter ordering handled and documented | 10 |
| Code organization / class design | 5 |
| **Total** | **100** |

### Extra Credit

- Support `--resize W H` (nearest-neighbor scaling): +10
- Load a pipeline from a text file: `--pipeline ops.txt` where each line is a filter command: +10
- `Filter::preview()` prints an ASCII-art thumbnail of the pixel grid to stdout: +5
