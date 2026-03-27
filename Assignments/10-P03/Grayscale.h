#pragma once
#include "Filter.h"

// Converts each pixel to grayscale by averaging its R, G, and B channels.
// Formula: avg = (r + g + b) / 3  then  r = g = b = avg
class Grayscale : public Filter {
public:
    void apply(Grid& pixels) override;
    std::string name() const override;
};
