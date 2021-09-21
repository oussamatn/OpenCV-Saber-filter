/*
 * Code written by Vinz (GeckoGeek.fr) 14/03/2010
 * http://www.geckogeek.fr/lire-le-flux-dune-webcam-camera-video-avec-opencv.html
 * And modified by Rostom Kachouri; 16/02/2016
 * 5E-LE4
 */
#define PIX_SORT(a, b) {if ((a) > (b)) PIX_SWAP((a), (b));}
#define PIX_SWAP(a, b) {int temp = (a);(a) = (b);(b) = temp;}

#include <stdio.h>

#include <iostream>

#include <stdlib.h>
// Switch to OpenCV2
#include  "/usr/include/opencv2/opencv.hpp"

#include <opencv2/core/core.hpp>

#include <opencv2/core.hpp>

#include <opencv2/highgui.hpp>

#include "/usr/include/opencv2/imgproc.hpp"

// Global variables :
// Limite frame number
int frame_limit = 20;
// Images
cv::Mat Image_IN;
cv::Mat Image_GRAY;
cv::Mat Image_SOBEL;
cv::Mat Image_MEDIAN;

// variables images
int height, width;  // Taille de l'image

// Sobel filter X
int G_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};

int G_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

int median_opti9(int* p) {
  PIX_SORT(p[1], p[2]);
  PIX_SORT(p[4], p[5]);
  PIX_SORT(p[7], p[8]);
  PIX_SORT(p[0], p[1]);
  PIX_SORT(p[3], p[4]);
  PIX_SORT(p[6], p[7]);
  PIX_SORT(p[1], p[2]);
  PIX_SORT(p[4], p[5]);
  PIX_SORT(p[7], p[8]);
  PIX_SORT(p[0], p[3]);
  PIX_SORT(p[5], p[8]);
  PIX_SORT(p[4], p[7]);
  PIX_SORT(p[3], p[6]);
  PIX_SORT(p[1], p[4]);
  PIX_SORT(p[2], p[5]);
  PIX_SORT(p[4], p[7]);
  PIX_SORT(p[4], p[2]);
  PIX_SORT(p[6], p[4]);
  PIX_SORT(p[4], p[2]);
  return (p[4]);
}
void insertionSort(int array[]) {
  int temp, i, j;
  for (i = 0; i < 9; i++) {
    temp = array[i];
    for (j = i - 1; j >= 0 && temp < array[j]; j--) {
      array[j + 1] = array[j];
    }
    array[j + 1] = temp;
  }
}

void grey_filter() {
  int i, j;  // indices des boucles

  int rows = Image_IN.rows;
  int cols = Image_IN.cols;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      Image_GRAY.at<uchar>(i, j) =
          0.2126 * (float)Image_IN.at<cv::Vec3b>(i, j)[2] +
          0.7152 * (float)Image_IN.at<cv::Vec3b>(i, j)[1] +
          0.0722 * (float)Image_IN.at<cv::Vec3b>(i, j)[0];
    }
  }
}

void grey_filter_2() {
  CV_Assert(Image_IN.type() == CV_8UC3);
  int rows = Image_IN.rows, cols = Image_IN.cols;

  Image_GRAY.create(Image_IN.size(), CV_8UC1);

  if (Image_IN.isContinuous() && Image_GRAY.isContinuous()) {
    cols = rows * cols;
    rows = 1;
  }

  for (int row = 0; row < rows; row++) {
    const uchar* Image_IN_ptr = Image_IN.ptr<uchar>(row);
    uchar* Image_GRAY_ptr = Image_GRAY.ptr<uchar>(row);

    for (int col = 0; col < cols; col++) {

      Image_GRAY_ptr[col] = Image_IN_ptr[0] | Image_IN_ptr[1] | Image_IN_ptr[2];
      Image_IN_ptr += 3;
      
    }
  }
}

