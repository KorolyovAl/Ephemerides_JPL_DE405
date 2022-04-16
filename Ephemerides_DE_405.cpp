#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;

class Planets_structure {
public:
	vector<int> Mercury;
	vector<int> Venus;
	vector<int> EarthMoon_barycenter;
	vector<int> Mars;
	vector<int> Jupiter;
	vector<int> Saturn;
	vector<int> Uranus;
	vector<int> Neptune;
	vector<int> Pluto;
	vector<int> Moon_geocentric;
	vector<int> Sun;
	vector<int> Earth_Nutations;
	vector<int> Lunar_mantle_libration;
};

class Date {
public:
	double j_date;
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
};

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

/*
 * функция перевода из юлианского формата в григорианский
 * Function for converting Julian date to Gregorian date
 */
string Get_gregorian_date(double j_date) {
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

map<double, vector<double>> Calc_coord(double local_start_j, double time, vector<int>& body, int lenght, vector<double>& coeff, int block_number) {
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

	vector<double> kx, ky, kz;

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
	vector<double> p(body[1]);
	p[0] = 1.;
	p[1] = tau;
	for (unsigned i = 2; i < p.size(); i++) {
		p[i] = 2 * tau * p[i - 1] - p[i - 2];
	}

	vector<double> pt(body[1]);
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
	double koefVelocity = subInterval / 2 * 86400;

	for (int i = pt.size() - 1; i >= 0; i--) {
		VX += kx[i] * pt[i];
		VY += ky[i] * pt[i];
		VZ += kz[i] * pt[i];
	}

	map<double, vector<double>> m1;
	m1[MD + 2400000.5] = {X, Y, Z, VX / koefVelocity, VY / koefVelocity, VZ / koefVelocity };

	return m1;
}

vector<double> Get_position(vector<double> body_1, vector<double> body_2) {
	// returned the vector from body_2 to body_1
	return { body_1[0] - body_2[0], 
		body_1[1] - body_2[1], 
		body_1[2] - body_2[2],
		body_1[3] - body_2[3], 
		body_1[4] - body_2[4], 
		body_1[5] - body_2[5]  };
}

vector<double> Get_position_earth(vector<double> body_1, vector<double> body_2, double emrat) {
	// returned the vector from body_2 to body_1
	return { body_1[0] - body_2[0] / (1 + emrat), 
		body_1[1] - body_2[1] / (1 + emrat), 
		body_1[2] - body_2[2] / (1 + emrat), 
		body_1[3] - body_2[3] / (1 + emrat),
		body_1[4] - body_2[4] / (1 + emrat),
		body_1[5] - body_2[5] / (1 + emrat) };
}

void PrintIntoFile(ofstream& stream, map<double, vector<double>>& body, const double MD) {
	stream << "date = " << left << scientific << setw(12) << setprecision(8) << MD
		<< " X = "  << setw(26) << setprecision(18) << body[MD][0]
		<< " Y = "  << setw(26) << setprecision(18) << body[MD][1]
		<< " Z = "  << setw(26) << setprecision(18) << body[MD][2]
		<< " R = "  << setw(26) << sqrt(body[MD][0] * body[MD][0] + body[MD][1] * body[MD][1] + body[MD][2] * body[MD][2])
		<< " VX = " << setw(15) << setprecision(6) << body[MD][3]
		<< " VY = " << setw(15) << setprecision(6) << body[MD][4]
		<< " VZ = " << setw(15) << setprecision(6) << body[MD][5]
		<< '\n';
}

int main() {
	/* 
	 * 1. Reading the hiderfile and extracting the neccecary data:
	 * ncoef, group 1030, group 1050.
	 */
	Planets_structure struct_1;
	string buffer;
	int ncoeff = 0;
	double julian_start, julian_end;

	ifstream header("header.405");

	if (header.is_open()) {
		cout << "File header is open" << endl;
	}
	else {
		cerr << "File header is not open" << endl;
		return 1;
	}

	int number_emrat = 0;
	double emrat = 0;

	while (!header.eof()) {
		header >> buffer;
		if (buffer == "NCOEFF=") {
			buffer.clear();
			header >> ncoeff;
		}

		if (buffer == "GROUP") {
			buffer.clear();
			header >> buffer;

			if (buffer == "1030") {
				header >> julian_start;
				header >> julian_end;
			}
			else if (buffer == "1040") {
				int size = 0;
				header >> size;
				for (int i = 0; i < size; i++) {
					string tmp;
					header >> tmp;
					if (tmp == "EMRAT") {
						number_emrat = i;
						break;
					}
					tmp.clear();
					buffer.clear();
				}
			}
			else if (buffer == "1041") {
				for (int i = 0; i <= number_emrat; i++) {
					header >> buffer;
					buffer.clear();
				}
				header >> emrat;
			}
			else if (buffer == "1050") {
				for (int i = 0; i < 3; i++) {
					int buf;
					header >> buf;
					struct_1.Mercury.push_back(buf);

					header >> buf;
					struct_1.Venus.push_back(buf);

					header >> buf;
					struct_1.EarthMoon_barycenter.push_back(buf);

					header >> buf;
					struct_1.Mars.push_back(buf);

					header >> buf;
					struct_1.Jupiter.push_back(buf);

					header >> buf;
					struct_1.Saturn.push_back(buf);

					header >> buf;
					struct_1.Uranus.push_back(buf);

					header >> buf;
					struct_1.Neptune.push_back(buf);

					header >> buf;
					struct_1.Pluto.push_back(buf);

					header >> buf;
					struct_1.Moon_geocentric.push_back(buf);

					header >> buf;
					struct_1.Sun.push_back(buf);

					header >> buf;
					struct_1.Earth_Nutations.push_back(buf);

					header >> buf;
					struct_1.Lunar_mantle_libration.push_back(buf);
				}
			}
		}

		buffer.clear();
	}

	header.close();

	/*
	 * 2.  Reading files by date and writing to array
	 */
	ofstream moon("results\\Moon_geocenter.txt");
	ofstream em("results\\EarthMoon_barycenter.txt");
	ofstream sun("results\\Sun.txt");
	ofstream se("results\\Sun_Earth.txt");
	ofstream earth("results\\Earth.txt");

	for (int type_file = 1940; type_file <= 2100; type_file += 20) {

		string file_name = "D:\\Work\\CubeSat\\Ephemerides_DE_405\\Ephemerides_DE_405\\files\\ascp"; // path to the ascp file
		string end_file_name = ".405";

		string full_name = file_name;
		stringstream fs;
		string buf;
		fs << type_file;
		fs >> buf;
		fs.clear();
		full_name += buf + end_file_name;
		buf.clear();

		ifstream ascp(full_name);

		//	ofstream ascp_re("ascp_rewrite.txt");

		if (ascp.is_open()) {
			cout << "File ascp is open" << endl;
		}
		else {
			cerr << "File aspc is not open" << endl;
			return 1;
		}
		vector<double> block(ncoeff + 2);
		vector<vector<double> > koeff(250, vector<double>(ncoeff + 2));
		map<double, vector<double>> Moon;
		map<double, vector<double>> EM_barycenter;
		map<double, vector<double>> Sun;
		map<double, vector<double>> SunEarth;
		map<double, vector<double>> Earth;
		int lenght_Moon = struct_1.Sun[0] - struct_1.Moon_geocentric[0];
		int lenght_EM = struct_1.Mars[0] - struct_1.EarthMoon_barycenter[0];
		int lenght_Sun = struct_1.Earth_Nutations[0] - struct_1.Sun[0];
		int block_number = 0;

		buffer.clear();

		while (!ascp.eof()) {
			block_number++;
			ascp >> buffer;
			cout << buffer << endl;
			ascp >> buffer;
			//		ascp_re << buffer << endl;
			buffer.clear();

			for (int i = 0; i < ncoeff + 2; i++) { // Filling vector with coefficients
				ascp >> buffer;
				if (!buffer.empty()) {
					auto it = buffer.end() - 4.;
					*it = 'E';
					stringstream ss(buffer);
					ss >> koeff[block_number - 1][i];
					buffer.clear();
				}
			}
			/*
			int count = 0;
			for (auto x : block) {
				ascp_re << scientific << setw(26) << setprecision(18) << x;
				count++;

				if (count == 3) {
					count = 0;
					ascp_re << endl;
				}
			}*/
		}

		//	double MD = 57390.2299558333 + 2400000.5;
		//	double MD = 2458850.5;
		double md0 = koeff[0][0];
		int Interval = 32;

		for (double MD = md0; MD < koeff[block_number - 2][1]; MD += 0.25) {
			int numer_block = (MD - md0) / Interval + 1;
			string date_check = Get_gregorian_date(MD);
			cout << date_check << endl;

			EM_barycenter = Calc_coord(md0, MD, struct_1.EarthMoon_barycenter, lenght_EM, koeff[numer_block - 1], numer_block);
			Sun = Calc_coord(md0, MD, struct_1.Sun, lenght_Sun, koeff[numer_block - 1], numer_block);
			Moon = Calc_coord(md0, MD, struct_1.Moon_geocentric, lenght_Moon, koeff[numer_block - 1], numer_block);

			Earth[MD] = Get_position_earth(EM_barycenter[MD], Moon[MD], emrat);
			SunEarth[MD] = Get_position(Sun[MD], Earth[MD]);

			PrintIntoFile(em, EM_barycenter, MD);
			PrintIntoFile(sun, Sun, MD);
			PrintIntoFile(moon, Moon, MD);
			PrintIntoFile(se, SunEarth, MD);
			PrintIntoFile(earth, Earth, MD);
		}

		ascp.close();

	}

	moon.close();
	sun.close();
	em.close();
	//	ascp_re.close();

	cout << "done!";

	return 0;
}
