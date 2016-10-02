#include "VanishingPoint.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>
#include <glm/gtx/string_cast.hpp>
#include <sstream>
#include <string>
#include <iomanip>

glm::vec2 convert(const glm::vec2& p, int orig_width, int orig_height, int new_width, int new_height, const glm::vec2& image_offset) {
	float orig_size = std::max(orig_width, orig_height);
	float new_size = std::max(new_width, new_height);

	glm::vec2 ret;
	ret.x = p.x * orig_size / new_size + (float)(new_size - new_width - orig_size + orig_width) / new_size * 400.0f + (float)image_offset.x / new_size * 800.0f;
	ret.y = p.y * orig_size / new_size + (float)(new_size - new_height - orig_size + orig_height) / new_size * 400.0f + (float)image_offset.y / new_size * 800.0f;
	return ret;
}

int main() {
	boost::filesystem::path dir("data");
	for (auto it = boost::filesystem::directory_iterator(dir); it != boost::filesystem::directory_iterator(); ++it) {
		if (it->path().filename().extension().string() == ".txt") {
			std::cout << it->path().filename().string() << std::endl;

			int id;
			sscanf(it->path().filename().string().c_str(), "lines_%d.txt", &id);

			// convert id to 2 digits string
			std::stringstream ss;
			ss << std::setw(2) << std::setfill('0') << id;			std::string str_id = ss.str();
			// read image
			std::string path = std::string("data/") + str_id + ".jpg";
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

			// read silhouette file
			std::vector<vp::VanishingLine> silhouette;
			std::string path2 = std::string("data/") + str_id + ".ctr";
			if (boost::filesystem::exists(path2)) {
				std::ifstream in2(path2);
				while (!in2.eof()) {
					std::string line;
					std::getline(in2, line);

					std::vector<std::string> strs;
					boost::split(strs, line, boost::is_any_of("\t "));

					if (strs.size() == 5) {
						glm::dvec2 start(std::stof(strs[0]), std::stof(strs[1]));
						glm::dvec2 end(std::stof(strs[2]), std::stof(strs[3]));
						if (start != end) {
							silhouette.push_back(vp::VanishingLine(start.x, start.y, end.x, end.y, std::stoi(strs[4])));
						}
					}
				}
				in2.close();
			}

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

			cv::imwrite((std::string("result/") + str_id + ".jpg").c_str(), result);

			// convert the lines
			center_point = convert(center_point, img.cols, img.rows, result.cols, result.rows, image_offset);
			for (int k = 0; k < lines.size(); ++k) {
				lines[k].start = convert(lines[k].start, img.cols, img.rows, result.cols, result.rows, image_offset);
				lines[k].end = convert(lines[k].end, img.cols, img.rows, result.cols, result.rows, image_offset);
			}

			// save the converted lines
			std::ofstream out((std::string("result/lines_") + std::to_string(id) + ".txt").c_str());
			out << center_point.x << " " << center_point.y << std::endl;
			for (int k = 0; k < lines.size(); ++k) {
				out << lines[k].start.x << " " << lines[k].start.y << " " << lines[k].end.x << " " << lines[k].end.y << " " << lines[k].type << std::endl;
			}
			out.close();

			// convert silhouette
			if (silhouette.size() > 0) {
				for (int k = 0; k < silhouette.size(); ++k) {
					silhouette[k].start = convert(silhouette[k].start, img.cols, img.rows, result.cols, result.rows, image_offset);
					silhouette[k].end = convert(silhouette[k].end, img.cols, img.rows, result.cols, result.rows, image_offset);
				}
			}

			// save the converted silhouette
			std::ofstream out2((std::string("result/") + str_id + ".ctr").c_str());
			for (int k = 0; k < silhouette.size(); ++k) {
				out2 << silhouette[k].start.x << " " << silhouette[k].start.y << " " << silhouette[k].end.x << " " << silhouette[k].end.y << " " << silhouette[k].type << std::endl;
			}
			out2.close();
		}
	}

	return 0;
}