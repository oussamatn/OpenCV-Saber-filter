/*
 * Code written by Vinz (GeckoGeek.fr) 14/03/2010
 * http://www.geckogeek.fr/lire-le-flux-dune-webcam-camera-video-avec-opencv.html
 * And modified by Rostom Kachouri; 16/02/2016
 * 5E-LE4
 */
// Helpers for optimisation
#define MEDIAN_SORT(a, b) { if ((a) > (b)) MEDIAN_SWAP((a), (b)); }
#define MEDIAN_SWAP(a, b) {int swap = (a);(a) = (b);(b) = swap;}


#include <stdio.h>

#include <time.h>

#include <stdlib.h>
// OpenCV header
#include "highgui.h"

#include "cv.h"

// Global variables :
// Limite frame number
int frame_limit = 20;
// running version by default = last: 3
int version = 3;
char buffer[1000];
// Images
IplImage* Image_IN;
IplImage* Image_GRAY;
IplImage* Image_SOBEL;
IplImage* Image_MEDIAN;

// Capture vidéo
CvCapture* capture;

// variables images
int height, width, step, channels;  // Taille de l'image
uchar* Data_in;                     // pointeur des données Image_IN
uchar* Data_out;                    // pointeur des données Image_GRAY
uchar* Sobel_out;                   // pointeur des données Sobel filter
uchar* Data_Median_out;             // pointeur des données Sobel filter
int step_gray;
// int i, j, k, a, b; // indices des boucles
// Sobel filter X
int G_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};

int G_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

int median_opti9(int* p) {
  MEDIAN_SORT(p[1], p[2]);
  MEDIAN_SORT(p[4], p[5]);
  MEDIAN_SORT(p[7], p[8]);
  MEDIAN_SORT(p[0], p[1]);
  MEDIAN_SORT(p[3], p[4]);
  MEDIAN_SORT(p[6], p[7]);
  MEDIAN_SORT(p[1], p[2]);
  MEDIAN_SORT(p[4], p[5]);
  MEDIAN_SORT(p[7], p[8]);
  MEDIAN_SORT(p[0], p[3]);
  MEDIAN_SORT(p[5], p[8]);
  MEDIAN_SORT(p[4], p[7]);
  MEDIAN_SORT(p[3], p[6]);
  MEDIAN_SORT(p[1], p[4]);
  MEDIAN_SORT(p[2], p[5]);
  MEDIAN_SORT(p[4], p[7]);
  MEDIAN_SORT(p[4], p[2]);
  MEDIAN_SORT(p[6], p[4]);
  MEDIAN_SORT(p[4], p[2]);
  return (p[4]);
}


