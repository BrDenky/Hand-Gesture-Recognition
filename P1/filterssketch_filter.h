#ifndef SKETCH_FILTER_HPP
#define SKETCH_FILTER_HPP
#include "base_filter.h"

class SketchFilter : public BaseFilter {
private:
    double sigmaX;
    double threshold1;
    double threshold2;
    int pencilIntensity;

public:
    SketchFilter(double sigmaX = 0.5, double threshold1 = 30, double threshold2 = 200, int pencilIntensity = 230)
        : sigmaX(sigmaX), threshold1(threshold1), threshold2(threshold2), pencilIntensity(pencilIntensity) {
    }

    cv::Mat apply(const cv::Mat& input) override {
        if (input.empty()) {
            return cv::Mat();
        }

        try {
			// Image to grayscale
            cv::Mat gray;
            if (input.channels() == 3) {
                cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
            }
            else {
                gray = input.clone();
            }

			// Apply bilateral filter to reduce noise
            cv::Mat blurred;
            cv::bilateralFilter(gray, blurred, 9, 75, 75);

			// Invert image
            cv::Mat inverted;
            cv::bitwise_not(blurred, inverted);

			// Apply Gaussian blur
            cv::Mat blurredInverted;
            cv::GaussianBlur(inverted, blurredInverted, cv::Size(21, 21), sigmaX);

			// Combine both images (Blurred and blurredInverted) with dodge blend effect
            cv::Mat sketch = dodgeBlend(blurred, blurredInverted);

			// Canny edge detection and combination
            cv::Mat edges;
            cv::Canny(blurred, edges, threshold1, threshold2);

            cv::Mat result;
            cv::addWeighted(sketch, 0.7, 255 - edges, 0.3, 0, result);

			// Call function to add pencil texture
            result = addPencilTexture(result, pencilIntensity);

            // tO BGR
            cv::Mat finalResult;
            cv::cvtColor(result, finalResult, cv::COLOR_GRAY2BGR);

            return finalResult;

        }
		// Exception handling
        catch (const std::exception& e) {
            std::cerr << "Error in SketchFilter::apply: " << e.what() << std::endl;
            return cv::Mat();
        }
    }

private:
	// Function to simulate dodge blend effect
    cv::Mat dodgeBlend(const cv::Mat& base, const cv::Mat& blend) {
        cv::Mat result = cv::Mat::zeros(base.size(), CV_8UC1);

        for (int i = 0; i < base.rows; ++i) {
            for (int j = 0; j < base.cols; ++j) {
                int baseVal = base.at<uchar>(i, j);
                int blendVal = blend.at<uchar>(i, j);

                // Dodge formula: 255 * base / (255 - blend)
                if (blendVal >= 255) {
                    result.at<uchar>(i, j) = 255;
                }
                else {
                    int val = (255 * baseVal) / (255 - blendVal + 1);
                    result.at<uchar>(i, j) = std::min(val, 255);
                }
            }
        }

        return result;
    }

	// Function to add pencil texture
    cv::Mat addPencilTexture(const cv::Mat& input, int intensity) {
        cv::Mat noise = cv::Mat(input.size(), CV_8UC1);
        cv::randu(noise, cv::Scalar(0), cv::Scalar(50));

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