#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <opencv2/opencv.hpp> // OpenCV library
#include <string>
#include <map>
#include <memory>
#include "base_filter.h" // Base class for the filters

// / Class to load, process, and display images
class ImageProcessor {
public:
    ImageProcessor();
    bool loadImage(const std::string& path); // We load the image (T/F)
    void applyFilter(const std::string& name); // Receives the filter name and processes it
    void showImage(const std::string& windowName = "Result"); // Receives the window name and shows the processed image
    //void saveImage(const std::string& path);
    cv::Mat getImage() const; // Returns the processed image
    void setImage(const cv::Mat& img); // Sets the processed image

    // The following members are only accessible from within the class
private:
    cv::Mat image;
    std::map<std::string, std::shared_ptr<BaseFilter>> filters; // Allows us to access the filters by their name
};

#endif
