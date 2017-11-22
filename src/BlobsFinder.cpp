/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
*/


#include "BlobsFinder.h"

BlobsFinder::BlobsFinder()
{
    //ctor
}

void BlobsFinder::printImageInConsole(Mat& image, string type){
	cout << "\n\n" << endl;
	for(int i = 0; i < image.rows; i++){
		for(int j =0; j < image.cols; j++){
			if(type == "8U")
				cout << ceil((unsigned short int) image.at<uchar>(i,j));
			else if(type == "32F")
				cout << fixed << image.at<float>(i,j)<<" ";
		}
		cout << "\n\n";
	}
}

void BlobsFinder::printImage(Mat& image, string windowName, string type){

	Mat toShow;
	if( type == "8U" ){
		toShow = image.clone();
		toShow = toShow*200;

	}
	else if ( type == "32F" ){
		
		/*need conversion from 32f to 8u*/
		double minVal, maxVal;
		minMaxLoc(image, &minVal, &maxVal); //find minimum and maximum intensities
		image.convertTo(toShow, CV_8U, 255.0/(maxVal - minVal), -minVal * 255.0/(maxVal - minVal));
		

	}

	resize(toShow, toShow, Size(), 4, 4, INTER_LINEAR);
	namedWindow(windowName, CV_WINDOW_AUTOSIZE);
	imshow(windowName, toShow);
	//waitKey();
	
}

