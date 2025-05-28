#ifndef BASE_FILTER_HPP
#define BASE_FILTER_HPP

#include <opencv2/opencv.hpp> // Import the OpenCV library

// Base class for all filters
class BaseFilter {
    // Members are accessible from outside the class
public:
    virtual ~BaseFilter() = default; // Virtual destructor
    virtual cv::Mat apply(const cv::Mat& input) = 0; // Pure virtual function, with which we will implement the behavior of the filters
};

#endif
