#include "VanishingPoint.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>
#include <glm/gtx/string_cast.hpp>

int main() {
	boost::filesystem::path dir("data");
	for (auto it = boost::filesystem::directory_iterator(dir); it != boost::filesystem::directory_iterator(); ++it) {
		if (it->path().filename().extension().string() == ".txt") {
			std::cout << it->path().filename().string() << std::endl;

			int id;
			sscanf(it->path().filename().string().c_str(), "lines_%d.txt", &id);

			// read image
			std::string path = std::string("data/") + std::to_string(id) + ".jpg";
			cv::Mat img = cv::imread(path.c_str());

			// read text file
			glm::vec2 center_point;
			std::vector<vp::VanishingLine> lines;
			std::ifstream in(std::string("data/") + it->path().filename().string());
			if (!in.is_open()) continue;
			while (!in.eof()) {
				std::string line;
				std::getline(in, line);

				std::vector<std::string> strs;
				boost::split(strs, line, boost::is_any_of("\t "));

				if (strs.size() == 2) {
					center_point = glm::vec2(std::stof(strs[0]), std::stof(strs[1]));
				}
				else if (strs.size() == 5) {
					glm::dvec2 start(std::stof(strs[0]), std::stof(strs[1]));
					glm::dvec2 end(std::stof(strs[2]), std::stof(strs[3]));
					if (start != end) {
						lines.push_back(vp::VanishingLine(start.x, start.y, end.x, end.y, std::stoi(strs[4])));
					}
				}
			}
			in.close();

			// compute vanishign points
			std::vector<glm::dvec2> vps;
			vp::computeVanishingPoints(lines, vps);

			// convert the coordinates of vp to [-1, 1]
			for (int i = 0; i < vps.size(); ++i) {
				vps[i].x = vps[i].x / 800.0f * 2.0 - 1;
				vps[i].y = 1 - vps[i].y / 800.0f * 2.0;
			}

			// compute camera parameter
			glm::dmat3 K, R;
			vp::extractCameraMatrixByGenApproach(vps, K, R);
			//std::cout << glm::to_string(K) << std::endl;

			int image_size = std::max(img.rows, img.cols);

			// get camera center
			glm::vec2 camera_center = glm::vec2(K[2][0] * image_size * 0.5, K[2][1] * image_size * 0.5);

			// new canvas
			cv::Mat result(img.rows + abs(camera_center.y) * 2, img.cols + abs(camera_center.x) * 2, CV_8UC3, cv::Scalar(255, 255, 255));
			int result_size = std::max(result.rows, result.cols);

			// copy the original image
			glm::vec2 image_offset(camera_center.x >= 0 ? 0 : -camera_center.x * 2, camera_center.y >= 0 ? camera_center.y * 2 : 0);
			cv::Mat roi(result, cv::Rect(image_offset.x, image_offset.y, img.cols, img.rows));
			img.copyTo(roi);

			cv::imwrite((std::string("result/") + std::to_string(id) + ".jpg").c_str(), result);

			// convert the lines
			center_point.x = center_point.x * (float)image_size / result_size + (float)(result_size - result.cols - image_size + img.cols) / result_size * 400.0f + (float)image_offset.x / result_size * 800.0f;
			center_point.y = center_point.y * (float)image_size / result_size + (float)(result_size - result.rows - image_size + img.rows) / result_size * 400.0f + (float)image_offset.y / result_size * 800.0f;
			for (int k = 0; k < lines.size(); ++k) {
				lines[k].start.x = lines[k].start.x * (float)image_size / result_size + (float)(result_size - result.cols - image_size + img.cols) / result_size * 400.0f + (float)image_offset.x / result_size * 800.0f;
				lines[k].start.y = lines[k].start.y * (float)image_size / result_size + (float)(result_size - result.rows - image_size + img.rows) / result_size * 400.0f + (float)image_offset.y / result_size * 800.0f;
				lines[k].end.x = lines[k].end.x * (float)image_size / result_size + (float)(result_size - result.cols - image_size + img.cols) / result_size * 400.0f + (float)image_offset.x / result_size * 800.0f;
				lines[k].end.y = lines[k].end.y * (float)image_size / result_size + (float)(result_size - result.rows - image_size + img.rows) / result_size * 400.0f + (float)image_offset.y / result_size * 800.0f;
			}

			// save the converted lines
			std::ofstream out((std::string("result/lines_") + std::to_string(id) + ".txt").c_str());
			out << center_point.x << " " << center_point.y << std::endl;
			for (int k = 0; k < lines.size(); ++k) {
				out << lines[k].start.x << " " << lines[k].start.y << " " << lines[k].end.x << " " << lines[k].end.y << " " << lines[k].type << std::endl;
			}
			out.close();
		}
	}

	return 0;
}