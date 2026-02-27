## Feasibility: C++ vs Python (tell-it-like-it-is)

### C++: feasible if you do one of these
**Option A (recommended): header-only image IO**
- **stb_image.h** (read) + **stb_image_write.h** (write)
- Dead simple, no build-system meltdown.
- Great for an OOP course.

**Option B: OpenCV**
- Powerful, but adds dependency friction.
- Great if your department already uses it.

**Option C: Magick++**
- Works, but install/build pain varies a lot.

For *your* class: **stb_image + stb_image_write** is the least drama.

### Python: great, but it becomes a “teach Python week”
- PIL/Pillow makes this trivial
- But you’ll spend time on Python basics, packaging, environments, etc.

So: **C++ is the move** unless you want the assignment to secretly become “intro Python bootcamp.”

---

## Key design choice: don’t replace each pixel with an emoji
That creates a file the size of a small moon.

Instead:

### Use blocks (mosaic tiles)
- Divide image into blocks: e.g. `8x8`, `16x16`, `24x24`
- Compute average color per block (or better: average in a perceptual-ish space, but optional)
- Choose emoji whose “representative color” is closest
- Paste that emoji tile into output image

This:
- produces a manageable output
- makes the algorithm faster
- makes results look like actual mosaics

---

## High-level pipeline (what students will implement)
1. Load input image
2. Load emoji set (images or pre-rendered glyphs)
3. Precompute emoji “signature” (average color, maybe more)
4. For each block in input:
   - compute block signature
   - find nearest emoji signature
   - write emoji tile into output
5. Save output image
6. Print progress to stdout + output path

That’s a perfect OOP pipeline.

---

## OOP-friendly multi-class design (clean, gradeable, extensible)

### 1) `Image` (data wrapper)
Responsibilities:
- hold pixels, width/height/channels
- get/set pixel
- extract block average (or return a `BlockSignature`)

Public API ideas:
- `Color averageColor(Rect block)`
- `void blit(const Image& tile, int x, int y)`

### 2) `Color` / `Vec3`
- store RGB
- distance function
- optional: convert to a better color space later

### 3) `EmojiTile`
Represents one emoji as an image tile.
- `std::string name;`
- `Image tileImage;`
- `Color signature;` (average or something)

### 4) `EmojiLibrary`
Loads emoji images and builds the catalog.
- `loadFromFolder(path)`
- computes signatures once
- exposes `const EmojiTile& bestMatch(Color c) const`

### 5) `Matcher` (Strategy Pattern)
Let them swap matching algorithms without changing everything:
- `NearestRGBMatcher` (Euclidean in RGB)
- (stretch) `NearestLABMatcher`
- (stretch) `KDTMatcher` for speed

### 6) `MosaicBuilder`
Orchestrates the process:
- tile size
- progress reporting
- output image construction

### 7) `ProgressReporter`
Because you want stdout feedback, and it’s a nice small OOP piece:
- prints percent, ETA optional, current tile, etc.

This is *plenty* of classes without being pointless.

---

## “Make C++ feasible” emoji sourcing options

### Option 1: Use a folder of PNG emoji tiles
This is easiest and most deterministic.
- Provide a starter emoji pack folder in the repo (like 64×64 PNGs)
- Students just load images.

### Option 2: Render emojis as fonts
Harder and platform-dependent (font rendering, missing emoji fonts, etc.).
Not recommended for intro OOP. That’s a graphics course trap.

So: **ship PNG emojis**.

---

## Output size control (so it doesn’t explode)

Let:
- input: `W x H`
- tile size: `T`
- emoji tile output size: `E` (pixels per emoji tile)

Output becomes:
- `(W/T) * E` by `(H/T) * E`

Example:
- input 1920×1080
- T=16 → 120×67 tiles
- E=32 → output 3840×2144 (big but reasonable)

That’s fine.

---

## Smart constraints for a first version (students won’t drown)
- fixed tile size (configurable via CLI arg)
- match by **average RGB** only
- use maybe **100–300 emoji tiles** (not 3000)
- brute-force nearest neighbor is fine at this scale

---

## Upgrade/bonus ideas (good for extra credit)
- Cache: if two blocks have similar signature, reuse result
- Use downsampling for faster block averages
- Replace average RGB with:
  - median color
  - luminance-weighted average
- Add “edge preservation”:
  - choose based on both color and local contrast
- Speed: KD-tree (or just sort by luminance bins)

---

