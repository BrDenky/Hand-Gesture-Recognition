#include "histogram_display.h"
#include <iostream>

// Constructor
HistogramDisplay::HistogramDisplay() {
}

// Destructor
HistogramDisplay::~HistogramDisplay() {
    cv::destroyAllWindows();
}



// Private method to calculate histogram
cv::Mat HistogramDisplay::calculateHistogram(const cv::Mat& image) {
    // Separate into 3 channels (BGR)
    std::vector<cv::Mat> bgr_planes;
    cv::split(image, bgr_planes);

    // Set histogram parameters
    float range[] = { 0, 256 };
    const float* histRange = { range };
    bool uniform = true, accumulate = false;

    // Calculate histograms for each channel
    std::vector<cv::Mat> histograms(3);
    for (int i = 0; i < 3; i++) {
        cv::calcHist(&bgr_planes[i], 1, 0, cv::Mat(), histograms[i],
            1, &HIST_SIZE, &histRange, uniform, accumulate);
    }

    // Create histogram image
    cv::Mat histImage(HIST_HEIGHT, HIST_WIDTH, CV_8UC3, cv::Scalar(255, 255, 255));

    // Normalize histograms to fit in image height
    for (int i = 0; i < 3; i++) {
        cv::normalize(histograms[i], histograms[i], 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
    }

    // Draw histograms
    drawHistogramLines(histImage, histograms);

    return histImage;
}

// Private method to draw histogram lines
void HistogramDisplay::drawHistogramLines(cv::Mat& histImage, const std::vector<cv::Mat>& histograms) {
    int bin_w = cvRound((double)HIST_WIDTH / HIST_SIZE);

    // Colors for BGR channels
    cv::Scalar colors[3] = {
        cv::Scalar(255, 0, 0),    // Blue
        cv::Scalar(0, 255, 0),    // Green  
        cv::Scalar(0, 0, 255)     // Red
    };

    // Draw histograms for each channel
    for (int channel = 0; channel < 3; channel++) {
        for (int i = 1; i < HIST_SIZE; i++) {
            cv::line(histImage,
                cv::Point(bin_w * (i - 1), HIST_HEIGHT - cvRound(histograms[channel].at<float>(i - 1))),
                cv::Point(bin_w * i, HIST_HEIGHT - cvRound(histograms[channel].at<float>(i))),
                colors[channel], 2, 8, 0);
        }
    }
}

// Public method to show histogram
void HistogramDisplay::showHistogram(const cv::Mat& image, const std::string& windowName) {
    if (image.empty()) {
        std::cerr << "Error: Cannot display histogram for empty image." << std::endl;
        return;
    }

    cv::Mat histImage = calculateHistogram(image);
    cv::imshow(windowName, histImage);
}

// Public method to show comparison histograms
void HistogramDisplay::showComparisonHistograms(const cv::Mat& originalImage,
    const cv::Mat& processedImage,
    const std::string& originalWindowName,
    const std::string& processedWindowName) {
    if (originalImage.empty() || processedImage.empty()) {
        std::cerr << "Error: Cannot display histograms for empty images." << std::endl;
        return;
    }

    // Show both histograms
    showHistogram(originalImage, originalWindowName);
    showHistogram(processedImage, processedWindowName);

    // Position windows side by side (optional)
    cv::moveWindow(originalWindowName, 50, 50);
    cv::moveWindow(processedWindowName, 600, 50);
}

// Method to close specific histogram windows
void HistogramDisplay::closeHistogramWindows(const std::string& originalWindowName,
    const std::string& processedWindowName) {
    cv::destroyWindow(originalWindowName);
    cv::destroyWindow(processedWindowName);
}

// Method to close all histogram windows
void HistogramDisplay::closeAllHistogramWindows() {
    cv::destroyAllWindows();
}



// Method to print histogram statistics (bonus feature)
void HistogramDisplay::printHistogramStats(const cv::Mat& image, const std::string& imageName) {
    if (image.empty()) {
        std::cerr << "Error: Cannot calculate stats for empty image." << std::endl;
        return;
    }

    // Separate channels
    std::vector<cv::Mat> bgr_planes;
    cv::split(image, bgr_planes);

    // Calculate mean and standard deviation for each channel
    cv::Scalar mean, stddev;
    cv::meanStdDev(image, mean, stddev);

    std::cout << "\n----- Histogram Statistics for " << imageName << " -----" << std::endl;
    std::cout << "Blue Channel  - Mean: " << mean[0] << ", Std Dev: " << stddev[0] << std::endl;
    std::cout << "Green Channel - Mean: " << mean[1] << ", Std Dev: " << stddev[1] << std::endl;
    std::cout << "Red Channel   - Mean: " << mean[2] << ", Std Dev: " << stddev[2] << std::endl;
}
