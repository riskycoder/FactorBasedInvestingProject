#ifndef VEC_H
#define VEC_H

#include <iostream>
#include <cmath>
#include "definitions.h"

//Vector class declaration
class Vector {
    double vec[SIZE];                                                           // Vector array containing elements of the vector
public:
    Vector(double a, double b);                                                 // Vector Constructor with 'SIZE' individual arguments
    Vector(double pt[SIZE]);                                                    // Vector constructor with array of length 'SIZE' as an argument
    void gradient(Vector& output, double (*fn)(double(&pos)[SIZE]));            // Gradient function stores the gradient of 'fn' at 'this' in 'output'
    void gradient(Vector& output, double (*fn)(double(&pos)[SIZE]), int arg, double scale);   // Gradient function stores the gradient of 'fn' at 'this' in 'output'
    double value(double (*fn)(double(&pos)[SIZE]));                             // Value function returns the value of 'fn' at 'this'
    void normalize();
    friend std::ostream& operator<<(std::ostream& out, const Vector& vec);      // Overridden print operator
    Vector operator*(double const& rs);                                         // Overridden * operator 
    Vector& operator+=(Vector const& rs);                                       // Overridden + operator
    Vector& operator=(Vector const& rs);										// Overridden = operator
};

std::ostream& operator<<(std::ostream& out, const Vector& vec);

#endif