## Suggested assignment milestones (multi-week, very OOP)
### Milestone 1: Image IO + basic stats
- read image
- compute average color of whole image
- write copy out

### Milestone 2: Emoji library
- load emoji tiles
- compute each emoji average color
- print top 10 closest emojis to a given test color

### Milestone 3: Mosaic build
- divide into blocks
- pick best emoji per block
- create output mosaic image

### Milestone 4 (optional): quality improvements
- better distance metric
- progress reporting
- performance

---

## Testing + grading hooks (you’ll want this)
- Deterministic output given:
  - same emoji pack
  - same tile size
- Require them to output:
  - number of blocks processed
  - time taken
  - output filename
- Provide 2–3 small test images (like 256×256) for quick grading

---

## CLI spec (clean and autograde friendly)
Example:
```bash
mosaic --in input.jpg --emoji-dir emojis/ --tile 16 --emoji-size 32 --out out.png
```

stdout:
- progress updates every N blocks
- final output path


## 1) Inheritance vs composition: where it actually fits

### Where **inheritance is useful** (polymorphism)
Use inheritance for “swappable behavior”:

- **Feature extraction** (how to summarize a block/emoji)
  - `AverageRGBExtractor`
  - `DominantColorExtractor`
  - `HistogramExtractor`
- **Distance / matching**
  - `EuclideanDistance`
  - `ChiSquareHistogramDistance`
  - `CosineDistance`
- **Search strategy**
  - `BruteForceMatcher`
  - `KDTreeMatcher` (bonus)
- **Progress reporting**
  - `ConsoleProgress`
  - `QuietProgress`
  - `TqdmStyleProgress` (if you want students to suffer slightly)

This is classic **Strategy Pattern** = inheritance/interfaces.

### Where inheritance is usually dumb here
- `Image` inheriting from something
- `EmojiTile` inheriting from `Image`
- Deep hierarchies like `PNGImage : Image : Asset : Thing`

That’s how you end up with 14 files and no one knows where pixels live.

### Where **composition** is best
- `EmojiTile` **has-a** `Image`
- `EmojiLibrary` **has-a** `vector<EmojiTile>`
- `MosaicBuilder` **has-a** `Matcher`, **has-a** `FeatureExtractor`, **has-a** `ProgressReporter`

Composition keeps responsibilities crisp and reduces inheritance abuse.

---

## 2) C++ class stubs (method headers + comments)

These are “stubs” designed for a repo skeleton. I’m using plain C++17-ish patterns, STL types, and leaving image IO backend abstract-ish so you can use `stb_image` internally without exposing it everywhere.

> NOTE: I’m writing these as if they’d go into `include/` headers. Students fill in `.cpp` bodies.