vector<Blob*> BlobsFinder::findBlobs(double PSF, string filePath, int** data, int rows, int cols, double CDELT1, double CDELT2) {	
	
	vector<Blob*> blobs;
	/*
		Algorithm: 
		
		input: int ** data
		output: vector<Blob*> blobs
 
			data -> 32FC1 workingImage32F  [for smoothing]			

			32FC1 workingImage32F -> 8UC1 workingImage8U [for findContours]

			8UC1 workingImage8U -> unsigned short int ** blobsImage  [for Blob's constructor]

	*/



	/*CONVERSION int ** TO 8U Mat*/

	Mat photonsMap(rows, cols, CV_8UC1, Scalar(0));

	for(int y = 0; y < rows; y++){
		for(int x =0; x < cols; x++){
			photonsMap.at<uchar>(y,x) = (uchar) data[y][x];
 
		}
	}
	//BlobsFinder::printImage(photonsMap, "Photons Image", "8U");





	/*SMOOTHING*/

	/* For Smoothing operation we use a float pixels matrix. */
	Mat workingImage32F(rows, cols, CV_32FC1, Scalar(0));
	for(int y = 0; y < rows; y++){
		for(int x =0; x < cols; x++){
			workingImage32F.at<float>(y,x) = (float) data[y][x];
 
		}
	}
	

	/* Gaussian smoothing */ 
	int kernelSize = (2 * PSF/CDELT2) + 1; 
	//cout << "Kernel size: " << kernelSize << endl;
	BlobsFinder::gaussianBlur(&workingImage32F, Size(kernelSize, kernelSize), PSF); // 17x17   2.5    23x23   3	21x21 4
	BlobsFinder::printImage(workingImage32F, "gaussianBlur32F", "32F");
	//BlobsFinder::printImageInConsole(workingImage32F, "32F");
 
  
	/* Convert back to 8UC1 with linear stretching */
	Mat workingImage8U; 
	double minVal, maxVal;
	minMaxLoc(workingImage32F, &minVal, &maxVal); //find minimum and maximum intensities
	workingImage32F.convertTo(workingImage8U, CV_8U, 255.0/(maxVal - minVal), -minVal * 255.0/(maxVal - minVal));




	


	/* Computing Image Histogram */
	int nimages = 1;
  	int histSize = 256;
	float range[] = { 0, 256 };
 	const float* histRange = { range };
  	bool uniform = true; bool accumulate = false;
	Mat hist;

  	calcHist(&workingImage8U, nimages, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate );

	//BlobsFinder::drawImageHistogram(hist,histSize);
	/*......................*/


	/* Thresholding -> mean value */

	BlobsFinder::printImageInConsole(workingImage8U, "8U");

	int mean = 0;
	for( int i = 1; i < histSize; i++ ){
		 mean += hist.at<float>(i-1); 
	}
	 
	mean = mean/histSize-1;
 
	for(int y = 0; y < rows; y++){
		for(int x =0; x < cols; x++){
			unsigned short int gl = (unsigned short int) workingImage8U.at<uchar>(y,x);			
			if(gl <= mean)
				workingImage8U.at<uchar>(y,x) = 0;
 
		}
	}
	cout << "\n\n" << endl;
	BlobsFinder::printImageInConsole(workingImage8U, "8U");
	BlobsFinder::printImage(workingImage8U, "workingImage8U Thresholded", "8U");
	/* ------------------  */


 	/* Convert 8UC1 to unsigned short int ** for the Blob's constuctor */
	unsigned short int ** blobsImage;
	blobsImage = new unsigned short int *[rows];
  	
	for(int y = 0; y < rows; y++){
		blobsImage[y] = new unsigned short int[cols];
		for(int x =0; x < cols; x++){
			blobsImage[y][x] = workingImage32F.at<float>(y,x);
		}
	}
	
	/* For printing contours and centroid */	
	Mat workingImage8U3C(rows, cols, CV_8UC3, Scalar(0,0,0));


	/* FIND THE CONTOURS OF EACH BLOB */

    	vector<vector<Point> > contoursImage8UWithPadding;
    	vector<Vec4i> hierarchy;



	// Adding 1x1 padding (the function does not take into account 1-pixel border of the image, so I dont want a photon pixel border to be clipped)
	Mat workingImage8UWithPadding(rows+2, cols+2, CV_8UC1, Scalar(0));
	for(int y = 0; y < rows; y++){
		for(int x = 0; x < cols; x++){
			workingImage8UWithPadding.at<uchar>(y+1,x+1) = workingImage8U.at<uchar>(y,x);
		}
	}
	
     	findContours(workingImage8UWithPadding, contoursImage8UWithPadding, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);

	
	



	/* BLOBS CREATION */

    	int indexx=0;
    	for(vector<vector<Point> >::iterator i = contoursImage8UWithPadding.begin(); i < contoursImage8UWithPadding.end(); i++){





	        vector<Point> currentContuorWithPadding = *i;

        	// Creating a blob if and only if it is not contained i another blob
        	if(hierarchy[indexx][3]==-1){

			

			

				
				
			vector<pair<CustomPoint,int>> pixelsOfBlobsNoPadding;
			vector<CustomPoint> photonsInBlobs;

			/*
				Compute blob's pixels
			*/
			bool computePixelsAndPhotonsOfBlob = BlobsFinder::computePixelsAndPhotonsOfBlob(  	currentContuorWithPadding, 
												 		workingImage8UWithPadding, 
												 		data, 
												 		pixelsOfBlobsNoPadding,
												 		photonsInBlobs);
 
	

			/* 
				Remove padding from contour & Convert vector<Point> to vector<CustomPoint> 
			*/
			vector<CustomPoint> currentCustomContuorNoPadding;
			for(vector<Point> ::iterator ii = currentContuorWithPadding.begin(); ii < currentContuorWithPadding.end(); ii++){
				Point p = *ii;
				CustomPoint cp(p.y-1,p.x-1);
				currentCustomContuorNoPadding.push_back(cp);
			}


			/*
				Handling errors	
			*/
			if(!computePixelsAndPhotonsOfBlob)
				BlobsFinder::reportError(photonsInBlobs,pixelsOfBlobsNoPadding,currentCustomContuorNoPadding,filePath,data);
			 
			/*
				Create Blob
			*/
			if(photonsInBlobs.size() > 1) {
			
				Blob* b = new Blob(filePath, currentCustomContuorNoPadding, pixelsOfBlobsNoPadding, photonsInBlobs, CDELT1, CDELT2);
				
				/*
					Push Blob in list
				*/
				blobs.push_back(b);
			
				/* Debug */
				cout << "\nFile: "<<b->getFilePath()<<"  "<<indexx+1<<" su "<<contoursImage8UWithPadding.size()<<endl;			
				cout << "[Blob "<<b->getCentroid().y<<","<<b->getCentroid().x<<"]" << endl;
				cout << "Number of pixels: " << b->getNumberOfPixels() << endl;
				cout << "Number of contour pixels: " << b->getContour().size() << endl;
				cout << "Photon's closeness: " << b->getPhotonsCloseness() << endl;
				cout << "Photon's: " << b->getNumberOfPhotonsInBlob() << endl;
				vector<CustomPoint> photons = b->getPhotonsInBlob();
				for(vector<CustomPoint>::iterator i = photons.begin(); i != photons.end(); i++){
					CustomPoint p = *i;
					cout << "*(" << p.y << "," << p.x <<")  ";
				}
				Vec3b color( rand()&255, rand()&255, rand()&255 );
 
				for(vector<CustomPoint>::iterator i = currentCustomContuorNoPadding.begin(); i != currentCustomContuorNoPadding.end(); i++){
					CustomPoint p = *i;
					workingImage8U3C.at<Vec3b>(p.y,p.x) = color;
				}
				///DRAW CENTROID
				CustomPoint centroid = b->getCentroid();
				workingImage8U3C.at<Vec3b>(centroid.y,centroid.x) = Vec3b(0,0,255);


			}


			 
			
			

 
		}
        	indexx++;
    	}
 
	/*
		Print contours	
	*/
	BlobsFinder::printImage(workingImage8U3C, "Contours and centroids", "8U");
	
	/*
		Wait key for images
	*/
	waitKey();

	/*
		!!!!FREE MEMORY!!!!
	
	

 	for (int i = 0; i < rows; i++)
	{
		delete [] editedImage[i];
	}
	delete [] editedImage;
	editedImage = 0;
*/
	
	//cout << "Number of blobs: "<< blobs.size() << endl;
	 
    	return blobs;
}

