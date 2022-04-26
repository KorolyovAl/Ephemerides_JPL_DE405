#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cmath>
#include <iomanip>
#include <thread>
#include <chrono>

#include "structures.h"
#include "functions.h"
#include "reading_ascp.h"

using namespace std;
using namespace std::chrono;



int main() {
	auto start = steady_clock::now();
	/* 
	 * 1. Reading the hiderfile and extracting the neccecary data:
	 * ncoef, group 1030, group 1050.
	 */
	Planets_structure struct_1;
	string buffer;
	int ncoeff = 0;
	double julian_start, julian_end;

	ifstream header("files\\header.405");

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
	Reading_ascp(1940, 2100, ncoeff, emrat, struct_1);

	auto end = steady_clock::now();
	auto elapsed = duration_cast<milliseconds>(end - start);
	ofstream time_count("results\\time.txt");
	time_count << elapsed.count() << " ms";
	time_count.close();

	cout << "done!";

	return 0;
}
