#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h> // For console input/output
#include <iomanip> // For I/O manipulators
#include "image_processor.h"
#include "histogram_display.h"

class ConsoleUI {
private:
    ImageProcessor processor;
    HistogramDisplay histogramDisplay;
    cv::Mat originalImage;
    std::string imagePath;
	bool imageLoaded = false; // To check if an image is loaded correctly
	HANDLE hConsole; // Manipulate the console output

public:
	// Constructor initializes the console and loads a default image
    ConsoleUI() {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetupConsole();
        LoadDefaultImage();
    }

	// Visualize the console UI designed
    void SetupConsole() {
        SetConsoleTitle(TEXT("Image Filter Studio - Professional Edition"));
        COORD coord = { 120, 40 };
        SetConsoleScreenBufferSize(hConsole, coord);
        SMALL_RECT rect = { 0, 0, 119, 39 };
        SetConsoleWindowInfo(hConsole, TRUE, &rect);
        DWORD mode;
        GetConsoleMode(hConsole, &mode);
        SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }

	// Set the console text color
    void SetColor(int color) {
        SetConsoleTextAttribute(hConsole, color);
    }

	// Prints the header of the application
    void PrintHeader() {
        system("cls");
        SetColor(11);
        std::cout << "\n";
        std::cout << "    ....................................................................................................\n";
        std::cout << "    .                             IMAGE FILTER STUDIO - Professional Edition                           .\n";
        std::cout << "    .                                    Advanced Image Processing Suite                               .\n";
        std::cout << "    ....................................................................................................\n";
        SetColor(7);
        std::cout << "\n";
    }

	// Prints a separator line in the console
    void PrintSeparator(char character = ' - ', int length = 100) {
        SetColor(8);
        for (int i = 0; i < length; i++) {
            std::cout << character;
        }
        std::cout << "\n";
        SetColor(7);
    }

	// Prints the image information if loaded, otherwise prompts to load an image
    void PrintImageInfo() {
        if (imageLoaded) {
            SetColor(10);
            std::cout << "    > IMAGE LOADED SUCCESSFULLY\n";
            SetColor(14);
            std::cout << "    > Path: " << imagePath << "\n";
			std::cout << "    > Dimensions: " << originalImage.cols << " x " << originalImage.rows << " pixels\n"; // Get Dimensions
            std::cout << "    > Size: " << (originalImage.total() * originalImage.elemSize() / 1024) << " KB\n";
        }
        else {
            SetColor(12);
            std::cout << "    X NO IMAGE LOADED\n";
            SetColor(8);
            std::cout << "    Please load an image to continue...\n";
        }
        SetColor(7);
    }

	// Prints the main menu of the application
    void PrintMenu() {
        PrintSeparator(' - ', 100);
        SetColor(13);
        std::cout << "    FILTER SELECTION MENU\n\n";
        SetColor(7);

        std::cout << "    ....................................................................................................\n";
        SetColor(11);
        std::cout << "    .  [1] SKETCH FILTER     /  Creates a pencil drawing effect with edge detection                    .\n";
        SetColor(10);
        std::cout << "    .  [2] PAINT FILTER      /  Applies an oil painting style with color blending                      .\n";
        SetColor(9);
        std::cout << "    .  [3] CARTOON FILTER    /  Creates a cartoon-like appearance with color quantization              .\n";
        SetColor(14);
        std::cout << "    .  [4] LOAD NEW IMAGE    /  Browse and load a different image file                                 .\n";
        SetColor(12);
        std::cout << "    .  [0] EXIT              /  Close the application                                                  .\n";
        SetColor(7);
        std::cout << "    ....................................................................................................\n\n";
    }