void grey_filter() {
  int i, j;  // indices des boucles
  for (i = 0; i < height; i++)
    for (j = 0; j < width; j++)
      Data_out[i * step_gray + j] =
          0.114 * Data_in[i * step + j * channels + 0] +
          0.587 * Data_in[i * step + j * channels + 1] +
          0.299 * Data_in[i * step + j * channels + 2];
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
void median_filter_v3() {
  int a, b, k, i, j;  // indices des boucles

  int data_array[9];

  for (i = 1; i < height - 1; i++)
    for (j = 1; j < width - 1; j++) {
      k = 0;
      while (k < 9) data_array[k++] = Data_out[i * step_gray + k + j];

    // Remove function to optimise runtime ( less assembleur jump )
      MEDIAN_SORT(data_array[1], data_array[2]);
      MEDIAN_SORT(data_array[4], data_array[5]);
      MEDIAN_SORT(data_array[7], data_array[8]);
      MEDIAN_SORT(data_array[0], data_array[1]);
      MEDIAN_SORT(data_array[3], data_array[4]);
      MEDIAN_SORT(data_array[6], data_array[7]);
      MEDIAN_SORT(data_array[1], data_array[2]);
      MEDIAN_SORT(data_array[4], data_array[5]);
      MEDIAN_SORT(data_array[7], data_array[8]);
      MEDIAN_SORT(data_array[0], data_array[3]);
      MEDIAN_SORT(data_array[5], data_array[8]);
      MEDIAN_SORT(data_array[4], data_array[7]);
      MEDIAN_SORT(data_array[3], data_array[6]);
      MEDIAN_SORT(data_array[1], data_array[4]);
      MEDIAN_SORT(data_array[2], data_array[5]);
      MEDIAN_SORT(data_array[4], data_array[7]);
      MEDIAN_SORT(data_array[4], data_array[2]);
      MEDIAN_SORT(data_array[6], data_array[4]);
      MEDIAN_SORT(data_array[4], data_array[2]);
      // Get Median value
      Data_Median_out[i * step_gray + j] =  data_array[4];
    }
}
void median_filter_v2() {
  int a, b, k, i, j;  // indices des boucles

  int data_array[9];

  for (i = 1; i < height - 1; i++)
    for (j = 1; j < width - 1; j++) {
      k = 0;
      for (a = 0; a < 3; a++) {
        for (b = 0; b < 3; b++) {
          data_array[k++] = Data_out[(i - 1 + a) * step_gray + b + j - 1];
        }
      }
      // while (k<9) data_array[k++] = Data_out[i * step_gray + k + j];
      
      // Get Median value
      Data_Median_out[i * step_gray + j] = median_opti9(data_array);
    }
}

void median_filter_v1() {
  int a, b, k, i, j;  // indices des boucles

  int data_array[9];

  for (i = 1; i < height - 1; i++)
    for (j = 1; j < width - 1; j++) {
      k = 0;
      for (a = 0; a < 3; a++) {
        for (b = 0; b < 3; b++) {
          data_array[k] = Data_out[(i - 1 + a) * step_gray + b + j - 1];
          k++;
        }
      }
      // RUN SORT ALGO BY INSERT
      insertionSort(data_array);
      // REPLACE BY MEDEAN
      Data_Median_out[i * step_gray + j] = data_array[4];
    }
}
// Optimised V2
void filtre_sobel_v2() {
  int Gx_sum, Gy_sum, G;  // Sobel var
  int i, j, a, b;  // indices des boucles
  for (i = 1; i < height - 1; i++) {
    for (j = 1; j < width - 1; j++) {
      // Sobel Vertical Gy
      // L0
      Gy_sum += Data_Median_out[(i * step_gray) + j] * G_y[0][0];
      Gy_sum += Data_Median_out[(i * step_gray) + 1 + j] * G_y[0][1];
      Gy_sum += Data_Median_out[(i * step_gray) + 2 + j] * G_y[0][2];
      // L1
      Gy_sum += Data_Median_out[(i * step_gray + 2) + j] * G_y[2][0];
      Gy_sum += Data_Median_out[(i * step_gray + 2) + 1 + j] * G_y[2][1];
      Gy_sum += Data_Median_out[(i * step_gray + 2) + 2 + j] * G_y[2][2];

      // Sobel Horizental Gx
      Gx_sum += Data_Median_out[(i * step_gray) + 0 + j] * G_x[0][0];
      Gx_sum += Data_Median_out[(i * step_gray) + 2 + j] * G_x[0][2];

      Gx_sum += Data_Median_out[(i * step_gray + 1) + j] * G_x[1][0];
      Gx_sum += Data_Median_out[(i * step_gray + 1) + 2 + j] * G_x[1][2];

      Gx_sum += Data_Median_out[(i * step_gray + 2) + j] * G_x[2][0];
      Gx_sum += Data_Median_out[(i * step_gray + 2) + 2 + j] * G_x[2][2];

      G = sqrt(abs(Gx_sum) * abs(Gx_sum) + abs(Gy_sum) * abs(Gy_sum));

      // Seuils White/Black
      G = G > 128 ? 255 : 0;
      G = G < 0 ? 0 : G;

      Sobel_out[i * step_gray + j] = (uchar)G;

      Gx_sum = 0;
      Gy_sum = 0;
    }
  }
}
// Optimised V1
void filtre_sobel_v1() {
  int Gx_sum, Gy_sum, G;  // Sobel var
  int i, j, a, b;  // indices des boucles

  for (i = 1; i < height - 1; i++) {
    for (j = 1; j < width - 1; j++) {
      for (a = 0; a < 3; a++) {
        for (b = 0; b < 3; b++) {
          Gx_sum +=
              Data_Median_out[(i - 1 + a) * step_gray + b + j - 1] * G_x[a][b];

          Gy_sum +=
              Data_Median_out[(i - 1 + a) * step_gray + b + j - 1] * G_y[a][b];
        }
      }

      G = sqrt(abs(Gx_sum) * abs(Gx_sum) + abs(Gy_sum) * abs(Gy_sum));

      // Seuils White/Black
      G = G > 20 ? 255 : 0;
      G = G < 19 ? 0 : G;

      Sobel_out[i * step_gray + j] = (uchar)G;

      Gx_sum = 0;
      Gy_sum = 0;
    }
  }
}
char const* file_name(char* image_name) {
  sprintf(buffer, "img/V%d_%s.jpg", version, image_name);
  printf(" %s\t", buffer);
  return buffer;
}

int main(int argc, char* argv[]) {
  // Get video input from commande line
  char* vi = argv[1];
  char* ver = argv[2];
  int video_input = atoi(vi);

  version = atoi(ver);

  // intialisation de la variable ESC_keyboard
  char ESC_keyboard;
  // intialisation de la variable ESC_keyboard
  ESC_keyboard = cvWaitKey(5);

  // Ouvrir le flux vidéo
  // capture =
  // cvCreateFileCapture("/dev/v4l/by-path/platform-ci_hdrc.1-usb-0\:1.1\:1.0-video-index0");
  // capture = cvCreateCameraCapture( CV_CAP_ANY );
  capture = cvCreateCameraCapture(video_input);

  // Vérifier si l'ouverture du flux est ok
  if (!capture) {
    printf("Ouverture du flux vidéo impossible ( video_input = %d ) !\n",
           video_input);
    return 1;
  }

  // Définition des fenêtres
  cvNamedWindow("Image_IN_Window", CV_WINDOW_AUTOSIZE);    // Image_IN
  cvNamedWindow("Image_GRAY_Window", CV_WINDOW_AUTOSIZE);  // Image_GRAY
  cvNamedWindow("Image_SOBEL", CV_WINDOW_AUTOSIZE);        // Image_SOBEL
  cvNamedWindow("Image_MEDIAN", CV_WINDOW_AUTOSIZE);       // Image_SOBEL

  // Positionnement des fenêtres
  cvMoveWindow("Image_IN_Window", 0, 0);
  cvMoveWindow("Image_GRAY_Window", 0, 700);
  cvMoveWindow("Image_SOBEL", 1, 700);
  cvMoveWindow("Image_MEDIAN", 1, 700);

  // Première acquisition
  Image_IN = cvQueryFrame(capture);

  // Création de l'image de sortie
  Image_GRAY =
      cvCreateImage(cvSize(Image_IN->width, Image_IN->height), IPL_DEPTH_8U, 1);

  // Création de l'image de sortie pour Sobel
  Image_SOBEL =
      cvCreateImage(cvSize(Image_IN->width, Image_IN->height), IPL_DEPTH_8U, 1);
  // Création de l'image de sortie pour Sobel
  Image_MEDIAN =
      cvCreateImage(cvSize(Image_IN->width, Image_IN->height), IPL_DEPTH_8U, 1);

  step_gray = Image_GRAY->widthStep / sizeof(uchar);
  // Start TIMER
  clock_t begin = clock();
  clock_t end;
  // Boucle tant que l'utilisateur n'appuie pas sur la touche q (ou Q)
  while (frame_limit) {
    // On récupère une Image_IN
    Image_IN = cvQueryFrame(capture);
    // Dimension
    height = Image_IN->height;
    width = Image_IN->width;
    // distance entre les deux premiers pixels de lignes successives
    step = Image_IN->widthStep;
    channels = Image_IN->nChannels;
    // initialisation des pointeurs de donnée
    Data_in = (uchar*)Image_IN->imageData;
    Data_out = (uchar*)Image_GRAY->imageData;
    Sobel_out = (uchar*)Image_SOBEL->imageData;
    Data_Median_out = (uchar*)Image_MEDIAN->imageData;

    // conversion RGB en niveau de gris
    // grey_filter();
    // Median Filter
    // median_filter_v1();
    // median_filter_v2();
    // median_filter_v3();

    // Sobel Filter
    // filtre_sobel_v1();
    // filtre_sobel_v2();

  switch (version)
    {
    case 1:
        grey_filter();
        median_filter_v1();
        filtre_sobel_v1();
      break;
    case 2:
        grey_filter();
        median_filter_v2();
        filtre_sobel_v2();
      break;
    case 3:
        grey_filter();
        median_filter_v3();
        filtre_sobel_v2();
      break;
    default:
      break;
    }

    // On affiche l'Image_IN dans une fenêtre
    cvShowImage("Image_IN_Window", Image_IN);
    // On affiche l'Image_GRAY dans une deuxième fenêtre
    cvShowImage("Image_GRAY_Window", Image_GRAY);
    cvShowImage("Image_SOBEL", Image_SOBEL);
    cvShowImage("Image_MEDIAN", Image_MEDIAN);

    // On attend 5ms
    ESC_keyboard = cvWaitKey(5);
    frame_limit--;
    printf("frame : %d \n\r", frame_limit);

    if (frame_limit == 0) {
      // Get run time before image saving
      end = clock();
      // Saving a copy of images for comparison
      printf("Saving images ...\n\r");

      cvSaveImage(file_name("Image_IN"), Image_IN);
      cvSaveImage(file_name("Image_GRAY"), Image_GRAY);
      cvSaveImage(file_name("Image_MEDIAN"), Image_MEDIAN);
      cvSaveImage(file_name("Image_SOBEL"), Image_SOBEL);
    }
  }

  unsigned long millis = (end - begin) * 1000 / CLOCKS_PER_SEC;
  printf("\n\rv%d --> Finished in %ld ms\n", version, millis);

  // Fermeture de l'acquisition Vidéo
  cvReleaseCapture(&capture);

  // Fermeture des fenêtres d'affichage
  cvDestroyWindow("Image_IN_Window");
  cvDestroyWindow("Image_GRAY_Window");
  cvDestroyWindow("Image_SOBEL");
  cvDestroyWindow("Image_MEDIAN");

  return 0;
}

#undef MEDIAN_SORT
#undef MEDIAN_SWAP
