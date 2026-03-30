#pragma once
#include <string>
#include <vector>

struct Pixel {
    int r, g, b;
};

using Row  = std::vector<Pixel>;
using Grid = std::vector<Row>;

// Abstract base class for all image filters.
// Each concrete filter must implement apply() and name().
class Filter {
public:
    // Apply the filter to the pixel grid in place.
    virtual void apply(Grid& pixels) = 0;

    // Return a human-readable name used by Pipeline::printSteps().
    virtual std::string name() const = 0;

    virtual ~Filter() = default;
};
