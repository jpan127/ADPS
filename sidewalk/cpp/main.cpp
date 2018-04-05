#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat convert_to_gray(Mat image)
{
	Mat gray;
	cvtColor(image, gray, COLOR_BGR2GRAY);
	return gray;
}

Mat convert_to_blur(Mat image, Size size=Size(9,9))
{
	Mat blur;
	GaussianBlur(image, blur, size, 0);
	return blur;
}

Mat convert_to_edges(Mat gray, double t1, double t2)
{
	Mat edges;
	Canny(gray, edges, t1, t2);
	return edges;
}

/*
	@param rho    : Resolution of parameter rho in pixels
	@param theta  : Resolution of parameter theta in pixels
	@param thresh : Minimum number of intersections to detect a line
*/
Mat convert_to_hough_lines(Mat edges, double rho, double theta, int thresh)
{
	// Empty matrix
	Mat image_lines = Mat::zeros(edges.size(), edges.type());
	// Find all lines
	vector <Vec4i> lines;
	HoughLinesP(edges, lines, 1, CV_PI/180, 50, 50, 10);
	// For each line found draw it
	for (size_t l = 0; l < lines.size(); l++)
	{
		Vec4i one_line = lines[l];
		// Draw line
		line(image_lines,						// Output matrix
			Point(one_line[0], one_line[1]),	// Point a
			Point(one_line[2], one_line[3]),	// Point b
			Scalar(255, 0, 0),					// Line color
			3,									// Thickness of line
			CV_AA);								// Type of line
	}
	cout << "Found " << lines.size() << " lines" << endl;
	return image_lines;
}

// Globals for blur_tile_width_trackbar_callback
int tile_x;
int tile_y;
void blur_tile_width_trackbar_callback(int pos, void* userdata)
{
	tile_x = (pos % 2 == 0) ? tile_x + 1 : tile_x;
	tile_y = (pos % 2 == 0) ? tile_y + 1 : tile_y;
}

// F7 to build, CTRL + F5 to run
int main( int argc, char** argv )
{
	// https://www.pyimagesearch.com/2015/04/06/zero-parameter-automatic-canny-edge-detection-with-python-and-opencv/
	// (1) Smooth image with Gaussian filter to remove high frequency noise
	// (2) Compute gradient intesnity representations of image
	// (3) Apply non-maximum suppression to remove false responses to edge detection
	// (4) Apply thresholding using lower/upper boundaries on gradient values
	// (5) Track edges using hysteresis by surpressing weak edges not connected to strong edges

	// Configurable blur tile size
	namedWindow("Blur");
	const int max_width = 255;
	createTrackbar("TILE X WIDTH", "Blur", &tile_x, max_width, blur_tile_width_trackbar_callback);
	createTrackbar("TILE Y WIDTH", "Blur", &tile_y, max_width, blur_tile_width_trackbar_callback);
	int blur_x = 3;
	int blur_y = 3;

	// Configurable color mask
	namedWindow("Color Mask");
	int lower_rgb[3] = { 0, 0, 0 };
	int upper_rgb[3] = { 0, 0, 0 };
	createTrackbar("Lower R", "Color Mask", &lower_rgb[0], 255);
	createTrackbar("Lower G", "Color Mask", &lower_rgb[1], 255);
	createTrackbar("Lower B", "Color Mask", &lower_rgb[2], 255);
	createTrackbar("Upper R", "Color Mask", &upper_rgb[0], 255);
	createTrackbar("Upper G", "Color Mask", &upper_rgb[1], 255);
	createTrackbar("Upper B", "Color Mask", &upper_rgb[2], 255);

	while (1)
	{
		// Make sure non-zero
		blur_x = (tile_x == 0) ? 1 : tile_x;
		blur_y = (tile_y == 0) ? 1 : tile_y;
		// Make sure odd numbered
		blur_x = (tile_x % 2 == 0) ? tile_x + 1 : tile_x;
		blur_y = (tile_y % 2 == 0) ? tile_y + 1 : tile_y;

		///////////////////////////////////////////////////////////////////////////////////////

		// Original
		Mat image = imread("Sidewalk.jpg", IMREAD_COLOR);

		// Color mask optimal seems to be:
		// Lower(190, 0, 0)
		// Upper(255, 255, 255)
		Mat mask;
		Scalar lower_bound = Scalar(lower_rgb[0], lower_rgb[1], lower_rgb[2]);
		Scalar upper_bound = Scalar(upper_rgb[0], upper_rgb[1], upper_rgb[2]);
		inRange(image, lower_bound, upper_bound, mask);

		// Unused right now
		Mat gray  = convert_to_gray(image);

		// Gaussian Blur
		Mat blur  = convert_to_blur(mask, Size(blur_x, blur_y));

		// Canny Edge
		Mat edges = convert_to_edges(blur, 50, 200);

		// Hough Transform
		// Only works in release crashes in debug, need to check on that
		Mat lines = convert_to_hough_lines(edges, 1, CV_PI/180, 50);
		
		// Show images
		imshow( "Original",		image	);
		imshow( "Gray",			gray	);
		imshow( "Blurred",		blur	);
		imshow( "Edges",		edges	);
		imshow( "Hough Lines",	lines	);
		imshow( "Color Mask",	mask	);

		// Quit if q
		if (waitKey(10) == 'q') break;
	}

	return 0;
}