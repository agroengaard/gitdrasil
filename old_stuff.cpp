#include <iostream>
#include <vector>

void printVector(const std::vector<int>& arr) {
    for (int i = 0; i < arr.size(); i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n";
}

void linspace_array(int *array, int min_n, int max_n){

    double step = (max_n - min_n) / sizeof(array) ;

    for(int i = 0; i < sizeof(array); i++) {
        array[i] = min_n + i * step;
    };
};

class Foil{
    public:
        std::string name       = "NoName";
        double chord_length    = 1.0;
        double angle_of_attack = 0.0;
        int    num_pts         = 100;

    Foil(std::string name, double chord_length, double angle_of_attack) {
       
     this->name = name;
     this->chord_length = chord_length;
     this->angle_of_attack = angle_of_attack;
        };
        
        void display_info() {
            std::cout << "This Foils name is " << name << "\n";
        };
};

class Nose : public Foil{
    public:
        Nose(std::string name, double chord_length, double angle_of_attack): Foil(name, chord_length, angle_of_attack) {}

    void tell_me_about_yourself() {
        std::cout << "I am a Nose, I am a child of Foil, I have access to all the properties and methods of Foil\n";
    };
};

class Haack : public Nose {
    public:
        double L=1.0;
        double R=0.25;
        double C=0.3333;

        Haack(std::string name, double chord_length, double angle_of_attack, double L, double R, double C) : Nose(name, chord_length, angle_of_attack),  L(L), R(R), C(C) {}

        void calculate_shape() {}
        void create_pts() {}

};


class Point {
public:
    double x_;
    double y_;

    Point(double x, double y) {
        x_ = x;
        y_ = y; 
    }

};

class Line {
public:
    std::vector<Point> points;

    Line(Point p1, Point p2) {
        points.push_back(p1);
        points.push_back(p2);
    }
    void addPoint(const Point& point) {
        points.push_back(point);
    }
};

class Sketch {
public:
    std::vector<Line> lines;
    bool is_fully_connected;

    Sketch () : is_fully_connected(false) {
        
    }

    void addLine(const Line& line) {
        lines.push_back(line);
    }

    void calculate_if_fully_connected() {
        // Implement logic to check if all lines are connected
        Point start_point = lines[0].points[0];

    };
};

#include <sciplot/sciplot.hpp>
#include <cmath>
using namespace sciplot;

int main()
{
Vec x = linspace(0.0, 5.0, 100);

Plot2D plot1;
plot1.palette("paired");
plot1.drawCurve(x, std::sin(x)).label("sin(x)").lineWidth(4);
plot1.drawCurve(x, std::cos(x)).label("cos(x)").lineWidth(2);

Plot2D plot2;
plot2.drawCurve(x, std::tan(x)).label("tan(x)").lineWidth(4);

Figure figure = {{plot1, plot2}};
Canvas canvas = {{figure}};
canvas.defaultPalette("set1");

canvas.show(); // Display in a pop-up
canvas.save("plot.svg"); // Save to file
}
//int main() {
//
//    Foil foil2("NACA 0013", 1.0, 10.0);
//    foil2.display_info();
//
//   Haack h("HaackSeries", 1.0, 5.0, 2.0, 0.5, 0.3);
//    h.display_info();
//
//    int array[10];
//    linspace_array(array, 0, 10);
//   // printArray(array);
//    return 0;
//}