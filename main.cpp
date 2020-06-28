#include <Eigen/Core>
#include <Eigen/Geometry>
#include <iomanip>
#include <iostream>
#include <pangolin/pangolin.h>
using namespace Eigen;
using namespace std;

struct EulerAngle {
    EulerAngle()
    {
        roll_ = 0.;
        pitch_ = 0.;
        yaw_ = 0.;
    }
public:
    double roll_;
    double pitch_;
    double yaw_;
};

std::ostream& operator<<(std::ostream& os, const EulerAngle& o)
{
    // yaw-pitch-roll => ZYX
    os << o.yaw_ << " " << o.pitch_ << " " << o.roll_;
    return os;
}

std::istream& operator>>(std::istream& is, EulerAngle& o)
{
    is >> o.yaw_;
    is >> o.pitch_;
    is >> o.roll_;
    return is;
}

pangolin::Var<EulerAngle>* eulerAngle;

struct RotationMatrix {
    Matrix3d matrix = Matrix3d::Identity();
};

ostream& operator<<(ostream& out, const RotationMatrix& r)
{
    out.setf(ios::fixed);
    Matrix3d matrix = r.matrix;
    out << '=';
    out << "[" << setprecision(2) << matrix(0, 0) << "," << matrix(0, 1) << "," << matrix(0, 2) << "],"
        << "[" << matrix(1, 0) << "," << matrix(1, 1) << "," << matrix(1, 2) << "],"
        << "[" << matrix(2, 0) << "," << matrix(2, 1) << "," << matrix(2, 2) << "]";
    return out;
}

istream& operator>>(istream& in, RotationMatrix& r)
{
    return in;
}

struct QuaternionDraw {
    Quaterniond q;
};

ostream& operator<<(ostream& out, const QuaternionDraw quat)
{
    auto c = quat.q.coeffs();
    out << "=[" << c[0] << "," << c[1] << "," << c[2] << "," << c[3] << "]";
    return out;
}

istream& operator>>(istream& in, const QuaternionDraw quat)
{
    return in;
}

void ResetCallback()
{
    std::cout << "You typed ctrl-r or pushed reset" << std::endl;
    EulerAngle ea;
    *eulerAngle = ea;
}

Eigen::Quaterniond euler2Quaternion(const double yaw, const double pitch, const double roll)
{
    Eigen::AngleAxisd rollAngle(roll, Eigen::Vector3d::UnitX());
    Eigen::AngleAxisd pitchAngle(pitch, Eigen::Vector3d::UnitY());
    Eigen::AngleAxisd yawAngle(yaw, Eigen::Vector3d::UnitZ());

    Eigen::Quaterniond q = yawAngle * pitchAngle * rollAngle;
    return q;
}

int main(int argc, char** argv)
{
    const int UI_WIDTH = 500;
    pangolin::CreateWindowAndBind("visualize geometry", 1000, 500);
    glEnable(GL_DEPTH_TEST);
    // OpenGlMatrixSpec pangolin::ProjectionMatrix(int w, int h, double fu, double fv, double u0, double v0, double zNear, double zFar);
    //OpenGlMatrix 	ModelViewLookAt (double ex, double ey, double ez, double lx, double ly, double lz, AxisDirection up)
    pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(1000, 600, 420, 420, 500, 300, 0.1, 100),
        pangolin::ModelViewLookAt(3, 3, 3, 0, 0, 0, pangolin::AxisZ));
    pangolin::View& d_cam = pangolin::CreateDisplay()
                                .SetBounds(0.0, 1.0, pangolin::Attach::Pix(UI_WIDTH), 1.0, -1000.0f / 600.0f)
                                .SetHandler(new pangolin::Handler3D(s_cam));
    // ui
    pangolin::CreatePanel("ui").SetBounds(0.0, 1.0, 0.0, pangolin::Attach::Pix(UI_WIDTH));
    eulerAngle = new pangolin::Var<EulerAngle>("ui.YawPitchRoll", EulerAngle());
    pangolin::Var<RotationMatrix> rotation_matrix("ui.R", RotationMatrix());
    pangolin::Var<QuaternionDraw> quaternion("ui.qaternon", QuaternionDraw());
    pangolin::Var<std::function<void(void)>> reset("ui.Reset", ResetCallback);

    while (!pangolin::ShouldQuit()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        d_cam.Activate(s_cam);

        // input eulerangle
        EulerAngle ea = *eulerAngle;
        cout << "yaw: " << ea.yaw_ << "\t pitch: " << ea.pitch_ << "\troll: " << ea.roll_ << endl;

        // Eulerangle to quaterniond
        Eigen::Quaterniond qd = euler2Quaternion(ea.yaw_ / 180 * M_PI, ea.pitch_ / 180 * M_PI, ea.roll_ / 180 * M_PI);

        // show quaternion
        QuaternionDraw quat;
        quat.q = qd;
        quaternion = quat;

        // quaterniond to rotationmatrix
        Matrix3d rot = qd.toRotationMatrix();

        // pangolin::OpenGlMatrix matrix = s_cam.GetModelViewMatrix();
        RotationMatrix R;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                R.matrix(i, j) = rot(j, i);
        rotation_matrix = R;
        // transform matrix for pangolin
        Matrix<double, 4, 4> matrix;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                matrix(i, j) = rot(j, i);

        s_cam.SetModelViewMatrix(pangolin::OpenGlMatrix(matrix));

        pangolin::glDrawColouredCube();

        // draw the original axis
        glLineWidth(3);
        glColor3f(0.8f, 0.f, 0.f);
        glBegin(GL_LINES);
        glVertex3f(0, 0, 0);
        glVertex3f(10, 0, 0);
        glColor3f(0.f, 0.8f, 0.f);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 10, 0);
        glColor3f(0.2f, 0.2f, 1.f);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 10);
        glEnd();

        pangolin::FinishFrame();
    }

    return 0;
}
