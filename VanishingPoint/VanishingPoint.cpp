#include "VanishingPoint.h"

namespace vp {

	/**
	 * Compute all three vanishing points, left of the horizontal ones, right of the horizontal ones, and a vertical one.
	 */
	void computeVanishingPoints(const std::vector<VanishingLine>& lines, std::vector<glm::dvec2>& vps) {
		vps.resize(3);

		// compute vp of horizontal left
		std::vector<VanishingLine> lines_horizontal_left;
		for (auto line : lines) {
			if (line.type == VanishingLine::TYPE_HORIZONTAL_LEFT) {
				lines_horizontal_left.push_back(line);
			}
		}
		vps[0] = computeVanishingPoint(lines_horizontal_left);

		// compute vp of horizontal right
		std::vector<VanishingLine> lines_horizontal_right;
		for (auto line : lines) {
			if (line.type == VanishingLine::TYPE_HORIZONTAL_RIGHT) {
				lines_horizontal_right.push_back(line);
			}
		}
		vps[1] = computeVanishingPoint(lines_horizontal_right);

		// compute vp of vertical
		std::vector<VanishingLine> lines_vertical;
		for (auto line : lines) {
			if (line.type == VanishingLine::TYPE_VERTICAL) {
				lines_vertical.push_back(line);
			}
		}
		vps[2] = computeVanishingPoint(lines_vertical);
	}

	/**
	 * Compute a vanishing point in a least squared manner.
	 * Each line is represented by a_1 x + a_2 y = b.
	 * Then, the entire system can be formulated as A[x,y]^T = b,
	 * where A is a N x 2 matrix and b is a N x 1 matrix.
	 * Finally, [x, y] can be computed as A^-1 * b.
	 *
	 * TODO: use RANSAC to remove outliers and get more reliable vanishign point.
	 */
	glm::dvec2 computeVanishingPoint(const std::vector<VanishingLine>& lines) {
		cv::Mat_<double> A(lines.size(), 2);
		cv::Mat_<double> b(lines.size(), 1);
		for (int i = 0; i < lines.size(); ++i) {
			A(i, 0) = lines[i].end.y - lines[i].start.y;
			A(i, 1) = lines[i].start.x - lines[i].end.x;
			b(i, 0) = (lines[i].end.y - lines[i].start.y) * lines[i].start.x + (lines[i].start.x - lines[i].end.x) * lines[i].start.y;
		}

		cv::Mat_<double> invA;
		cv::invert(A, invA, cv::DECOMP_SVD);
		cv::Mat X = invA * b;

		glm::dvec2 result(X.at<double>(0, 0), X.at<double>(1, 0));
		return result;
	}

	void extractCameraMatrix(const std::vector<glm::dvec2>& vps, glm::dmat3& K, glm::dmat3& R) {
		// assume the center of the image be the orthographic projection of the camera center
		glm::dvec2 Oi(0, 0);

		// compute the projection of Oi on the horizontal vanishing line
		glm::dvec2 Vi = vp::nearestPointOnLine(vps[0], vps[1], glm::dvec2(0, 0));

		// compute the length between Vi and the camera center
		double OcVi = sqrt(glm::length(vps[0] - Vi) * glm::length(vps[1] - Vi));

		// compute the folca length by Eq (4) in the paper "Camera calibration using two or three vanishing points" by Orghidan et al.
		double f = sqrt(OcVi * OcVi - glm::dot(Vi - Oi, Vi - Oi));

		// recover the intrinsic matrix by Eq (3) in the paper "Camera calibration using two or three vanishing points" by Orghidan et al.
		K = glm::dmat3();
		K[0][0] = f;
		K[1][1] = f;
		K[2][2] = 1;

		// recover the rotation matrix by Eq (6) in the paper "Camera calibration using two or three vanishing points" by Orghidan et al.
		R = glm::dmat3();
		for (int i = 0; i < 2; ++i) {
			double denom = sqrt(vps[i].x * vps[i].x + vps[i].y * vps[i].y + f * f);
			R[(i + 2) % 3][0] = vps[i].x / denom * (i == 0 ? -1 : 1);
			R[(i + 2) % 3][1] = vps[i].y / denom * (i == 0 ? -1 : 1);
			R[(i + 2) % 3][2] = -f / denom * (i == 0 ? -1 : 1);
		}
		R[1] = glm::cross(R[2], R[0]);
	}

	void extractCameraMatrix(const std::vector<glm::dvec2>& vps, double f, const glm::dvec2& origin, double camera_ditance, glm::dvec3& T) {
		// define O_c P_1' in Eq (11)
		glm::dvec3 OP1(origin.x, origin.y, -f);

		// compute T
		T = OP1 * (camera_ditance / f);
	}

	glm::dvec2 projectPoint(const glm::dmat4& mvpMatrix, const glm::dvec3& p) {
		glm::dvec4 pp = mvpMatrix * glm::dvec4(p, 1);
		return glm::dvec2(pp.x / pp.w, pp.y / pp.w);
	}

	/**
	 * Create a rotation matrix.
	 * Note: glm mat4 contains element values in column major. For instance,
	 *       X[1][2] means a element at 3rd row and 2nd column.
	 *       This is different from a typical notation, so be careful!
	 *
	 * @param xrot		rotation around X axis in radian
	 * @param yrot		rotation around Y axis in radian
	 * @param zrot		rotation around Z axis in radian
	 * @return			rotation matrix
	 */
	glm::dmat3 composeRotation(double xrot, double yrot, double zrot) {
		glm::dmat3 X;
		X[1][1] = cos(xrot);
		X[2][1] = -sin(xrot);
		X[1][2] = sin(xrot);
		X[2][2] = cos(xrot);

		glm::dmat3 Y;
		Y[0][0] = cos(yrot);
		Y[2][0] = sin(yrot);
		Y[0][2] = -sin(yrot);
		Y[2][2] = cos(yrot);

		glm::dmat3 Z;
		Z[0][0] = cos(zrot);
		Z[1][0] = -sin(zrot);
		Z[0][1] = sin(zrot);
		Z[1][1] = cos(zrot);

		return X * Y * Z;
	}

	/**
	 * Convert the rotation matrix to three rotation angles around axes.
	 * ToDO: Degenerate cases are not supported yet.
	 *       See the below to support them.
	 *       http://www.staff.city.ac.uk/~sbbh653/publications/euler.pdf
	 *
	 * @param R		rotation matrix
	 * @param xrot	rotation around X axis in radian
	 * @param yrot	rotation around Y axis in radian
	 * @param zrot	rotation around Z axis in radian
	 */
	void decomposeRotation(const glm::dmat3& R, double& xrot, double& yrot, double& zrot) {
		xrot = atan2(-R[2][1], R[2][2]);
		yrot = atan2(R[2][0], std::hypot(R[2][1], R[2][2]));
		zrot = atan2(-R[1][0], R[0][0]);
	}

	glm::dvec2 nearestPointOnLine(const glm::dvec2& p1, const glm::dvec2& p2, const glm::dvec2& p) {
		glm::dvec2 dir = p2 - p1;
		return p1 + dir * glm::dot(p - p1, dir) / glm::dot(dir, dir);
	}

	double deg2rad(double deg) {
		return deg / 180.0 * M_PI;
	}

	double rad2deg(double rad) {
		return rad / M_PI * 180.0;
	}

}