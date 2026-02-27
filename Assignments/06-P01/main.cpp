#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {

    if (argc < 3) {
        std::cout << "Usage: mosaic <input_image> <output_image>\n";
        return 1;
    }

    std::string inputPath = argv[1];
    std::string outputPath = argv[2];

    int width, height, channels;

    // Force 3 channels (RGB)
    unsigned char* data = stbi_load(inputPath.c_str(), &width, &height, &channels, 3);
    if (!data) {
        std::cerr << "Failed to load image: " << inputPath << "\n";
        return 1;
    }

    // number of color channels
    channels = 3;

    std::cout << "Loaded image: "
              << width << "x" << height
              << " channels: " << channels << "\n";

    // Simple processing: invert colors
    int totalPixels = width * height;

    for (int i = 0; i < totalPixels; ++i) {
        int idx = i * channels;

        double gray = 0.299 * data[idx + 0] + 0.587 * data[idx + 1] + 0.114 * data[idx + 2];

        // data[idx + 0] = 255 - data[idx + 0]; // R
        // data[idx + 1] = 255 - data[idx + 1]; // G
        // data[idx + 2] = 255 - data[idx + 2]; // B

        data[idx + 0] = gray; // R
        data[idx + 1] = gray; // G
        data[idx + 2] = gray; // B

        
    }

    // Save PNG
    if (!stbi_write_png(outputPath.c_str(),
                        width,
                        height,
                        channels,
                        data,
                        width * channels)) {
        std::cerr << "Failed to write output image\n";
        stbi_image_free(data);
        return 1;
    }

    stbi_image_free(data);

    std::cout << "Saved output to: " << outputPath << "\n";
    return 0;
}