/**********************************

	computePixelsOfBlob

**********************************/
bool BlobsFinder::computePixelsAndPhotonsOfBlob(vector<Point>& contour, 
						Mat& image,
						int ** data, 
						vector<pair<CustomPoint,int>>& pixelsOfBlobs, 
						vector<CustomPoint>& photonsOfBlobs
){

 

	for(int y = 0; y < image.rows; y++){
		for(int x = 0; x < image.cols; x++){

			Point p(x,y);
			double isInside = pointPolygonTest(contour,p,false); /*opencv*/

			/* check if pixel is inside blob */
			if( isInside >= 0 ){

				/* Removing padding */
				CustomPoint cp(p.y - 1, p.x - 1);
				pixelsOfBlobs.push_back(make_pair(cp,(int)image.at<uchar>(y,x)));
				
				/* check if pixel is a photon */
				int greyLevel = data[cp.y][cp.x];
				for(int j = 0; j < greyLevel; j++){
					photonsOfBlobs.push_back(cp);
				}
				
			}
		}	
	}
	if(photonsOfBlobs.size()==0)
		return false;
	else 
		return true;
 
}


void BlobsFinder::reportError(vector<CustomPoint>& photonsOfBlobs, vector<pair<CustomPoint,int>>& pixelsOfBlobs, vector<CustomPoint>& contour, string filePath, int ** data){
	/** DEBUG **/
	if(photonsOfBlobs.size()==0){
		// debug
		cout << "*BlobsFinder Error: 0 fotoni nel blob! File: "<< filePath << endl;
		cout << "*Number of pixels: " << pixelsOfBlobs.size() << endl;
		cout << "*Number of contour pixels: " << contour.size() << endl;

		for(vector<pair<CustomPoint,int>>::iterator i = pixelsOfBlobs.begin(); i != pixelsOfBlobs.end(); i++){
			pair<CustomPoint,int> p = *i;
			cout << "*(" << p.first.y << "," << p.first.x <<")"<<" gl: "<<p.second<<" photon:" <<data[p.first.y][p.first.x]<<endl;
		}
		cout <<"\n*Contorno:" << endl;
		for(vector<CustomPoint>::iterator i = contour.begin(); i != contour.end(); i++){
			CustomPoint p = *i;
			cout << "*(" << p.y << "," << p.x <<")"<<" photon:" <<data[p.y][p.x]<<endl;
		}

		exit(EXIT_FAILURE);
	}	  
}





/***********************************

	STRETCHING

***********************************/
void BlobsFinder::nonLinearStretch(Mat* image, double r)
{
   // Mat image = inputImage.clone();
	int rows = image->rows;
	int cols = image->cols;
	for (int i = 0; i < rows; ++i){
		for (int j = 0; j < cols; ++j){
			//unsigned int pixelValue = (unsigned int)image->at<uchar>(i, j);
			//image->at<uchar>(i, j) =  (uchar) pow(255, 1 - r)* pow(pixelValue,r);
			double pixelValue = image->at<double>(i, j);
			image->at<double>(i, j) = pow(255, 1 - r)* pow(pixelValue,r);
		}
	}
   // return image;

}









/**********************************

	SMOOTHING

**********************************/
void BlobsFinder::gaussianBlur(Mat* image, Size kernelSize, double sigma) {
	Mat outputImg;
	GaussianBlur(*image, outputImg, kernelSize, sigma, 0, 0);
 
	*image = outputImg;
	//return outputImg;
}



/**********************************

	DRAW HISTOGRAM

***********************************/


