#ifndef HISTOGRAM_DISPLAY_H
#define HISTOGRAM_DISPLAY_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>



class HistogramDisplay {

private:
	// Histogram parameters
	static const int HIST_SIZE = 256;
	static const int HIST_WIDTH = 512;
	static const int HIST_HEIGHT = 400;

	// Methods for histogram calculation
	cv::Mat calculateHistogram(const cv::Mat& image);
	void drawHistogramLines(cv::Mat& histImage, const std::vector<cv::Mat>& histograms);



public:
	// Constructor and Destructor
	HistogramDisplay();
	~HistogramDisplay();
	// Main method to show histogram
	void showHistogram(const cv::Mat& image, const std::string& windowName);

	// Method to show comparison between two images
	void showComparisonHistograms(const cv::Mat& originalImage,
		const cv::Mat& processedImage,
		const std::string& originalWindowName,
		const std::string& processedWindowName);


	// Method to close specific histogram windows
	void closeHistogramWindows(const std::string& originalWindowName,
		const std::string& processedWindowName);

	// Method to close all histogram windows
	void closeAllHistogramWindows();


	// Method to get histogram statistics (optional)
	void printHistogramStats(const cv::Mat& image, const std::string& imageName = "Image");

};

#endif // HISTOGRAM_DISPLAY_H