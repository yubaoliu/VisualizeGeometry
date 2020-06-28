// https://stackoverflow.com/questions/21412169/creating-a-rotation-matrix-with-pitch-yaw-roll-using-eigen
#include <Eigen/Dense>
#include <iostream>
#include <math.h>

Eigen::Matrix3d rotation_from_euler(double roll, double pitch, double yaw)
{
    // roll and pitch and yaw in radians
    double su = sin(roll);
    double cu = cos(roll);
    double sv = sin(pitch);
    double cv = cos(pitch);
    double sw = sin(yaw);
    double cw = cos(yaw);
    Eigen::Matrix3d Rot_matrix(3, 3);
    Rot_matrix(0, 0) = cv * cw;
    Rot_matrix(0, 1) = su * sv * cw - cu * sw;
    Rot_matrix(0, 2) = su * sw + cu * sv * cw;
    Rot_matrix(1, 0) = cv * sw;
    Rot_matrix(1, 1) = cu * cw + su * sv * sw;
    Rot_matrix(1, 2) = cu * sv * sw - su * cw;
    Rot_matrix(2, 0) = -sv;
    Rot_matrix(2, 1) = su * cv;
    Rot_matrix(2, 2) = cu * cv;
    return Rot_matrix;
}

int main()
{
    Eigen::Matrix3d rot_mat = rotation_from_euler(0, 0, 0.5 * M_PI);
    std::cout << rot_mat << std::endl;
    return 0;
}
