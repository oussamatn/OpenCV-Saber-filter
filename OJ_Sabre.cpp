//
// OJ_Sabre.cpp
//
// Compilation : 
// >> g++ `pkg-config opencv --cflags` OJ_Sabre.cpp -o OJ_Projet `pkg-config opencv --libs`
//
// pour trouver le /dev/video? de la WebCam faire :
// sudo rm /dev/video*
// brancher la WebCam
// >> debian@SabreLite:~/Desktop$ ls /dev/video*
// ==>/dev/video4
//
// adapter la ligne :
//     capture = cvCreateCameraCapture( 4 );
//

/*
 * Code written by Vinz (GeckoGeek.fr) 14/03/2010
 * http://www.geckogeek.fr/lire-le-flux-dune-webcam-camera-video-avec-opencv.html
 * 
 */
 
#include <stdio.h>
#include <stdlib.h>
// OpenCV header

#include "highgui.h"
#include "cv.h"

//Global variables : 
    // Images
     IplImage *Image_IN;
     IplImage *Image_OUT;
     IplImage *Image_SOBEL;
     IplImage *Image_MEDIAN;

    // Capture vidéo
    CvCapture *capture;
	
	// variables images
	int height,width,step,channels;		// Taille de l'image
	uchar *Data_in;						// pointeur des données Image_IN
	uchar *Data_out;					// pointeur des données Image_OUT
	uchar *Sobel_out;					// pointeur des données Sobel filter
    uchar *Data_Median_out;					// pointeur des données Sobel filter
	int step_gray;
    int i,j,k,a,b;							// indices
      int G_x[3][3] = {{-1, 0, 1}, //Sobel filter X
                      {-2, 0, 2},
                      {-1, 0, 1}};

     int G_y[3][3] = {{-1, -2, -1},
                      {0, 0, 0},
                      {1, 2, 1}};

//Functions prototype
void filtre_sobel();
void grey_filter();
void median_filter();

int main()
{ 
    // Touche clavier pour quitter
    int frame_limit=1000;
	// intialisation de la variable ESC_keyboard

    char ESC_keyboard;
    // intialisation de la variable ESC_keyboard
    ESC_keyboard = cvWaitKey(5);

    // Ouvrir le flux vidéo
   // capture = cvCreateFileCapture("/dev/v4l/by-path/platform-ci_hdrc.1-usb-0\:1.1\:1.0-video-index0");
    //capture = cvCreateCameraCapture( CV_CAP_ANY );
   capture = cvCreateCameraCapture( 0 );
    
    // Vérifier si l'ouverture du flux est ok
    if (!capture) {
 
       printf("Ouverture du flux vidéo impossible !\n");
       return 1;
 
    }
 
    // Définition des fenêtres
	cvNamedWindow("Image_IN_Window", CV_WINDOW_AUTOSIZE);   // Image_IN
	cvNamedWindow("Image_OUT_Window", CV_WINDOW_AUTOSIZE); 	// Image_OUT
	cvNamedWindow("Image_SOBEL", CV_WINDOW_AUTOSIZE); 	// Image_SOBEL
    cvNamedWindow("Image_MEDIAN", CV_WINDOW_AUTOSIZE); 	// Image_SOBEL

	// Positionnement des fenêtres
    cvMoveWindow("Image_IN_Window", 0,0);
    cvMoveWindow("Image_OUT_Window", 0,700);
    cvMoveWindow("Image_SOBEL", 1,700);
    cvMoveWindow("Image_MEDIAN", 1,700);

	// Première acquisition
   Image_IN = cvQueryFrame(capture); 
	
   // Création de l'image de sortie
   Image_OUT = cvCreateImage(cvSize(Image_IN->width,Image_IN->height),  IPL_DEPTH_8U, 1);

   // Création de l'image de sortie pour Sobel
   Image_SOBEL = cvCreateImage(cvSize(Image_IN->width,Image_IN->height),  IPL_DEPTH_8U, 1);
    // Création de l'image de sortie pour Sobel
   Image_MEDIAN = cvCreateImage(cvSize(Image_IN->width,Image_IN->height),  IPL_DEPTH_8U, 1);


    step_gray = Image_OUT->widthStep/sizeof(uchar);
   
    // Boucle tant que l'utilisateur n'appuie pas sur la touche q (ou Q)
    while(frame_limit) {
 
		// On récupère une Image_IN
		Image_IN = cvQueryFrame(capture);
		// Dimension
		height    = Image_IN->height;
		width     = Image_IN->width;
		// distance entre les deux premiers pixels de lignes successives
		step      = Image_IN->widthStep;
		channels  = Image_IN->nChannels;
		// initialisation des pointeurs de donnée
		Data_in      = (uchar *)Image_IN->imageData; 
		Data_out = (uchar *) Image_OUT->imageData;
		Sobel_out = (uchar *) Image_SOBEL->imageData;
        Data_Median_out = (uchar *) Image_MEDIAN->imageData;
	  
		//conversion RGB en niveau de gris
        grey_filter();
        //Median Filter
        median_filter();

        
        //Sobel Filter
        filtre_sobel();
            

        // On affiche l'Image_IN dans une fenêtre
        cvShowImage( "Image_IN_Window", Image_IN);
        // On affiche l'Image_OUT dans une deuxième fenêtre
        cvShowImage( "Image_OUT_Window", Image_OUT);
        cvShowImage( "Image_SOBEL", Image_SOBEL);
        cvShowImage( "Image_MEDIAN", Image_MEDIAN);




		// On attend 5ms
		ESC_keyboard = cvWaitKey(5);
		frame_limit--;
 
    }
 
    // Fermeture de l'acquisition Vidéo
	cvReleaseCapture(&capture);
   
	// Fermeture des fenêtres d'affichage
	cvDestroyWindow("Image_IN_Window");
    cvDestroyWindow("Image_OUT_Window");
    cvDestroyWindow("Image_SOBEL");
    cvDestroyWindow("Image_MEDIAN");

    return 0;
 
}
void grey_filter(){
    for(i=0;i<height;i++) 
			for(j=0;j<width;j++)

				Data_out[i*step_gray+j]=0.114*Data_in[i*step+j*channels+0]+ 0.587*Data_in[i*step+j*channels+1] + 0.299*Data_in[i*step+j*channels+2];

}

