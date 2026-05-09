#include <iostream>
#include <algorithm>
#include <Eigen/Dense>
#include <sciplot/sciplot.hpp>
#include <cmath>
using namespace sciplot;
using Eigen::VectorXd;
using Eigen::Vector2d; 

 
class Foil{
    public:
        std::string name       = "NoName";
        double chord_length    = 1.0;
        double angle_of_attack = 0.0;
        int    num_pts         = 100;
        VectorXd x;
        VectorXd y;  
        VectorXd yc;  
        VectorXd Pts_x;
        VectorXd Pts_y;      
        
        Foil(std::string name, double chord_length, double angle_of_attack) {
        
        this->name = name;
        this->chord_length = chord_length;
        this->angle_of_attack = angle_of_attack;
        this->y = VectorXd::Zero(num_pts);
            };
        
    void display_info() {
        std::cout << "This Foils name is " << name << "\n";
    };

    void plot() {
        std::vector<double> pts_x(Pts_x.data(), Pts_x.data() + Pts_x.size());
        std::vector<double> pts_y(Pts_y.data(), Pts_y.data() + Pts_y.size());
        std::vector<double> camber_x(x.data(), x.data() + x.size());
        std::vector<double> camber_y(yc.data(), yc.data() + yc.size());

        Plot2D plot1;
        plot1.palette("paired");
        plot1.drawCurve(pts_x, pts_y).label("NACA " + name).lineWidth(2);
        plot1.drawCurve(camber_x, camber_y).label("Camber Line").lineWidth(2);
        plot1.xrange(-0.1, 1.1);
        plot1.yrange(-0.2, 1.1);
        plot1.xlabel("x/c");
        plot1.ylabel("y/c");

        Figure figure = {{plot1}};
        Canvas canvas = {{figure}};
        canvas.defaultPalette("set1");
        canvas.show();
        canvas.save("naca" + name + ".svg");
    };
};

class NACA : public Foil {
    public:
        VectorXd dyc_dx;
        VectorXd yt;
        NACA(std::string name, double chord_length, double angle_of_attack): Foil(name, chord_length, angle_of_attack) {};

    void calculate_thickness_distribution() {
        std::cout << "Initializing thickness distribution calculation" << "\n";
        VectorXd a(5);
        a << 0.2969, -0.1260, -0.3516, 0.2843, -0.1036;
        double TT = std::stod(name.substr(2, 2))/100;
        this->yt = 5.0 * TT * (
            a[0] * x.array().sqrt() +
            a[1] * x.array() +
            a[2] * x.array().square() +
            a[3] * x.array().cube() +
            a[4] * x.array().pow(4)
        );
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

    };

};

class NACA4DIGIT : public NACA {
    public:
        NACA4DIGIT(std::string name, double chord_length, double angle_of_attack): NACA(name, chord_length, angle_of_attack) {
            calculate_camber_line();
            calculate_thickness_distribution();
            calculate_ordinates();
        };

    void calculate_camber_line() {
        std::cout << "Initializing camber calculation" << "\n";
        double M = (name[0] - '0') / 100.0;
        double P = (name[1] - '0') / 10.0;
   

        this->x = VectorXd::LinSpaced(num_pts, 0.0, 1.0);
        int split_idx = std::upper_bound(x.data(), x.data() + x.size(), P) - x.data();
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
};

class NACA5DIGIT : public NACA {
    public:
        NACA5DIGIT(std::string name, double chord_length, double angle_of_attack): NACA(name, chord_length, angle_of_attack) {
            calculate_camber_line();
            calculate_thickness_distribution();
            calculate_ordinates();
        };


    void calculate_camber_line() {
        if (name[2] != '0' && name[2] != '1') {
            throw std::invalid_argument("Third digit in a 5-digit NACA Airfoil should be 1 or 0");
        }

        double P      = (name[1] - '0') * 0.05;
        int    reflex = name[2] - '0';

        VectorXd p_tbl, M_tbl, K_tbl;
        if (reflex == 0) {
            p_tbl.resize(5); M_tbl.resize(5); K_tbl.resize(5);
            p_tbl << 0.05, 0.10, 0.15, 0.20, 0.25;
            M_tbl << 0.0580, 0.1260, 0.2025, 0.2900, 0.3910;
            K_tbl << 361.4,  51.64, 15.957,  6.643,  3.230;
        } else {
            p_tbl.resize(4); M_tbl.resize(4); K_tbl.resize(4);
            p_tbl << 0.10, 0.15, 0.20, 0.25;
            M_tbl << 0.130, 0.217, 0.318, 0.441;
            K_tbl << 51.99, 15.793, 6.520, 3.191;
        }

        // Linear interpolation (replaces scipy CubicSpline for these small tables)
        auto lerp = [](const VectorXd& xs, const VectorXd& ys, double t) {
            for (int i = 0; i < xs.size() - 1; ++i) {
                if (t >= xs[i] && t <= xs[i + 1]) {
                    double frac = (t - xs[i]) / (xs[i + 1] - xs[i]);
                    return ys[i] + frac * (ys[i + 1] - ys[i]);
                }
            }
            return t <= xs[0] ? ys[0] : ys[ys.size() - 1];
        };

        double m  = lerp(p_tbl, M_tbl, P);
        double k1 = lerp(M_tbl, K_tbl, m);

        this->x = VectorXd::LinSpaced(num_pts, 0.0, 1.0);
        this->yc.resize(num_pts);
        this->dyc_dx.resize(num_pts);

        if (reflex == 0) {
            for (int i = 0; i < num_pts; ++i) {
                double xi = x[i];
                if (xi < P) {
                    yc[i]     = (k1/6.0) * (xi*xi*xi - 3.0*m*xi*xi + m*m*(3.0-m)*xi);
                    dyc_dx[i] = (k1/6.0) * (3.0*xi*xi - 6.0*m*xi + m*m*(3.0-m));
                } else {
                    yc[i]     = (k1*m*m*m/6.0) * (1.0 - xi);
                    dyc_dx[i] = -(k1*m*m*m/6.0);
                }
            }
        } else {
            double K1_K2 = (3.0*(m-P)*(m-P) - m*m*m) / ((1.0-m)*(1.0-m)*(1.0-m));
            for (int i = 0; i < num_pts; ++i) {
                double xi = x[i];
                if (xi < P) {
                    yc[i]     = (k1/6.0) * (std::pow(xi-m,3) - K1_K2*std::pow(1.0-m,3)*xi - m*m*m*xi + m*m*m);
                    dyc_dx[i] = (k1/6.0) * (3.0*std::pow(xi-m,2) - K1_K2*std::pow(1.0-m,3) - m*m*m);
                } else {
                    yc[i]     = (k1/6.0) * (K1_K2*std::pow(xi-m,3) - K1_K2*std::pow(1.0-m,3)*xi - m*m*m*xi + m*m*m);
                    dyc_dx[i] = (k1/6.0) * (3.0*K1_K2*std::pow(xi-m,2) - K1_K2*std::pow(1.0-m,3) - m*m*m);
                }
            }
        }
    };
};


int main() {
    NACA4DIGIT naca0013("2413", 1.0, 10.0);
    naca0013.display_info();
    naca0013.plot();
    NACA5DIGIT naca23113("23113", 1.0, 10.0);
    naca23113.display_info();
    naca23113.plot();
}