#include "Grayscale.h"

std::string Grayscale::name() const {
    return "grayscale";
}

void Grayscale::apply(Grid& pixels) {
    for (Row& row : pixels) {
        for (Pixel& p : row) {
            // TODO: compute the average of r, g, b
            int avg = 0; // fix this

            // TODO: set r, g, and b to avg
        }
    }
}
