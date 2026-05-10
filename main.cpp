
#include "foils.h"

int main() {
    NACA4DIGIT naca0013("2413", 1.0, 10.0);
    naca0013.display_info();
    naca0013.plot();
    NACA5DIGIT naca23113("23113", 1.0, 10.0);
    naca23113.display_info();
    naca23113.plot();
    Biconvex bitest("bitest", 1.0, 10.0);
    bitest.display_info();
    bitest.plot();
}