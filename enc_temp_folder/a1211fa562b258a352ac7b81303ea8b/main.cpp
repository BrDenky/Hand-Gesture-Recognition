// main.cpp
#include <iostream>
#include "image_processor.h" // We include the ImageProcessor class

int main() {
    // Create an instance of ImageProcessor
    ImageProcessor processor;
    // Variables for the image path and the menu option
    std::string path;
    int option = 0;

    // Load the image
    std::cout << "Enter image path: ";
    path = "C:\\Users\\ASUS\\Desktop\\P1\\P1\\prueba2.jpg";
    // std::cin >> path;  // Uncomment this line if you want user input

    // Error handling
    if (!processor.loadImage(path)) {
        std::cerr << "Could not load the image.\n";
        return 1;
    }

    // We create a copy of the original image to restore it after applying filters
    cv::Mat originalImage = processor.getImage().clone();




    // Main loop
    while (option != 4) {  // Option to exit the program
        // We restore the original image
        processor.setImage(originalImage.clone());

        // We display the menu
        std::cout << "\n----- Filter Menu -----\n";
        std::cout << "1. Sketch\n";
        std::cout << "2. Paint\n";
        std::cout << "3. Cartoon\n";
        std::cout << "4. Leave\n";
        std::cout << "Option: ";
        std::cin >> option;

        // We process the image according to the selected option
        switch (option) {
        case 1:
            processor.applyFilter("sketch");
            processor.showImage("Sketch Filter");
            cv::waitKey(0);
            cv::destroyWindow("Sketch Filter");
            break;
        case 2:
            processor.applyFilter("paint");
            processor.showImage("Paint Filter");
            cv::waitKey(0);
            cv::destroyWindow("Paint Filter");
            break;
        case 3:
            processor.applyFilter("cartoon");
            processor.showImage("Cartoon Filter");
            cv::waitKey(0);
            cv::destroyWindow("Cartoon Filter");
            break;
        case 4:
            std::cout << "Leaving...\n";
            break;
        default:
            std::cout << "Invalid option. Please try again..\n";
            break;
        }
    }

    return 0;
}