```cpp
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <optional>

// ----------------------------
// Basic types
// ----------------------------
struct ColorRGB {
    double r{0}, g{0}, b{0};   // 0..255 or 0..1, just be consistent
};

struct Rect {
    int x{0}, y{0}, w{0}, h{0};
};

// A generic feature vector so we can support:
// - 3 floats (avg RGB)
// - histograms (e.g., 48 bins)
// - multi-feature combos
using FeatureVec = std::vector<double>;


// ----------------------------
// Image (pixel container + helpers)
// ----------------------------
class Image {
public:
    Image() = default;
    Image(int w, int h, int channels);

    // Factory load/save (implementation can use stb_image/stb_image_write)
    static Image load(const std::filesystem::path& path);
    void savePng(const std::filesystem::path& path) const;

    int width() const { return w_; }
    int height() const { return h_; }
    int channels() const { return c_; }

    // Pixel access (RGBA or RGB stored in data_)
    // Keep it simple: assume RGB in data_ unless you want alpha.
    ColorRGB getPixel(int x, int y) const;
    void setPixel(int x, int y, const ColorRGB& c);

    // Extract a region (for block operations)
    // Could return a view, but copying is easier for students.
    Image crop(const Rect& r) const;

    // "Blit" (copy) a tile image into this image at (dstX, dstY)
    // This is how you paste emoji tiles into output.
    void blit(const Image& tile, int dstX, int dstY);

private:
    int w_{0}, h_{0}, c_{0};
    std::vector<uint8_t> data_; // row-major, size = w*h*c
};


// ----------------------------
// Feature Extractor (Strategy)
// ----------------------------
class IFeatureExtractor {
public:
    virtual ~IFeatureExtractor() = default;

    // Compute a feature vector for a region of an image (block)
    virtual FeatureVec extractBlock(const Image& img, const Rect& block) const = 0;

    // Compute a feature vector for an entire tile (emoji)
    virtual FeatureVec extractTile(const Image& tile) const = 0;

    // Optional: name for debugging/logging
    virtual std::string name() const = 0;
};


// Average RGB: simplest baseline
class AverageRGBExtractor : public IFeatureExtractor {
public:
    FeatureVec extractBlock(const Image& img, const Rect& block) const override;
    FeatureVec extractTile(const Image& tile) const override;
    std::string name() const override { return "AverageRGBExtractor"; }
};


// Dominant Color (histogram peak / quantized bins)
class DominantColorExtractor : public IFeatureExtractor {
public:
    // binsPerChannel = 8 => 8*8*8 = 512 bins
    explicit DominantColorExtractor(int binsPerChannel = 8);

    FeatureVec extractBlock(const Image& img, const Rect& block) const override;
    FeatureVec extractTile(const Image& tile) const override;
    std::string name() const override { return "DominantColorExtractor"; }

private:
    int bins_{8};

    // Helper to map RGB -> bin index
    int binIndex(const ColorRGB& c) const;

    // Helper to compute dominant bin color (center of bin)
    ColorRGB binCenter(int idx) const;
};


// Full histogram feature (for better matching)
class HistogramExtractor : public IFeatureExtractor {
public:
    explicit HistogramExtractor(int binsPerChannel = 8);

    FeatureVec extractBlock(const Image& img, const Rect& block) const override;
    FeatureVec extractTile(const Image& tile) const override;
    std::string name() const override { return "HistogramExtractor"; }

private:
    int bins_{8};
    int totalBins_{512}; // bins^3

    int binIndex(const ColorRGB& c) const;
};


// Multi-feature extractor: combine multiple extractors with weights
class CompositeExtractor : public IFeatureExtractor {
public:
    // Each extractor produces its own FeatureVec; we concatenate them.
    // weights: scales each extractor's output (simple but effective).
    void add(std::shared_ptr<IFeatureExtractor> fx, double weight);

    FeatureVec extractBlock(const Image& img, const Rect& block) const override;
    FeatureVec extractTile(const Image& tile) const override;
    std::string name() const override { return "CompositeExtractor"; }

private:
    struct Entry {
        std::shared_ptr<IFeatureExtractor> fx;
        double weight{1.0};
    };
    std::vector<Entry> parts_;
};


// ----------------------------
// Distance Metric (Strategy)
// ----------------------------
class IDistanceMetric {
public:
    virtual ~IDistanceMetric() = default;
    virtual double distance(const FeatureVec& a, const FeatureVec& b) const = 0;
    virtual std::string name() const = 0;
};

// Euclidean for avg RGB or general vectors
class EuclideanDistance : public IDistanceMetric {
public:
    double distance(const FeatureVec& a, const FeatureVec& b) const override;
    std::string name() const override { return "EuclideanDistance"; }
};

// Chi-square distance works well for histograms
class ChiSquareDistance : public IDistanceMetric {
public:
    double distance(const FeatureVec& a, const FeatureVec& b) const override;
    std::string name() const override { return "ChiSquareDistance"; }
};


// ----------------------------
// EmojiTile + Library
// ----------------------------
struct EmojiTile {
    std::string name;  // filename stem or emoji id
    Image tile;        // the tile image (e.g., 32x32 PNG)
    FeatureVec feature; // computed once at load-time
};

class EmojiLibrary {
public:
    // Load all images in a folder; compute features for each.
    void loadFromFolder(const std::filesystem::path& folder,
                        const IFeatureExtractor& extractor);

    size_t size() const { return tiles_.size(); }
    const EmojiTile& at(size_t i) const { return tiles_.at(i); }

    // For matchers that just need iteration
    const std::vector<EmojiTile>& tiles() const { return tiles_; }

private:
    std::vector<EmojiTile> tiles_;
};


// ----------------------------
// Matcher (Strategy)
// ----------------------------
class IMatcher {
public:
    virtual ~IMatcher() = default;

    // Return index of best matching emoji tile for a block feature
    virtual size_t bestMatch(const FeatureVec& blockFeature,
                             const EmojiLibrary& lib) const = 0;

    virtual std::string name() const = 0;
};

// Brute force is fine for 200-500 emojis
class BruteForceMatcher : public IMatcher {
public:
    BruteForceMatcher(std::shared_ptr<IDistanceMetric> metric);

    size_t bestMatch(const FeatureVec& blockFeature,
                     const EmojiLibrary& lib) const override;

    std::string name() const override { return "BruteForceMatcher"; }

private:
    std::shared_ptr<IDistanceMetric> metric_;
};


// ----------------------------
// Progress reporting (Strategy)
// ----------------------------
class IProgressReporter {
public:
    virtual ~IProgressReporter() = default;
    virtual void start(int totalBlocks) = 0;
    virtual void update(int doneBlocks) = 0;
    virtual void finish(const std::filesystem::path& outPath) = 0;
};

class ConsoleProgress : public IProgressReporter {
public:
    explicit ConsoleProgress(int everyN = 200);

    void start(int totalBlocks) override;
    void update(int doneBlocks) override;
    void finish(const std::filesystem::path& outPath) override;

private:
    int everyN_{200};
    int total_{0};
};


// ----------------------------
// Mosaic Builder (orchestrator)
// ----------------------------
struct MosaicConfig {
    int blockSize{16};      // input block size
    int emojiSize{32};      // output tile size (emoji tile dimensions)
    std::filesystem::path inputPath;
    std::filesystem::path emojiDir;
    std::filesystem::path outputPath;
};

class MosaicBuilder {
public:
    MosaicBuilder(MosaicConfig cfg,
                  std::shared_ptr<IFeatureExtractor> extractor,
                  std::shared_ptr<IMatcher> matcher,
                  std::shared_ptr<IProgressReporter> progress);

    // Runs the whole pipeline:
    // load input, load emoji library, build mosaic, save output.
    void run();

private:
    MosaicConfig cfg_;
    std::shared_ptr<IFeatureExtractor> extractor_;
    std::shared_ptr<IMatcher> matcher_;
    std::shared_ptr<IProgressReporter> progress_;

    Image buildMosaic(const Image& input, const EmojiLibrary& lib) const;
};
```