void median_filter() {
  int a, b, k, i, j;  // indices des boucles

  int data_array[9];
  int delta[3 * 3][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 0},
                         {0, 1},   {1, -1}, {1, 0},  {1, 1}};
  for (i = 1; i < Image_GRAY.rows - 1; i++)
    for (j = 1; j < Image_GRAY.cols - 1; j++) {
      k = 0;
      while (k < 9)
        data_array[k++] = Image_GRAY.at<uchar>(i + delta[k][0], j + delta[k][1]);
      // Get Median value
      Image_MEDIAN.at<uchar>(i, j) = median_opti9(data_array);

    }
}

// Optimised V2
void filtre_sobel_v2() {
  int Gx_sum, Gy_sum, G;  // Sobel var
  int i, j, a, b;  // indices des boucles
  for (j = 0; j < Image_MEDIAN.rows - 2; j++) {
    for (i = 0; i < Image_MEDIAN.cols - 2; i++) {
      // Sobel Vertical Gy
      // L0
      Gy_sum =
          (G_y[0][0] * (int)Image_MEDIAN.at<uchar>(j, i)) +
          (G_y[0][1] * (int)Image_MEDIAN.at<uchar>(j + 1, i)) +
          (G_y[0][2] * (int)Image_MEDIAN.at<uchar>(j + 2, i)) +
          /*(G_y[1][0] * (int)Image_MEDIAN.at<uchar>(j,i+1)) + (G_y[1][1] *
             (int)Image_MEDIAN.at<uchar>(j+1,i+1)) + (G_y[1][2] *
             (int)Image_MEDIAN.at<uchar>(j+2,i+1)) +*/
          (G_y[2][0] * (int)Image_MEDIAN.at<uchar>(j, i + 2)) +
          (G_y[2][1] * (int)Image_MEDIAN.at<uchar>(j + 1, i + 2)) +
          (G_y[2][2] * (int)Image_MEDIAN.at<uchar>(j + 2, i + 2));

      // Sobel Horizental Gx
      Gx_sum = (G_x[0][0] * (int)Image_MEDIAN.at<uchar>(j, i)) +
               /*(G_x[0][1] * (int)Image_MEDIAN.at<uchar>(j+1,i)) +*/ (
                   G_x[0][2] * (int)Image_MEDIAN.at<uchar>(j + 2, i)) +
               (G_x[1][0] * (int)Image_MEDIAN.at<uchar>(j, i + 1)) +
               /*(G_x[1][1] * (int)Image_MEDIAN.at<uchar>(j+1,i+1)) + */ (
                   G_x[1][2] * (int)Image_MEDIAN.at<uchar>(j + 2, i + 1)) +
               (G_x[2][0] * (int)Image_MEDIAN.at<uchar>(j, i + 2)) +
               /*(G_x[2][1] * (int)Image_MEDIAN.at<uchar>(j+1,i+2)) +*/ (
                   G_x[2][2] * (int)Image_MEDIAN.at<uchar>(j + 2, i + 2));

      // Calcul du gradient (G(x,y)=(Gx(x,y)^2 + Gy(x,y)^2)^(1/2)
      G = sqrt(abs(Gx_sum) * abs(Gx_sum) + abs(Gy_sum) * abs(Gy_sum));

      // Seuils White/Black
      G = G > 128 ? 255 : 0;
      G = G < 127 ? 0 : G;

      Image_SOBEL.at<uchar>(j, i) = (uchar)G;

      Gx_sum = 0;
      Gy_sum = 0;
    }
  }
}

void get_res(int res ,int *w,int *h){
  if(res < 10 ){
    int list_w[9] = {1280,640,544,320,432, 160,800,960,1024};
    int list_h[9] = {720,480,288,240,120,600,480,720,576};
    *w = list_w[res];
    *h = list_h[res];
  }else{
    *w= 432; *h = 240;
  }
}

