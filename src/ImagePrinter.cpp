#include "ImagePrinter.h"

ImagePrinter::ImagePrinter()
{
	//ctor
}

void ImagePrinter::printImageInConsole(Mat image) {

	cout << "Image rows: " << image.rows << " Image cols: " << image.cols << endl;

	// Print on Console
	int rows = image.rows;
	int cols = image.cols;

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j)
		{
			int pixelValue = (int)image.at<uchar>(i, j);
			if (pixelValue==0)
				cout << ".";
			else
				cout << pixelValue << ",";
		}
		cout << "\n";
	}
	cout << "\n*************************************************************************\n" << endl;

}

void ImagePrinter::printImageInWindow(Mat inputImage, string windowName){
	Mat outputImage;
	resize(inputImage, outputImage, Size(0, 0), 3, 3, INTER_LINEAR);
	namedWindow(windowName, CV_WINDOW_AUTOSIZE);
	imshow(windowName, outputImage);
	waitKey(0);
 }
void ImagePrinter::printImageInWindowWithStretching(Mat inputImage, string windowName){
    Mat img = inputImage.clone();
    img = HistogramStretching::linearStretch(img);
    resize(img, img, Size(0, 0), 3, 3, INTER_LINEAR);
	namedWindow(windowName, CV_WINDOW_AUTOSIZE);
	imshow(windowName, img);
	waitKey(0);
}

void ImagePrinter::printImageBlobs(int rows,int cols, vector<Blob*> blobs, string windowName){
    Mat temp3ChannelImage(rows,cols, CV_8UC3, Scalar(0,0,0));
    for(vector<Blob*>::iterator i = blobs.begin(); i != blobs.end(); i++){
        printImageBlob(temp3ChannelImage,*i,windowName);
    }
    waitKey(0);
}
void ImagePrinter::printImageBlob(Mat inputImage, Blob* b, string windowName) {
   	Mat outputImage;

    ///DRAW CONTOUR
    vector<Point> contour = b->getContour();
    Vec3b color( rand()&255, rand()&255, rand()&255 );
    for(vector<Point>::iterator i = contour.begin(); i != contour.end(); i++){
        Point p = *i;
        inputImage.at<Vec3b>(p.y,p.x) = color;
    }

    ///DRAW CENTROID
    Point centroid = b->getCentroid();
    inputImage.at<Vec3b>(centroid.y, centroid.x) = Vec3b(0,0,255);

    namedWindow( windowName, CV_WINDOW_AUTOSIZE );
    resize(inputImage, outputImage, Size(0, 0), 3, 3, INTER_LINEAR);
    imshow( windowName, outputImage );

}
