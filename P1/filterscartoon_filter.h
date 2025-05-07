// filters/cartoon_filter.hpp
#ifndef CARTOON_FILTER_HPP
#define CARTOON_FILTER_HPP
#include "base_filter.h"

class CartoonFilter : public BaseFilter {
public:
    // Constructor with customizable number of colors
    CartoonFilter(int colorLevels = 8) : k(colorLevels) {}

    //  With override, we redefine a virtual function from the base class
    cv::Mat apply(const cv::Mat& input) override {
        // We apply color quantization
        return color_quantization(input, k);
    }

private:
    int k = 7;  // Number of colors for quantization

    // Color quantization function
    cv::Mat color_quantization(const cv::Mat& img, int k) {
        // Convert the image to floating-point format and reshape for k-means
        cv::Mat data;
        // Transformation to floating-point for k-means
        img.convertTo(data, CV_32F);
        // We reshape the image for k-means
        data = data.reshape(1, img.rows * img.cols);

        // Criteria for the k-means algorithm
        cv::TermCriteria criteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 20, 1.0);

        // Variables to store the results
        cv::Mat labels, centers;

        // We apply the k-means function
        cv::kmeans(data, k, labels, criteria, 10, cv::KMEANS_RANDOM_CENTERS, centers); // centers is the result

        // Convert centers to uint8
        centers.convertTo(centers, CV_8U); // We convert centers (cluster centers) to unsigned 8-bit integer format

        // Create result matrix
        cv::Mat result(img.size(), img.type());

        // Iterations
        // Map each pixel to its cluster center
        for (int i = 0; i < img.rows * img.cols; i++) {
            int cluster_idx = labels.at<int>(i, 0);
            int y = i / img.cols;
            int x = i % img.cols;

            // We check if the image is in color (RGB) or grayscale
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
        // We return the image with the filter applied
        return result;
    }
};
#endif
