// filters/sketch_filter.hpp
#ifndef SKETCH_FILTER_HPP
#define SKETCH_FILTER_HPP
#include "base_filter.h"

class SketchFilter : public BaseFilter {
private:
    double sigmaX;       // Parameter to control the blur
    double threshold1;  // Minimum threshold for Canny edge detector
    double threshold2;  // Maximum threshold for Canny edge detector
    int pencilIntensity; // Intensity of the pencil effect (0-255)

public:
    SketchFilter(double sigmaX = 0.5, double threshold1 = 30, double threshold2 = 200, int pencilIntensity = 230)
        : sigmaX(sigmaX), threshold1(threshold1), threshold2(threshold2), pencilIntensity(pencilIntensity) {
    }

    cv::Mat apply(const cv::Mat& input) override {
        // Convert to grayscale
        cv::Mat gray;
        cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);

        // Apply bilateral filter to preserve edges but smooth flat areas
        cv::Mat blurred;
        cv::bilateralFilter(gray, blurred, 9, 75, 75);

        // Invert the image
        cv::Mat inverted;
        cv::bitwise_not(blurred, inverted);

        // Apply Gaussian blur
        cv::Mat blurredInverted;
        cv::GaussianBlur(inverted, blurredInverted, cv::Size(0, 0), sigmaX);

        // Combine the inverted and blurred image using dodge blend
        cv::Mat sketch = dodgeBlend(blurred, blurredInverted);

        // Apply Canny to detect fine edges
        cv::Mat edges;
        cv::Canny(blurred, edges, threshold1, threshold2);

        // Enhance the main edges in the sketch
        cv::Mat result;
        cv::addWeighted(sketch, 0.7, 255 - edges, 0.3, 0, result);

        // Add pencil texture (controlled noise)
        result = addPencilTexture(result, pencilIntensity);

        return result;
    }

private:
    // Function to simulate Photoshop's "dodge" effect
    cv::Mat dodgeBlend(const cv::Mat& base, const cv::Mat& blend) {
        cv::Mat result = cv::Mat::zeros(base.size(), CV_8UC1);

        for (int i = 0; i < base.rows; ++i) {
            for (int j = 0; j < base.cols; ++j) {
                int baseVal = base.at<uchar>(i, j);
                int blendVal = blend.at<uchar>(i, j);

                // Dodge formula: 255 * base / (255 - blend)
                int val = (255 * baseVal) / (255 - blendVal + 1);
                result.at<uchar>(i, j) = std::min(val, 255);
            }
        }

        return result;
    }

    // Function to add pencil texture (controlled noise)
    cv::Mat addPencilTexture(const cv::Mat& input, int intensity) {
        cv::Mat noise = cv::Mat(input.size(), CV_8UC1);
        cv::randu(noise, cv::Scalar(0), cv::Scalar(50));  // Low noise

        cv::Mat textured;
        cv::addWeighted(input, 0.9, noise, 0.1, 0, textured);

        // Increase contrast slightly to simulate pencil strokes
        cv::Mat result;
        double alpha = 1.2;    // Contrast
        int beta = -10;        // Brightness
        textured.convertTo(result, -1, alpha, beta);

        return result;
    }
};
#endif
