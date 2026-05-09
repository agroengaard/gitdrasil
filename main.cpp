#include <iostream>
#include <algorithm>
#include <Eigen/Dense>
#include <sciplot/sciplot.hpp>
#include <cmath>
using namespace sciplot;
using Eigen::VectorXd;
using Eigen::Vector2d; 

 
VectorXd linspace(double start, double end, std::size_t num_points) {
    VectorXd result(num_points);
    double step = (end - start) / (num_points - 1);

    for (std::size_t i = 0; i < num_points; ++i) {
        result[i] = start + i * step;
    }

    return result;
}

class Foil{
    public:
        std::string name       = "NoName";
        double chord_length    = 1.0;
        double angle_of_attack = 0.0;
        int    num_pts         = 100;
        VectorXd x;
        VectorXd y;  

        Foil(std::string name, double chord_length, double angle_of_attack) {
        
        this->name = name;
        this->chord_length = chord_length;
        this->angle_of_attack = angle_of_attack;
        this->x = VectorXd::Zero(num_pts);
        this->y = VectorXd::Zero(num_pts);
            };
        
    void display_info() {
        std::cout << "This Foils name is " << name << "\n";
    };
};

class NACA : public Foil {
    public:
        VectorXd yc;
        VectorXd dyc_dx;
        VectorXd yt;
        VectorXd Pts_x;
        VectorXd Pts_y;
        NACA(std::string name, double chord_length, double angle_of_attack): Foil(name, chord_length, angle_of_attack) {
            four_digit();
            calculate_thickness_distribution();
            calculate_ordinates();
        };


    void four_digit() {
        std::cout << "Initializing camber calculation" << "\n";
        double M = name[0];  M = M/ 100;
        double P = name[1];  P = P / 10;
   

        this->x = linspace(0.0, 1.0, num_pts);
        auto it = std::find_if(x.data(), x.data() + x.size(), [P](double val) { return val > P; }); 
        int split_idx = std::distance(x.data(), it);
        VectorXd x1 = x.head(split_idx);
        VectorXd x2 = x.tail(x.size() - split_idx);
        
        if (M == 0) {
            this->yc     = VectorXd::Zero(x.size());
            this->dyc_dx = VectorXd::Zero(x.size());
        } else {
            VectorXd yc1 = (M/(P*P))*((2.0*P*x1.array())- x1.array().square()).matrix();                 // Camber line
            VectorXd yc2 = ((M / ((1.0 - P) * (1.0 - P))) * (1.0 - (2.0 * P) + (2.0 * P * x2.array()) - x2.array().square())).matrix();
            VectorXd dyc_dx1 = (((2.0 * M) / (P * P)) * (P - x1.array())).matrix();                     // Derivative of the camber line
            VectorXd dyc_dx2 = ((2.0 * M) / ((1.0 - P) * (1.0 - P)) * (P - x2.array())).matrix();

            this->yc     = (VectorXd(x1.size() + x2.size()) << yc1, yc2).finished();
            this->dyc_dx = (VectorXd(dyc_dx1.size() + dyc_dx2.size()) << dyc_dx1, dyc_dx2).finished();
        };
    };
    void calculate_thickness_distribution() {
        std::cout << "Initializing thickness distribution calculation" << "\n";
        VectorXd a(5);
        a << 0.2969, -0.1260, -0.3516, 0.2843, -0.1036;
        double TT = std::stod(name.substr(2, 2))/100;
        VectorXd yt = 5.0 * TT * (
            a[0] * x.array().sqrt() +
            a[1] * x.array() +
            a[2] * x.array().square() +
            a[3] * x.array().cube() +
            a[4] * x.array().pow(4)
        );
        this->yt = yt;
    };
    void calculate_ordinates() {
        std::cout << "Initializing ordinates calculation" << "\n";
        VectorXd theta = dyc_dx.array().atan();
        VectorXd xu = x.array()  - yt.array() * theta.array().sin();
        VectorXd yu = yc.array() + yt.array() * theta.array().cos();
        VectorXd xl = x.array()  + yt.array() * theta.array().sin();
        VectorXd yl = yc.array() - yt.array() * theta.array().cos();

        VectorXd sliced = xu.tail(xu.size() - 1);
        VectorXd flipped = sliced.reverse();
        VectorXd yu_sliced  = yu.tail(yu.size() - 1);
        VectorXd yu_flipped = yu_sliced.reverse();

        Pts_x.resize(flipped.size() + xl.size());
        Pts_x << flipped, xl;

        Pts_y.resize(yu_flipped.size() + yl.size());
        Pts_y << yu_flipped, yl;

        //VectorXd Pts_x = np.append(np.flip(xu[1:]), xl);
       // VectorXd Pts_y = np.append(np.flip(yu[1:]), yl);
       // Pts_z = np.zeros(np.size(Pts_y))                                       # Initializing z-vector

       // self.base_pts = np.concatenate((Pts_x.T[:, None], 
       //                            Pts_y.T[:, None], 
       ////                            Pts_z.T[:, None]), 
       //                            axis=1)
        
        //self.pts = self.base_pts
    };

    void tell_me_about_yourself() {
        std::cout << "I am a NACA foil, I am a child of Foil, I have access to all the properties and methods of Foil\n";
    };
};

int main() {
    NACA naca0013("2413", 1.0, 10.0);
    naca0013.display_info();


    // Convert Eigen vectors to std::vector for sciplot
    std::vector<double> pts_x(naca0013.Pts_x.data(), naca0013.Pts_x.data() + naca0013.Pts_x.size());
    std::vector<double> pts_y(naca0013.Pts_y.data(), naca0013.Pts_y.data() + naca0013.Pts_y.size());
    std::vector<double> camber_x(naca0013.x.data(), naca0013.x.data() + naca0013.x.size());
    std::vector<double> camber_y(naca0013.yc.data(), naca0013.yc.data() + naca0013.yc.size());
    Plot2D plot1;
    plot1.palette("paired");
    plot1.drawCurve(pts_x, pts_y).label("NACA 2413").lineWidth(2);
    plot1.drawCurve(camber_x, camber_y).label("Camber Line").lineWidth(2);
    plot1.xrange(-0.1, 1.1);
    plot1.yrange(-0.2, 1.1);
    plot1.xlabel("x/c");
    plot1.ylabel("y/c");

    Figure figure = {{plot1}};
    Canvas canvas = {{figure}};
    canvas.defaultPalette("set1");

    canvas.show();
    canvas.save("naca2413.svg");
}