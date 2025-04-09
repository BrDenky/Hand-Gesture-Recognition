//First Code try

#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>


// Function to calculate the centroid of a contour
cv::Point getCentroid(const std::vector<cv::Point>& contour) {
	cv::Moments m = cv::moments(contour);
	if (m.m00 != 0) {
		return cv::Point(static_cast<int>(m.m10 / m.m00), static_cast<int>(m.m01 / m.m00));
	}
	return cv::Point(0, 0);
}


// Main function

int main() {
	// Initialize the camera
	cv::VideoCapture capture(0);
	if (!capture.isOpened()) {
		std::cerr << "Error opening the camera" << std::endl;
		return -1;
	}
	std::cout << "Camera status: OK" << std::endl;

	// Declare variables to process the images
	cv::Mat original, gray;
	cv::Rect roi(335, 12, 288, 288); // Region of interest x,y, width, height


	// Loop to capture images
	while (true) {
		capture >> original; // Capture a frame
		cv::flip(original, original, 1);  // Flip horizontally
		// Check if the frame has been captured correctly
		if (original.empty()) {
			std::cerr << "Error capturing the frame" << std::endl;
			break;
		}
		// Crop the region of interest (ROI)
		cv::Mat roiImg = original(roi); // Extract the region of interest and create a copy
		// Convert the image to grayscale
		cv::cvtColor(roiImg, gray, cv::COLOR_BGR2GRAY);
		cv::imshow("Grey", gray);

		// Apply Blur effect to reduce noise
		//cv::GaussianBlur(gray, gray, cv::Size(12, 12), 1.5, 1.5);
		blur(gray, gray, cv::Size(12, 12));

		// Show the image with blur applied
		cv::imshow("Image with Blur", gray);

		// Apply a threshold to binarize the image (INVERT IMAGE B/W, OTSU)
		cv::threshold(gray, gray, 100, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

		// Find contours
		std::vector<std::vector<cv::Point>> contours; // Improvement point with reserve()
		contours.reserve(100); // Reserve space for 100 contours
		std::vector<cv::Vec4i> hierarchy; // Vec4i is a vector of 4 integers that will store the contour hierarchy
		cv::findContours(gray.clone(), contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE); // Find the contours

		// Find the largest contour
		double maxArea = 0;
		int maxAreaIdx = -1;

		for (size_t i = 0; i < contours.size(); i++) {
			double area = cv::contourArea(contours[i]); // Calculate the contour area
			if (area > maxArea) {
				maxArea = area;
				maxAreaIdx = i; // Store the index of this contour
			}
		}

		// Only process if the largest contour is greater than 1000 pixels
		if (maxArea > 1000 && maxAreaIdx >= 0) {
			std::vector<std::vector<cv::Point>> hulls(1); // Store the points of the convex hull
			std::vector<std::vector<int>> hullI(1); // Store the points of the convex hull
			cv::convexHull(contours[maxAreaIdx], hulls[0], false); // Calculate the convex hull points
			cv::convexHull(contours[maxAreaIdx], hullI[0], false); // Calculate the convex hull indices


			// Find convexity defects
			std::vector<cv::Vec4i> defects; // Store the convexity defects
			if (contours[maxAreaIdx].size() > 3) {
				cv::convexityDefects(contours[maxAreaIdx], hullI[0], defects); // Calculate the convexity defects
			}

			// Draw the main contour
			cv::drawContours(roiImg, contours, maxAreaIdx, cv::Scalar(0, 255, 255), 1.5);

			// Draw the convex hull
			for (size_t i = 0; i < hulls[0].size(); i++) {
				int idx = (i + 1) % hulls[0].size();  // Calculate the index of the next point (close the polygon)
				line(roiImg, hulls[0][i], hulls[0][idx], cv::Scalar(255, 0, 0), 1, cv::LINE_AA);  // Draw the blue line
			}


			// Calculate the centroid to improve detection
			cv::Point centroid = getCentroid(contours[maxAreaIdx]);
			cv::circle(roiImg, centroid, 5, cv::Scalar(255, 0, 255), -1);


			// Process the space between fingers (Convexity defects)
			int fingercount = 0; // Counter for extended fingers
			for (size_t i = 0; i < defects.size(); i++) {
				cv::Point start = contours[maxAreaIdx][defects[i][0]]; // Start point
				cv::Point end = contours[maxAreaIdx][defects[i][1]]; // End point
				cv::Point farthest = contours[maxAreaIdx][defects[i][2]]; // Farthest point = contours[maxAreaIdx][defects[i][2]]; // Farthest point
				float depth = defects[i][3] / 256.0; // Depth of the defect


				// Calculate the angle to improve detection
				double angle = std::atan2(centroid.y - farthest.y, centroid.x - farthest.x) * 180 / CV_PI;

				// If the depth is greater than 30
				// considering the angle - Improvement
				if (depth > 30 && angle > 20 && angle < 160) {
					fingercount++; // Increase the extended finger count
					// Draw the convexity defects
					cv::line(roiImg, start, farthest, cv::Scalar(255, 255, 0), 1, cv::LINE_AA); // Draw the yellow line
					cv::line(roiImg, farthest, end, cv::Scalar(0, 255, 0), 1, cv::LINE_AA); // Draw the cyan line
					cv::circle(roiImg, farthest, 5, cv::Scalar(0, 0, 255), 2, 8, 0); // Draw the red circle at the farthest point
					cv::circle(roiImg, start, 5, cv::Scalar(0, 250, 0), 2, 8, 0); // Draw the green circle at the start point

				}


			}

			// Determine the message based on the number of defects
			std::string message;
			if (fingercount == 1)
				message = "2";  // 1 defect ≈ 2 extended fingers
			else if (fingercount == 2)
				message = "3";      // 2 defects ≈ 3 extended fingers
			else if (fingercount == 3)
				message = "4";       // 3 defects ≈ 4 extended fingers
			else if (fingercount == 4)
				message = "5";            // 4 defects ≈ 5 extended fingers
			else if (fingercount == 0) {
				// Check if it's a finger or a fist
				double hullArea = cv::contourArea(hulls[0]);
				double solidity = maxArea / hullArea;
				if (solidity > 0.9)
					message = "0"; // Closed fist
				else
					message = "1"; // One extended finger
			}
			else
				message = "No Fingers"; // Other number of fingers

			cv::imshow("contour", roiImg);  // Show the ROI with contours and defects
			cv::putText(roiImg, message, cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(0, 0, 255), 5, cv::LINE_AA);


		}


		//cv::imshow("Original", original); // Show the original image
		rectangle(original, roi, cv::Scalar(0, 255, 0), 2);  // Draw a green rectangle around the ROI
		cv::imshow("Gesture Recognition", roiImg);  // Show the original image with text and ROI
		//cv::imshow("ROI", roiImg); // Show the region of interest
		cv::imshow("Threshold", gray); // Show the binarized image

		// Exit with 'ESC'
		if (cv::waitKey(30) == 27) break;



	}
	// Release resources
	capture.release();
	cv::destroyAllWindows();
	return 0;

}