int main(int argc, char* argv[]) {
  // Get video input from commande line
  char* a = argv[1];
  
  int video_input = 0;
  video_input = atoi(a);
  
  if(argc < 3 ) { width= 432; height = 240; 
  }else {
    char* b = argv[2];
    int res = atoi(a);
    get_res(res,&width,&height);
  }
  // intialisation de la variable ESC_keyboard
  char ESC_keyboard;
  // intialisation de la variable ESC_keyboard
  ESC_keyboard = cvWaitKey(5);

  // Ouvrir le flux vidéo
  cv::VideoCapture capture(video_input);
  
  // Set Frame resolution 
  capture.set(CV_CAP_PROP_FRAME_WIDTH, width);
  capture.set(CV_CAP_PROP_FRAME_HEIGHT, height);


  // Définition des fenêtres
  cv::namedWindow("Image_IN_Window", CV_WINDOW_AUTOSIZE);    // Image_IN
  cv::namedWindow("Image_GRAY_Window", CV_WINDOW_AUTOSIZE);  // Image_GRAY
  cv::namedWindow("Image_SOBEL", CV_WINDOW_AUTOSIZE);        // Image_SOBEL
  cv::namedWindow("Image_MEDIAN", CV_WINDOW_AUTOSIZE);       // Image_SOBEL

  // Positionnement des fenêtres
  cv::moveWindow("Image_IN_Window", 0, 0);
  cv::moveWindow("Image_GRAY_Window", 0, 700);
  cv::moveWindow("Image_SOBEL", 1, 700);
  cv::moveWindow("Image_MEDIAN", 1, 700);

  // Première acquisition
  capture >> Image_IN;
  
  if (!Image_IN.type()) {
    printf("Ouverture du flux vidéo impossible ( video_input = %d ) !\n",
           video_input);
    return 1;
  }
  // Création de l'image de sortie
  Image_GRAY = cv::Mat::zeros(Image_IN.size(), CV_8UC1);
  //   // Création de l'image de sortie pour Sobel
  Image_SOBEL = cv::Mat::zeros(Image_IN.size(), CV_8UC1);
  //   // Création de l'image de sortie pour Sobel
  Image_MEDIAN = cv::Mat::zeros(Image_IN.size(), CV_8UC1);

  // Boucle tant que l'utilisateur n'appuie pas sur la touche q (ou Q)
  while (frame_limit) {
    // On récupère une Image_IN
    capture >> Image_IN;

    // conversion RGB en niveau de gris

    grey_filter();

    // Application du filtre median

    median_filter();

    // Application du filtre SOBEL
    filtre_sobel_v2();

    // On affiche des frames dans une fenêtre
    cv::imshow("read video", Image_IN);
    cv::imshow("Image_GRAY_Window", Image_GRAY);
    cv::imshow("Image_SOBEL", Image_SOBEL);
    cv::imshow("Image_MEDIAN", Image_MEDIAN);

    // On attend 5ms
    cvWaitKey(5);
    frame_limit--;
    printf("frame : %d \n\r", frame_limit);

    if (frame_limit == 0) {
      // Saving a copy of images for comparison
      printf("Saving images ...\n\r");

      cv::imwrite("img/opencv3_Image_IN.jpg", Image_IN);
      cv::imwrite("img/opencv3_Image_GRAY.jpg", Image_GRAY);
      cv::imwrite("img/opencv3_Image_MEDIAN.jpg", Image_MEDIAN);
      cv::imwrite("img/opencv3_Image_SOBEL.jpg", Image_SOBEL);
    }
  }

  // Fermeture des fenêtres d'affichage
  cv::destroyWindow("Image_IN_Window");
  cv::destroyWindow("Image_GRAY_Window");
  cv::destroyWindow("Image_SOBEL");
  cv::destroyWindow("Image_MEDIAN");

  system("pause");
  return 0;
}

#undef PIC_SORT
#undef PIC_SWAP