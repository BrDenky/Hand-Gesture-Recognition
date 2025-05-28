#ifndef PAINT_FILTER_HPP
#define PAINT_FILTER_HPP
#include "base_filter.h"
#include <vector>
#include <algorithm>

class PaintFilter : public BaseFilter {
public:
    // Constructor with adjustable parameters
    PaintFilter(int radius = 4, int intensityLevels = 20,
        int bilateral_d = 9, double bilateral_color = 75, double bilateral_space = 75)
        : radius_(radius), intensityLevels_(intensityLevels),
        bilateral_d_(bilateral_d), bilateral_color_(bilateral_color), bilateral_space_(bilateral_space) {
    }

    cv::Mat apply(const cv::Mat& input) override {
        // Apply bilateral filter to smooth while preserving edges
        cv::Mat smoothed;
        cv::bilateralFilter(input, smoothed, bilateral_d_, bilateral_color_, bilateral_space_);
        cv::Mat result = smoothed.clone();


        /* Apply oil paint effect
        For each pixel in the image (avoiding the edges) */
        for (int y = radius_; y < smoothed.rows - radius_; y++) {
            for (int x = radius_; x < smoothed.cols - radius_; x++) {
                applyOilEffect(smoothed, result, x, y);
            }
        }

        return result;
    }

private:
    int radius_;
    int intensityLevels_;
    int bilateral_d_;
    double bilateral_color_;
    double bilateral_space_;

    void applyOilEffect(const cv::Mat& src, cv::Mat& dst, int x, int y) {
        // Arrays to count intensities and accumulate colors
        std::vector<int> intensityCounts(intensityLevels_, 0);
        std::vector<cv::Vec3f> avgColors(intensityLevels_, cv::Vec3f(0, 0, 0));

        // Process pixels in the neighborhood
        for (int rY = -radius_; rY <= radius_; rY++) {
            for (int rX = -radius_; rX <= radius_; rX++) {
                int posX = x + rX;
                int posY = y + rY;

                // Check boundaries
                if (posX < 0 || posX >= src.cols || posY < 0 || posY >= src.rows)
                    continue;

                // Get pixel color in BGR
                cv::Vec3b color = src.at<cv::Vec3b>(posY, posX);

                // Calculate intensity (average of the channels)
                float intensity = (color[0] + color[1] + color[2]) / 3.0f;

                // Determine intensity level (discretized)
                int intensityLevel = std::min(static_cast<int>(intensity * intensityLevels_ / 255), intensityLevels_ - 1);

                // Increment counter and accumulate color
                intensityCounts[intensityLevel]++;
                avgColors[intensityLevel][0] += color[0];
                avgColors[intensityLevel][1] += color[1];
                avgColors[intensityLevel][2] += color[2];
            }
        }

        // Find dominant intensity level in neighborhood
        int maxIndex = 0;
        int maxValue = intensityCounts[0];

        for (int i = 1; i < intensityLevels_; i++) {
            if (intensityCounts[i] > maxValue) {
                maxValue = intensityCounts[i];
                maxIndex = i;
            }
        }

        // Calculate average color for the dominant level
        if (maxValue > 0) {
            cv::Vec3b finalColor;
            finalColor[0] = static_cast<uchar>(avgColors[maxIndex][0] / maxValue);
            finalColor[1] = static_cast<uchar>(avgColors[maxIndex][1] / maxValue);
            finalColor[2] = static_cast<uchar>(avgColors[maxIndex][2] / maxValue);

            // Assign color to the output pixel
            dst.at<cv::Vec3b>(y, x) = finalColor;
        }
    }
};
#endif
