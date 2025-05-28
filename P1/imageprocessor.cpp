#include "image_processor.h"
#include "filterssketch_filter.h"
#include "filterspaint_filter.h"
#include "filterscartoon_filter.h"
#include <iostream>

// Constructor
ImageProcessor::ImageProcessor() {
	// Start all filters
    filters["sketch"] = std::make_shared<SketchFilter>();
    filters["paint"] = std::make_shared<PaintFilter>();
    filters["cartoon"] = std::make_shared<CartoonFilter>();
}

// Load image from a file path
bool ImageProcessor::loadImage(const std::string& path) {
    image = cv::imread(path, cv::IMREAD_COLOR);
    // Error handling
    if (image.empty()) {
        std::cerr << "Error: Could not load image from path: " << path << std::endl;
        return false;
    }
    return true;
}

// Apply a filter by its name (user selection)
void ImageProcessor::applyFilter(const std::string& name) {
    if (image.empty()) {
        std::cerr << "Error: No image loaded to apply filter." << std::endl;
        return;
    }

    // Verify and apply if filters exists
    auto it = filters.find(name);
    if (it != filters.end()) {
        try {
            cv::Mat result = it->second->apply(image);
            if (!result.empty()) {
                image = result;
            }
            else {
                std::cerr << "Error: Filter " << name << " returned empty image." << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error applying filter " << name << ": " << e.what() << std::endl;
        }
    }
    else {
        std::cerr << "Error: Filter '" << name << "' not found." << std::endl;
    }
}

// Show Image 
void ImageProcessor::showImage(const std::string& windowName) {
    if (image.empty()) {
        std::cerr << "Error: No image to display." << std::endl;
        return;
    }

	// Reduce image size for display (if it is too large)
    cv::Mat displayImage;
    if (image.cols > 800 || image.rows > 600) {
        cv::resize(image, displayImage, cv::Size(800, 600));
    }
    else {
        displayImage = image.clone();
    }

    cv::imshow(windowName, displayImage);
}

// Obtain the processed image
cv::Mat ImageProcessor::getImage() const {
    return image;
}

// Establish the processed image
void ImageProcessor::setImage(const cv::Mat& img) {
    if (!img.empty()) {
        image = img.clone();
    }
    else {
        std::cerr << "Warning: Attempting to set empty image." << std::endl;
    }
}