	// Displays a loading animation with a message
    void ShowLoadingAnimation(const std::string& message) {
        SetColor(14);
        std::cout << "    " << message;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 4; j++) {
                std::cout << ".";
                Sleep(200);
            }
			// Erase the last 4 dots
            std::cout << "\b\b\b\b    ";
            Sleep(100);
        }
        std::cout << " <> COMPLETE!\n";
        SetColor(7);
    }

	// Loads a default image from a predefined path
    bool LoadDefaultImage() {
        imagePath = "C:\\Users\\ASUS\\Desktop\\P1\\P1\\prueba2.jpg";

        if (!processor.loadImage(imagePath)) {
            imageLoaded = false;
            return false;
        }

		// Store the original image
        originalImage = processor.getImage().clone();
        imageLoaded = true;
        return true;
    }

	// Loads the Image / User Options
    bool LoadImage() {
        PrintSeparator();
        SetColor(14);
        std::cout << "    IMAGE LOADING SECTION\n\n";
        SetColor(7);

        std::cout << "    Current path: ";
        SetColor(8);
        std::cout << "C:\\Users\\ASUS\\Desktop\\P1\\P1\\prueba2.jpg" << "\n\n";
        SetColor(7);

        std::cout << "    Options:\n";
        std::cout << "    [1] Use default path\n";
        std::cout << "    [2] Enter custom path\n";
        std::cout << "    [0] Return to main menu\n\n";
        std::cout << "    Your choice: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1:
            imagePath = "C:\\Users\\ASUS\\Desktop\\P1\\P1\\prueba2.jpg";
            break;
        case 2:
            std::cout << "    Enter image path: ";
			std::cin.ignore(); // Clean the input buffer
            std::getline(std::cin, imagePath);
            break;
        case 0:
            return false;
        default:
            SetColor(12);
            std::cout << "    X Invalid option!\n";
            SetColor(7);
            Sleep(1500);
            return false;
        }

		// Show loading animation
        ShowLoadingAnimation(" > Loading image");

		// Error load image handling
        if (!processor.loadImage(imagePath)) {
            SetColor(12);
            std::cout << "    X ERROR: Could not load the image!\n";
            std::cout << "    Please check the file path and try again.\n";
            SetColor(7);
            Sleep(2000);
            return false;
        }

        originalImage = processor.getImage().clone();
        imageLoaded = true;

        SetColor(10);
        std::cout << "    <> Image loaded successfully!\n";
        SetColor(7);
        Sleep(1500);
        return true;
    }

	// Applies the selected filter to the loaded image
    void ApplyFilter(int filterOption) {
        if (!imageLoaded) {
            SetColor(12);
            std::cout << "    X Please load an image first!\n";
            SetColor(7);
            Sleep(2000);
            return;
        }

        std::string filterType;
        std::string filterName;
        std::string windowTitle;

        switch (filterOption) {
        case 1:
            filterType = "sketch";
            filterName = "SKETCH";
            windowTitle = "Sketch Filter Result";
            break;
        case 2:
            filterType = "paint";
            filterName = "PAINT";
            windowTitle = "Paint Filter Result";
            break;
        case 3:
            filterType = "cartoon";
            filterName = "CARTOON";
            windowTitle = "Cartoon Filter Result";
            break;
        }

        PrintSeparator();
        SetColor(13);
        std::cout << "    > APPLYING " << filterName << " FILTER\n\n";
        SetColor(7);

		// Restore original image
		processor.setImage(originalImage.clone()); // Check if the image is restored correctly

        ShowLoadingAnimation(" Processing image");

        try {
            // Apply filter selected
            processor.applyFilter(filterType);

            //  Verify Image loaded
            cv::Mat processedImage = processor.getImage();
            if (processedImage.empty()) {
                SetColor(12);
                std::cout << "    X Error: Filter processing failed!\n";
                SetColor(7);
                Sleep(2000);
                return;
            }

            // Show windows
            // Original Image
            cv::Mat originalResized, processedResized;
            cv::resize(originalImage, originalResized, cv::Size(600, 400));
            cv::resize(processedImage, processedResized, cv::Size(600, 400));

            cv::imshow("Original Image", originalResized);
            cv::imshow(windowTitle, processedResized);

            // Windows position
            cv::moveWindow("Original Image", 100, 100);
            cv::moveWindow(windowTitle, 750, 100);

            ShowLoadingAnimation("Generating histograms");

            std::string originalHistTitle = "Original Histogram";
            std::string filteredHistTitle = filterName + " Histogram";

            histogramDisplay.showComparisonHistograms(
                originalImage,
                processedImage,
                originalHistTitle,
                filteredHistTitle
            );

			// Histogram windows position
            cv::moveWindow(originalHistTitle, 100, 550);
            cv::moveWindow(filteredHistTitle, 750, 550);

            // Show stadistics histograms
            std::cout << "\n";
            SetColor(11);
            std::cout << "    > HISTOGRAM STATISTICS\n";
            PrintSeparator(' - ', 50);
            SetColor(7);

			// Print histogram statistics
            histogramDisplay.printHistogramStats(originalImage, "Original Image");
            histogramDisplay.printHistogramStats(processedImage, filterName + " Filtered Image");

            SetColor(10);
            std::cout << "\n    OK! " << filterName << " filter applied successfully!\n";
            std::cout << "    X Press ESC to close windows or any other key to continue...";
            SetColor(7);

			// Wait for user input
            int key = cv::waitKey(0);

            // Close all windows
            cv::destroyAllWindows();

        }
        // Error filter applied handling
        catch (const std::exception& e) {
            SetColor(12);
            std::cout << "    X Error applying filter: " << e.what() << std::endl;
            SetColor(7);
            cv::destroyAllWindows();
            Sleep(2000);
        }
    }

    void Run() {
		int option = -1; // USer option

		// Call all initialization functions
        while (option != 0) {
            PrintHeader();
            PrintImageInfo();
            std::cout << "\n";
            PrintMenu();

            SetColor(14);
            std::cout << "    Select a valid choice: ";
            SetColor(15);
            std::cin >> option;
            SetColor(7);

			// Validate user input
            switch (option) {
            case 1:
            case 2:
            case 3:
                ApplyFilter(option);
                break;
            case 4:
                LoadImage();
                break;
				// Exit application option
            case 0:
                PrintSeparator();
                SetColor(11);
                std::cout << "    Thank you for using Image Filter Studio!\n";
                std::cout << "    Keep creating amazing art!\n";
                SetColor(7);
                cv::destroyAllWindows();
                Sleep(2000);
                break;
				// Invalid option handling
            default:
                SetColor(12);
                std::cout << "    X Invalid option! Please try again...\n";
                SetColor(7);
                Sleep(1500);
                break;
            }
        }
    }
};

int main() {
    try {
        ConsoleUI app;
		app.Run(); // Initialize and run the application
    }
	// Error handling for exceptions
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        system("pause");
    }

    return 0;
}