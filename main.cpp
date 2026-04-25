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


int main() {

    Foil foil2("NACA 0013", 1.0, 10.0);
    foil2.display_info();

    Haack h("HaackSeries", 1.0, 5.0, 2.0, 0.5, 0.3);
    h.display_info();

    int array[10];
    linspace_array(array, 0, 10);
   // printArray(array);
    return 0;
}