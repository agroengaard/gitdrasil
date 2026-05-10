#pragma once
#include <iostream>
#include <Eigen/Dense>
using Eigen::VectorXd;

class Foil{
    public:
        std::string name     ;
        double chord_length   ;
        double angle_of_attack ;
        int    num_pts         ;
        VectorXd x;
        VectorXd y;  
        VectorXd yc;  
        VectorXd Pts_x;
        VectorXd Pts_y;     

        Foil(std::string name, double chord_length, double angle_of_attack);
        
        void display_info();
        void plot();
};


class NACA : public Foil {
public:
    VectorXd dyc_dx;
    VectorXd yt;

    NACA(std::string name, double chord_length, double angle_of_attack);
    void calculate_thickness_distribution();
    void calculate_ordinates();
};

class NACA4DIGIT : public NACA {
public:
    NACA4DIGIT(std::string name, double chord_length, double angle_of_attack);
    void calculate_camber_line();
};

class NACA5DIGIT : public NACA {
public:
    NACA5DIGIT(std::string name, double chord_length, double angle_of_attack);
    void calculate_camber_line();
};

class Biconvex : public Foil {
    public:
        Biconvex(std::string name, double chord_length, double angle_of_attack);
        void calculate_pts();
};