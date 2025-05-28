#ifndef CARTOON_FILTER_HPP
#define CARTOON_FILTER_HPP
#include "base_filter.h"

class CartoonFilter : public BaseFilter {
public:
    // Constructor with customizable number of colors
    CartoonFilter(int colorLevels = 8) : k(colorLevels) {}

    //  With override, we redefine a virtual function from the base class
    cv::Mat apply(const cv::Mat& input) override {
          return color_quantization(input, k); // Apply color quantization
    }

private:
    int k = 7;  // Number of colors for quantization

	// Main function, where the imagen needs to be converted to floating-point format and be reshaped for k-means
    cv::Mat color_quantization(const cv::Mat& img, int k) {
        cv::Mat data;
        img.convertTo(data, CV_32F); // To floating-point
        data = data.reshape(1, img.rows * img.cols); // Reshape for k-means

        // Criteria for the k-means algorithm
        cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 20, 1.0);
        cv::Mat labels, centers; // Variables to store the results

        cv::kmeans(data, k, labels, criteria, 10, cv::KMEANS_RANDOM_CENTERS, centers); // Applies k-means function where centers is the result
        centers.convertTo(centers, CV_8U); // Convert centers (cluster centers) to uint8
        cv::Mat result(img.size(), img.type()); // Create result matrix

        // Iterations
        // Map each pixel to its cluster center
        for (int i = 0; i < img.rows * img.cols; i++) {
            int cluster_idx = labels.at<int>(i, 0);
            int y = i / img.cols;
            int x = i % img.cols;

            // Check if the image is in color (RGB) or grayscale
            if (img.channels() == 3) {
                // Assign the center color to the corresponding pixel
                result.at<cv::Vec3b>(y, x)[0] = centers.at<uchar>(cluster_idx, 0);
                result.at<cv::Vec3b>(y, x)[1] = centers.at<uchar>(cluster_idx, 1);
                result.at<cv::Vec3b>(y, x)[2] = centers.at<uchar>(cluster_idx, 2);
            }
            else {
                // Assign the grayscale value to the corresponding pixel
                result.at<uchar>(y, x) = centers.at<uchar>(cluster_idx, 0);
            }
        }
        cv::medianBlur(result, result, 3); // Apply median blur with 3x3 kernel
        // Image with the filter applied as return 
        return result;
    }
};
#endif