### Why this is OOP-friendly
- Students are forced to respect responsibilities:
  - image ops in `Image`
  - feature logic in extractors
  - distance logic in metrics
  - search logic in matcher
  - orchestration in builder
- You can grade **design quality** even if their matching is basic.

---

## 3) Your worry about averaging is valid (and fixable without turning into a PhD)

### Why average color sucks
Average collapses multi-modal data.  
If a block is half blue and half white, the average becomes some gross pale color that matches nothing.

Your emoji mosaic experience nailed this: averaging produces mud.

### Better: dominant color / histogram
For diagrams/figures, “dominant color” often aligns better with what the eye sees.

#### Instructor-provided “dominant color” approach (student-safe)
- Quantize RGB into bins, e.g. `8 bins/channel` → 512 bins total
- Count pixels per bin
- Dominant bin = highest count
- Use bin center color as the “feature”

This is:
- easy to implement
- deterministic
- works well for your diagram-heavy Overleaf library

#### Even better (still student-safe): histogram feature vector
Instead of just dominant bin, keep the whole normalized histogram:
- feature length = 512
- compare with Chi-square distance

This yields much better matching.

### Best design lesson: make it pluggable
You don’t need students debating color theory. You want them learning OOP.

So you provide:
- `AverageRGBExtractor` baseline
- `DominantColorExtractor` as “better”
- `HistogramExtractor` as “best”
- optionally `CompositeExtractor` to weight them

They can swap strategies without touching the rest.

That is *peak* OOP.

---

## Weighted multi-feature system (your idea, cleanly)
Example “feature system” you can set up:

- Color histogram (weight 1.0)
- Average luminance/contrast (weight 0.3)
- Edge density (weight 0.5) (optional, diagrams benefit)

Then `CompositeExtractor` concatenates and weights outputs, and the distance metric measures the combined vector.

Students experience real-world design:
> build a pipeline, not a one-liner.

---

## One warning you should absolutely include in the handout
**Don’t let them match per pixel.**  
Block size is essential or output blows up.

---

## What I can give you next (no extra questions needed)
If you say: **“generate the C++ starter skeleton repo layout”** I’ll produce:

- folder structure (`include/`, `src/`, `data/emojis/`, `tests/`)
- `main.cpp` CLI argument parsing (simple)
- `stb_image` integration points
- a sample `MosaicConfig`
- and a rubric that grades:
  - design separation
  - extensibility (strategy swapping)
  - output correctness
  - performance sanity

Also: if you want inheritance emphasized (since students like seeing it), we can explicitly require:
- at least **one Strategy interface**
- at least **two concrete strategies**
- and a short written compare/contrast: *composition vs inheritance in their design*

RubberChicken optional, but encouraged.