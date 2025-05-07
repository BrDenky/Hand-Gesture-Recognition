#include "image_processor.h" // We include the ImageProcessor class
// We include the available filters
#include "filterssketch_filter.h" // We include the SketchFilter class
#include "filterspaint_filter.h" // We include the PaintFilter class
#include "filterscartoon_filter.h" // We include the CartoonFilter class


// Constructor of the ImageProcessor class
ImageProcessor::ImageProcessor() {
    // We initialize the available filters
    filters["sketch"] = std::make_shared<SketchFilter>(); // We create an instance of the sketch filter
    filters["paint"] = std::make_shared<PaintFilter>(); // We create an instance of the paint filter
    filters["cartoon"] = std::make_shared<CartoonFilter>(); // We create an instance of the cartoon filter
}

// We load the image (T/F)
bool ImageProcessor::loadImage(const std::string& path) {
    image = cv::imread(path);
    return !image.empty();
}

// We apply the selected filter
void ImageProcessor::applyFilter(const std::string& name) {
    // We verify if the filter exists
    if (filters.count(name)) {
        // We access the pointer
        image = filters[name]->apply(image); // If the filter exists, we apply it
    }
}


// We display the processed image in a window
void ImageProcessor::showImage(const std::string& windowName) {
    // Resize the image to 800x600 (adjust as needed)
    cv::Mat resizedImage;
    cv::resize(image, resizedImage, cv::Size(800, 600)); // Change the size as desired
    cv::imshow(windowName, resizedImage);
}

//void ImageProcessor::saveImage(const std::string& path) {
//    cv::imwrite(path, image);
//}

// We return the processed image
cv::Mat ImageProcessor::getImage() const {
    return image;
}
// We set the processed image
void ImageProcessor::setImage(const cv::Mat& img) {
    image = img;
}
