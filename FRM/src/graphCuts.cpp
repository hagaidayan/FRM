#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <iostream>
#include <fstream>

#include "maxflow/graph.h"

#include "image.h"

using namespace std;
using namespace cv;
int WID;
int HEI;
int SIZ;
int C;
int ALPHA;
inline float gFunc_laplace(float a, float b);
inline float gFunc(Vec3b a);
inline float gFunc1(float a);
void classification(Image<float> I);

inline int getNodeNum(int img, int i, int j) {
	return ((img - 1) * WID * HEI) + (i + (j * WID));
}

inline float gFunc_laplace(float a, float b) {
	return 600 / (1 + (exp(b*a)));
}

void FRM(Image<float> src[],int n, float b){
	int wid = src[0].width();
	int hei = src[0].height();
	WID = wid;
	HEI = hei;
	SIZ = n;
	Graph<int, int, int> g(n*hei*wid, (3 * n * wid * hei));
	g.add_node(n*hei*wid);
	C = 1;
	int kernel_size = 3;
	int scale = 1;
	int delta = 0;
	int c;
	int ddepth = CV_16S;
	Image<uchar>* src_gray = new Image<uchar>[n];
	Image<uchar>* src_laplace = new Image<uchar>[n];
	Image<float>* src_preAbs = new Image<float>[n];
	Scalar mean;
	Scalar* stddev = new Scalar[n];
	Image<Vec3b>dst(src[0].width(), src[0].height());


	for (int i = 0; i < n; i++) { //create set of laplacians
		cvtColor(src[i], src_gray[i], CV_BGR2GRAY);
		Laplacian(src_gray[i], src_preAbs[i], ddepth, kernel_size, scale, delta, BORDER_DEFAULT);
		meanStdDev(src_preAbs[i], mean, stddev[i], Mat());
		convertScaleAbs(src_preAbs[i], src_laplace[i]);
		GaussianBlur(src_laplace[i], src_laplace[i], Size(9, 9), 0, 0, BORDER_DEFAULT);
	}
	for (int i = 0; i < wid; i++) { 
		for (int j = 0; j < hei; j++) { //add edges to s,t vertices in the diercted graph
			g.add_tweights(getNodeNum(1, i, j), 100000, 0);
			g.add_tweights(getNodeNum(n, i, j), 0, float(gFunc_laplace(src_laplace[n - 1].at<uchar>(j, i), b)));
		}
	}
	for (int k = 0; k < SIZ; k++) {
		for (int i = 0; i < WID; i++) {
			for (int j = 0; j < HEI; j++) {
				if (i != WID - 1) {
					g.add_edge(getNodeNum(k + 1, i, j), getNodeNum(k + 1, i + 1, j), C, C); // right neighbor
				}
				if(j != HEI - 1) {
					g.add_edge(getNodeNum(k + 1, i, j), getNodeNum(k + 1, i, j + 1), C, C); // down neighbor
				}
				if (k != SIZ - 1) {
					g.add_edge(getNodeNum(k + 1, i, j), getNodeNum(k + 2, i, j), float(gFunc_laplace(src_laplace[k].at<uchar>(j, i), b)), float(gFunc_laplace(src_laplace[k].at<uchar>(j, i), b))); // lower image neighbor
				}
			}
		}
	}

	int flow = g.maxflow();
	std::cout << "Flow = " << flow << endl;
	try {
		for (int k = 0; k < SIZ; k++) {
			for (int i = 0; i < WID; i++) {
				for (int j = 0; j < HEI; j++) {
					if ((k == SIZ - 1) && g.what_segment(getNodeNum(k + 1, i, j)) == Graph<int, int, int>::SOURCE) //if pixel in last image is chosen
					{
						dst(i, j) = src[k].at<Vec3b>(j, i);
					}
					else if ((k != SIZ - 1) && g.what_segment(getNodeNum(k + 1, i, j)) != g.what_segment(getNodeNum(k + 2, i, j)))
						dst(i, j) = src[k].at<Vec3b>(j, i);
				}
			}
		}
	}
	catch (exception e) {
		std::cout << "unable to generate graph" << endl;
	}
	imshow("FRM", dst);
	imwrite("../outputFRMfly.jpg", dst);
}


int main() {
	//testGCuts();

	//Image<float> I= Image<Vec3b>(imread("../fishes.jpg"));
	//Image<float> I2 = Image<Vec3b>(imread("../fishes.jpg"));
	//Image<float> I = Image<Vec3b>(imread("../sheep1.jpg"));
	//Image<float> I2 = Image<Vec3b>(imread("../sheep1.jpg"));
	//Image<float> I3 = Image<Vec3b>(imread("../sheep2.jpg"));
	//Image<float> I4 = Image<Vec3b>(imread("../sheep2.jpg"));

	//Image<float> I = Image<Vec3b>(imread("../fly11.jpg"));
	//Image<float> I2 = Image<Vec3b>(imread("../fly11.jpg"));
	//Image<float> I3 = Image<Vec3b>(imread("../fly22.jpg"));
	//Image<float> I4 = Image<Vec3b>(imread("../fly22.jpg"));

	Image<float> I = Image<Vec3b>(imread("../IMG_2657reduced.jpg"));
	Image<float> I2 = Image<Vec3b>(imread("../IMG_2656reduced.jpg"));
	Image<float> I3 = Image<Vec3b>(imread("../IMG_2655reduced.jpg"));
	Image<float> I4 = Image<Vec3b>(imread("../IMG_2658reduced.jpg"));

	Image<float> imArr[4];
	imArr[0] = I;
	imArr[1] = I2;
	imArr[2] = I3;
	imArr[3] = I4;
	//imshow("I",I);
	FRM(imArr, 4, 0.2);
	waitKey(0);
	return 0;
}