void insertionSort(int array[])
{
    int temp, i , j;
    for(i = 0; i < 9; i++){
        temp = array[i];
        for(j = i-1; j >= 0 && temp < array[j]; j--){
            array[j+1] = array[j];
        }
        array[j+1] = temp;
    }
}

void median_filter(){
    int a,b,k;

    int data_array[9];

    for(i=1;i<height-1;i++)
			for(j=1;j<width-1;j++)
				{ 

                    //for(k=0;k<9;k++) data_array[k]=Data_out[i*step_gray+j+k];
                    k=0;
                    for (a=0;a<3;a++){
                        for(b=0;b<3;b++)
                        {
                            data_array[k]= Data_out[(i-1)*step_gray+a + b+j-1] ;
                            k++;
                            //printf("k= %d \n\r",k);

                        }
                    }
                    // RUN SORT ALGO BY INSERT
                    insertionSort(data_array);
                    //REPLACE BY MEDEAN
                    Data_Median_out[i*step_gray+j]=data_array[4];

				}
}



//Optimised V1
//
void filtre_sobel(){
int Gx_sum,Gy_sum,G; //Sobel var
    for(i=1;i<height-1;i++){
                    for(j=1;j<width-1;j++){

                        for (a=0;a<3;a++){
                            for(b=0;b<3;b++)
                            {
                                //Gx_sum += Data_out[i*step_gray + j ] * G_x[a][b];
                                Gx_sum += Data_Median_out[(i-1)*step_gray+a + b+j-1] * G_x[a][b];


                                Gy_sum += Data_Median_out[(i-1)*step_gray+a+ b+j-1] * G_y[a][b];

                            }
                        }

                        G = sqrt( abs(Gx_sum)*abs(Gx_sum)  + abs(Gy_sum)*abs(Gy_sum));


                        //Seuils White/Black
                        G= G > 128 ? 255:0;
                        G= G < 0 ? 0 : G;
                        /*if(G>128) G=255;
                        else G=0;*/
                        Sobel_out[i*step_gray+j]=(uchar) G;

                        Gx_sum=0;
                        Gy_sum=0;

                    }
}
}