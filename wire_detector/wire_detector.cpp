#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "cmath"

void approximateLine(std::vector<cv::Vec3f> lines, const float rho_max, const float theta_max, std::vector<cv::Vec3f>& mainLines)
{
	std::vector<std::vector<int>> approximateIndex(lines.size());

	for (int i = 0; i < lines.size(); i++)
	{
		int index = i;
		for (int j = i; j < lines.size(); j++)
		{
			float distance_i = lines[i][0];
			float distance_j = lines[j][0];
			float incline_i = lines[i][1];
			float incline_j = lines[j][1];

			float delta_distance = abs(distance_i - distance_j);
			float delta_incline = abs(incline_i - incline_j);

			if (delta_incline < theta_max && delta_distance < rho_max)
			{
				bool isApproximated = false;
				for (int k = 0; k < i; k++)
				{
					for (int l = 0; l < approximateIndex[k].size(); l++)
					{
						if (approximateIndex[k][l] == j)
						{
							isApproximated = true;
							break;
						}
						if (approximateIndex[k][l] == i)
							index = k;
					}
					if (isApproximated)
						break;
				}
				if (!isApproximated)
					approximateIndex[index].push_back(j);
			}
		}
	}

	for (int i = 0; i < approximateIndex.size(); i++)
	{
		if (approximateIndex[i].size() == 0)
			continue;
		cv::Vec3f line_t(0, 0);
		for (int j = 0; j < approximateIndex[i].size(); j++)
		{
			line_t[0] += lines[approximateIndex[i][j]][0];
			line_t[1] += lines[approximateIndex[i][j]][1];
		}
		line_t[0] /= approximateIndex[i].size();
		line_t[1] /= approximateIndex[i].size();
		mainLines.push_back(line_t);
	}
}

int main()
{
	int imageNumber;
	std::cin >> imageNumber;

	cv::Mat image = cv::imread(std::to_string(imageNumber) + ".jpg");
	resize(image, image, cv::Size(1200, 800), cv::INTER_LINEAR);

	cv::Mat image_copy = image.clone();
	cvtColor(image, image, cv::COLOR_BGR2GRAY);

	imshow(std::to_string(imageNumber) + ".jpg", image);

	GaussianBlur(image, image, {9, 9}, 3, 3);

	Canny(image, image, 150, 170);
	
	std::vector<cv::Vec3f> lines;
	std::vector<cv::Vec3f> lines_t;

	HoughLines(image, lines_t, 1, CV_PI / 180, 180, 0, 0); 

	float rho_max = 9;
	float theta_max = 3 * (CV_PI / 180);

	approximateLine(lines_t, rho_max, theta_max, lines);

	for (size_t i = 0; i < lines.size(); i++) 
	{
		float rho = lines[i][0];
		float theta = lines[i][1];

		if (sin(theta) <= sqrt(2) / 2 && sin(theta) >= -sqrt(2) / 2)
		{
			cv::Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a * rho, y0 = b * rho;
			pt1.x = cvRound(x0 + 800 * (-b));
			pt1.y = cvRound(y0 + 800 * (a));
			pt2.x = cvRound(x0 - 800 * (-b));
			pt2.y = cvRound(y0 - 800 * (a));
			line(image_copy, pt1, pt2, cv::Scalar(255, 0, 255), 2);
		}
	}
	imshow("Dedicated wires", image_copy);

	cv::waitKey(0);
	return 0;
}
