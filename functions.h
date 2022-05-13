#pragma once
#include "structures.h"

using namespace std;
/*
 * функция перевода из юлианского формата в григорианский
 * Function for converting Julian date to Gregorian date
 */
std::string Get_gregorian_date(double j_date) {
	int a = j_date + 32'044;
	int b = (4 * a + 3) / 146'097;
	int c = a - (int)((146'097 * b) / 4);
	int d = (4 * c + 3) / 1'461;
	int e = c - (int)((1461 * d) / 4);
	int m = (5 * e + 2) / 153;

	int day = e - (int)((153 * m + 2) / 5) + 1;
	int month = m + 3 - 12 * (int)(m / 10);
	int year = 100 * b + d - 4800 + (int)(m / 10);

	stringstream ss;
	ss << day << '.' << month << '.' << year;
	string tmp;
	ss >> tmp;
	return tmp;
}

/*
 * функция перевода из григорианского формата в юлианский
 * Function for converting Gregorian date to Julian date
 */
void Get_julian_date(int year, int month, int day, int hour, int minute) {
	double a = (14 - month) / 12;
	double y = year + 4'800 - a;
	double m = month + 12 * a - 3;
	double jdn = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32'045;
	double jd = jdn + (hour - 12) / 24 + minute / 1'440;
}

vector<double> Calc_coord(double local_start_j, double time, std::vector<int>& body, int lenght,
	std::vector<double>& coeff, int block_number) {

	// local_start_j - initial Julian date of file
	// time - the desired date on which to calculate the ephemeris
	// body - three numbers for concrete planet, where [0] - serial number of initial coefficient, [1] - number of coefficients for each coordinate, 
	//		[2] - number of subintervals
	// lenght - number of coefficients for concrete planet
	//coeff - vector of coefficients

	double MD0 = local_start_j - 2400000.5;
	double MD = time - 2400000.5;
	int interval = 32; // the lenght of interval in days
	int NSetBody = body[2]; // number of subintervals for concrete planet
	int subInterval = interval / NSetBody; // the lenght of subintervals in days
	double MD1 = MD0 + (block_number - 1) * interval; // initial date of desired block
	int podint = (MD - MD1) / subInterval; // the number of full subintervals in the block until the desired date
	double Mdat = MD1 + podint * subInterval; // initial date of desired subinterval
	int IndBegin = 3 * body[1] * podint; // shift of the beginning of reading coefficients taking into account the subinterval

	int lenght_sub = lenght / NSetBody; // the number of coefficients in the subterval
	int start_point = body[0] + IndBegin; // start point in vector

	std::vector<double> kx, ky, kz;

	for (int i = 0; i < lenght_sub; i++) {
		if (i < lenght_sub / 3.) {
			kx.push_back(coeff[start_point + i - 1]);
			//			cout << " X: " << coeff[start_point + i - 1] << "    ";
		}
		else if (i < 2 * lenght_sub / 3.) {
			ky.push_back(coeff[start_point + i - 1]);
			//			cout << " Y: " << coeff[start_point + i - 1] << "    ";
		}
		else {
			kz.push_back(coeff[start_point + i - 1]);
			//			cout << " Z: " << coeff[start_point + i - 1] << "    ";
		}
	}
	double tau = (2. * (MD - Mdat) / subInterval) - 1.;
	std::vector<double> p(body[1]);
	p[0] = 1.;
	p[1] = tau;
	for (unsigned i = 2; i < p.size(); i++) {
		p[i] = 2 * tau * p[i - 1] - p[i - 2];
	}

	std::vector<double> pt(body[1]);
	pt[0] = 0.;
	pt[1] = 1.;
	//pt(i) = 2 * (p(i - 1) + tau * pt(i - 1)) - pt(i - 2)
	for (unsigned i = 2; i < p.size(); i++) {
		pt[i] = 2 * (p[i - 1] + tau * pt[i - 1]) - pt[i - 2];
	}

	double X = 0., Y = 0., Z = 0.;
	for (int i = p.size() - 1; i >= 0; i--) {
		X += kx[i] * p[i];
		Y += ky[i] * p[i];
		Z += kz[i] * p[i];
	}

	double VX = 0., VY = 0., VZ = 0.;
	double koefVelocity = subInterval / 2 * 86'400;

	for (int i = pt.size() - 1; i >= 0; i--) {
		VX += kx[i] * pt[i];
		VY += ky[i] * pt[i];
		VZ += kz[i] * pt[i];
	}

	return { X, Y, Z, VX / koefVelocity, VY / koefVelocity, VZ / koefVelocity };
}

std::vector<double> Get_position(std::vector<double> body_1, std::vector<double> body_2) {
	// returned the vector from body_2 to body_1
	return { body_1[0] - body_2[0],
		body_1[1] - body_2[1],
		body_1[2] - body_2[2],
		body_1[3] - body_2[3],
		body_1[4] - body_2[4],
		body_1[5] - body_2[5] };
}

std::vector<double> Get_position_earth(std::vector<double> body_1, std::vector<double> body_2, double emrat) {
	// returned the vector from body_2 to body_1
	return { body_1[0] - body_2[0] / (1 + emrat),
		body_1[1] - body_2[1] / (1 + emrat),
		body_1[2] - body_2[2] / (1 + emrat),
		body_1[3] - body_2[3] / (1 + emrat),
		body_1[4] - body_2[4] / (1 + emrat),
		body_1[5] - body_2[5] / (1 + emrat) };
}

void PrintIntoFile(std::ofstream& stream, std::map<double, std::vector<double>>& body, const double MD) {
	stream << "date = " << left << scientific << setw(12) << setprecision(8) << MD
		<< " X = " << setw(26) << setprecision(18) << body[MD][0]
		<< " Y = " << setw(26) << setprecision(18) << body[MD][1]
		<< " Z = " << setw(26) << setprecision(18) << body[MD][2]
		<< " R = " << setw(26) << sqrt(body[MD][0] * body[MD][0] + body[MD][1] * body[MD][1] + body[MD][2] * body[MD][2])
		<< " VX = " << setw(15) << setprecision(6) << body[MD][3]
		<< " VY = " << setw(15) << setprecision(6) << body[MD][4]
		<< " VZ = " << setw(15) << setprecision(6) << body[MD][5]
		<< '\n';
}