void BlobsFinder::drawImageHistogram(Mat& hist, int histSize){

	int hist_w = 512; 
	int hist_h = 400;
	int bin_w = cvRound( (double) hist_w/histSize );

	Mat histImage( hist_h, hist_w, CV_8UC1, Scalar(0) );

	/// Normalize the result to [ 0, histImage.rows ]
	//normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

 
	for( int i = 1; i < histSize; i++ ){
		line( histImage, 
		Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) ,
		Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
		Scalar( 255, 0, 0), 2, 8, 0  );
	}

	/// Display
	namedWindow("Image Histogram", CV_WINDOW_AUTOSIZE );
	imshow("Image Histogram", histImage );

}







/**********************************

	PRINT FUNCTIONS

**********************************/

void BlobsFinder::printImageBlobs(int rows,int cols, vector<Blob>& blobs, string windowName){
	Mat temp3ChannelImage(rows,cols, CV_32FC3, Scalar(0,0,0));
	for(vector<Blob>::iterator i = blobs.begin(); i != blobs.end(); i++){
		printImageBlob(temp3ChannelImage,*i,windowName);
	}
	waitKey(0);
}
void BlobsFinder::printImageBlob(Mat& inputImage, Blob& b, string windowName) {

	Mat tempInputImage = inputImage.clone();
	///DRAW CONTOUR
	vector<CustomPoint> contour = b.getContour();
	Vec3b color( rand()&255, rand()&255, rand()&255 );
	for(vector<CustomPoint>::iterator i = contour.begin(); i != contour.end(); i++){
		CustomPoint p = *i;
		tempInputImage.at<Vec3b>(p.y,p.x) = color;
	}

	///DRAW CENTROID
	CustomPoint centroid = b.getCentroid();
	tempInputImage.at<Vec3b>(centroid.y, centroid.x) = Vec3b(0,0,255);

	namedWindow( windowName, CV_WINDOW_AUTOSIZE );
	resize(tempInputImage, tempInputImage, Size(0, 0), 3, 3, INTER_LINEAR);
	imshow( windowName, tempInputImage );

}



/*
void BlobsFinder::printImageInConsole(int ** image, int rows, int cols){
	for(int i = 0; i < rows; i++){
		for(int j =0; j < cols; j++){
			cout << image[i,j] << " ";
		}
		cout << "\n";
	}
}
*/

/*TEST DEI TONI DI GRIGIO*/
/*
Mat test(30, 255, CV_8UC1, Scalar(0));
unsigned short int greylevel = 0;
for(int y = 0; y < 30; y++){
	greylevel = 0;
	for(int x = 0; x < 255; x++){
		test.at<uchar>(y,x) = greylevel;
		greylevel++;
	}
}
BlobsFinder::printImage(test, "TEST DEI TONI DI GRIGIO", "8U");
*/
/*------------------------*/


/*SHOWING KERNEL*/
/*
double _PSF = 7.5;
int kernelSize = ( ( 2  * _PSF) / CDELT2) + 1;
Mat float_kernel =  getGaussianKernel(kernelSize, _PSF, CV_32F ); 


cout << "kernelSize: " << kernelSize << endl;
cout << "float_kernel" << endl;
for (int i = 0; i < kernelSize; ++i) {
	for (int j = 0; j < 1; ++j)
	{
		cout << float_kernel.at<float>(i, j);

	}
	cout << "\n";
}
/*--------------*/
 



/*
Unsigned 8bits uchar 0~255
IplImage: IPL_DEPTH_8U
Mat: CV_8UC1, CV_8UC2, CV_8UC3, CV_8UC4

Signed 8bits char -128~127
IplImage: IPL_DEPTH_8S
Mat: CV_8SC1，CV_8SC2，CV_8SC3，CV_8SC4

Unsigned 16bits ushort 0~65535
IplImage: IPL_DEPTH_16U
Mat: CV_16UC1，CV_16UC2，CV_16UC3，CV_16UC4

Signed 16bits short -32768~32767
IplImage: IPL_DEPTH_16S
Mat: CV_16SC1，CV_16SC2，CV_16SC3，CV_16SC4

Signed 32bits int -2147483648~2147483647
IplImage: IPL_DEPTH_32S
Mat: CV_32SC1，CV_32SC2，CV_32SC3，CV_32SC4

Float 32bits float -1.18*10-38~3.40*10-38 
IplImage: IPL_DEPTH_32F
Mat: CV_32FC1，CV_32FC2，CV_32FC3，CV_32FC4

Double 64bits double 
Mat: CV_64FC1，CV_64FC2，CV_64FC3，CV_64FC4

Unsigned 1bit bool
IplImage: IPL_DEPTH_1U


*/
