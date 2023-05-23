#include "vector.h"

Vector::Vector(double a, double b)
    : vec{ a,b }
{}


Vector::Vector(double pt[SIZE])
{
    for (int i = 0; i < SIZE; i++) {
        vec[i] = pt[i];
    }
}

void Vector::gradient(Vector& output, double (*fn)(double(&pos)[SIZE])) {

    double obj = fn(vec);                   //Evaluate 'fn' at 'this' vector

    vec[0] += DLT2;							//Start by moving 'DLT2' along first axis from 'this' vector
    output.vec[0] = fn(vec) - obj;          //Evaluate first entry of the gradient by finding difference in 'fn'
    for (int i = 1; i < SIZE; i++) {
        vec[i - 1] -= DLT2;                 //Move back to starting position along i-1st axis
        vec[i] += DLT2;						//Move 'DLT2' along ith axis from 'this' vector
        output.vec[i] = fn(vec) - obj;      //Evaluate ith entry of the gradient by finding difference in 'fn'
    }
    vec[SIZE - 1] -= DLT2;                  //Ensure Vector::gradient executes without making changes to 'vec'

	for (int i = 0; i < SIZE; i++) {
		output.vec[i] = output.vec[i] > 0 ? output.vec[i] : 0;
	}
}

void Vector::gradient(Vector& output, double(*fn)(double(&pos)[SIZE]), int arg, double scale) {

	double obj = fn(vec);										//Evaluate 'fn' at 'this' vector

	for (int i = 0; i < SIZE; i++) output.vec[i] = 0;

	vec[arg] += DLT2 * scale;											//Start by moving 'DLT2' along first axis from 'this' vector
	output.vec[arg] = fn(vec) - obj > 0? DLT2 : DLT2 * -1;				//Evaluate first entry of the gradient by finding difference in 'fn'
	output.vec[arg] *= scale;
	vec[arg] -= DLT2 * scale;											//Ensure Vector::gradient executes without making changes to 'vec'
}

double Vector::value(double (*fn)(double(&pos)[SIZE])) {
    return fn(vec);
}

void Vector::normalize() {
	double norm = 0;
	for (int i = 0; i < SIZE; i++) {
		norm += pow(vec[i], 2);
	}
	norm = pow(norm, 0.5);
	for (int i = 0; i < SIZE; i++) {
		vec[i] /= norm;
	}
}

Vector Vector::operator*(double const& rs) {
    double vals[SIZE] = { 0 };
    for (int i = 0; i < SIZE; i++) {
        vals[i] = vec[i] * rs;
    }
    return Vector(vals);
}

Vector& Vector::operator+=(Vector const& rs) {
    for (int i = 0; i < SIZE; i++) {
        vec[i] += rs.vec[i];
    }
    return *this;
}

Vector& Vector::operator=(Vector const& rs) {
	if (this == &rs) return *this;

	for (int i = 0; i < SIZE; i++) {
		vec[i] = rs.vec[i];
	}
	return *this;
}

std::ostream& operator<<(std::ostream& out, const Vector& vec) {
	out << '(';
	for (int i = 0; i < SIZE; i++) {
		out << vec.vec[i] << (i == SIZE - 1? ')':',');
	}
    return out;
}