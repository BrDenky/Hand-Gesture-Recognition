//Firts Code try

#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

// Función principal

int main() {
	// Inicializamos la cámara
	cv::VideoCapture capture(0);
	if (!capture.isOpened()) {
		std::cerr << "Error al abrir la cámara" << std::endl;
		return -1;
	}
	std::cout << "Status de la cámara: OK" << std::endl;

	// Obetenemos las dimensiones del fotograma
	int fWidth = capture.get(cv::CAP_PROP_FRAME_WIDTH); // Ancho del fotograma 
	int fHeight = capture.get(cv::CAP_PROP_FRAME_HEIGHT); // Alto del fotograma
	std::cout << "Dimensiones del fotograma: " << fWidth << "x" << fHeight << std::endl;

	// Declaramos variables para procesar las imágenes
	cv::Mat original, gray;
	cv::Rect roi(335, 12, 288, 288); // Región de interés x,y, ancho, alto
	
	
	// Bucle para capturar imágenes
	while (true) {
		capture >> original; // Capturamos un fotograma
		cv::flip(original, original, 1);  // Voltear horizontalmente
		// Comprobamos si el fotograma se ha capturado correctamente
		if (original.empty()) {
			std::cerr << "Error al capturar el fotograma" << std::endl;
			break;
		}
		// Recortamos la región de interés (ROI)
		cv::Mat roiImg = original(roi); // Extraemos la región de interés y creamos una copia
		// Convertimos la imagen a escala de grises
		cv::cvtColor(roiImg, gray, cv::COLOR_BGR2GRAY);
		cv::imshow("Grey", gray);

		// Aplicamos efecto Blur para reducir el ruido
		//cv::GaussianBlur(gray, gray, cv::Size(12, 12), 1.5, 1.5);
		blur(gray, gray, cv::Size(12, 12));

		// Mostramos la imagen con blur aplicado
		cv::imshow("Image with Blur", gray);

		// Aplicamos un umbral para binarizar la imagen (INVERTIR IMAGEN B/N, OTSU)
		cv::threshold(gray, gray,100, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

		// Encontramos contornos
		std::vector<std::vector<cv::Point>> contours; //Punto de mejor con reserve()
		std::vector<cv::Vec4i> hierarchy; // Vec4i es un vector de 4 enteros que almacenará la jerarquía de contornos
		cv::findContours(gray.clone(), contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE); // Encontramos los contornos

		// Buscamos el contorno más grande
		double maxArea = 0;
		int maxAreaIdx = -1;

		for (size_t i = 0; i < contours.size(); i++) {
			double area = cv::contourArea(contours[i]); // Calculamos el área del contorno
			if (area > maxArea) {
				maxArea = area;
				maxAreaIdx = i; // Guardamos el índice de este contorno
			}
		}

		// SOlo procesa si encontramos contorno más grande que 1000 pixeles
		if (maxArea > 1000 && maxAreaIdx >= 0) {
			std::vector<std::vector<cv::Point>> hulls(1); // Amacena los puntos del casco convexco
			std::vector<std::vector<int>> hullI(1); // Almacena los puntos del casco convexo
			cv::convexHull(contours[maxAreaIdx], hulls[0], false); // calculamos los puntos del casco convexo
			cv::convexHull(contours[maxAreaIdx], hullI[0], false); // Calculamos los indices del casco convexo
		

			// Buscamos defectos de convexidad
			std::vector<cv::Vec4i> defects; // Almacenamos los defectos de convexidad
			if (contours[maxAreaIdx].size() > 3) {
				cv::convexityDefects(contours[maxAreaIdx], hullI[0], defects); // Calculamos los defectos de convexidad
			}

			// Dibujamos el casco convexo
			for (size_t i = 0; i < hulls[0].size(); i++) {
				int idx = (i + 1) % hulls[0].size();  // Calculamos el índice del siguiente punto (cierra el polígono)
				line(roiImg, hulls[0][i], hulls[0][idx], cv::Scalar(255, 0, 0), 1, cv::LINE_AA);  // Dibujamos la línea azul
			}

			// Procesamos el espacio entre dedos (Defectos de convexidad)
			int fingercount = 0; // Contador de dedos extendidos
			for (size_t i = 0; i < defects.size(); i++) {
				cv::Point start = contours[maxAreaIdx][defects[i][0]]; // Punto inicial
				cv::Point end = contours[maxAreaIdx][defects[i][1]]; // Punto final
				cv::Point farthest = contours[maxAreaIdx][defects[i][2]]; // Punto más lejano = contours[maxAreaIdx][defects[i][2]]; // Punto más lejano
				float depth = defects[i][3] / 256.0; // Profundidad del defecto

				// Si la profundidad es mayor a 30
				if (depth > 40) {
					fingercount++; // Aumentamos el contador de dedos extendidos
					// Dibujamos los defectos de convexidad
					cv::line(roiImg, start, farthest, cv::Scalar(255, 255, 0), 1, cv::LINE_AA); // Dibujamos la línea amarilla
					cv::line(roiImg, farthest, end, cv::Scalar(0, 255, 0), 1, cv::LINE_AA); // Dibujamos la línea cían
					cv::circle(roiImg, farthest, 5, cv::Scalar(0, 0, 255), 2, 8, 0); // Dibujamos el circulo rojo en el punto más lejao
					cv::circle(roiImg, start, 5, cv::Scalar(0, 250, 0), 2, 8, 0); // Dibujamos el circulo verde en el punto de inicio
					
				}


			}

			// Determinar el mensaje basado en el número de defectos
			std::string message;
			if (fingercount == 1)
				message = "1";  // 1 defecto ≈ 2 dedos extendidos
			else if (fingercount == 2)
				message = "2";      // 2 defectos ≈ 3 dedos extendidos
			else if (fingercount == 3)
				message = "3";       // 3 defectos ≈ 4 dedos extendidos
			else if (fingercount == 4)
				message = "4";            // 4 defectos ≈ 5 dedos extendidos
			else
				message = "No Fingers"; // Otro número de dedos

			cv::imshow("contour", roiImg);  // Muestra la ROI con los contornos y defectos
			cv::putText(original, message, cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(0, 0, 255), 5, cv::LINE_AA);


		}
		

		//cv::imshow("Original", original); // Mostramos la imagen original
		rectangle(original, roi, cv::Scalar(0, 255, 0), 2);  // Dibuja un rectángulo verde alrededor de la ROI
		cv::imshow("Gesture Recognition", original);  // Muestra la imagen original con texto y ROI
		//cv::imshow("ROI", roiImg); // Mostramos la región de interés
		cv::imshow("Threshold", gray); // Mostramos la imagen binarizada

		// Salir con 'ESC'
		if (cv::waitKey(30) == 27) break;



	}
	// Liberar recursos
	capture.release();
	cv::destroyAllWindows();
	return 0;

}

