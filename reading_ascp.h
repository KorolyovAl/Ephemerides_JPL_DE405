#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cmath>
#include <iomanip>
#include <thread>
#include <future>

#include "structures.h"
#include "functions.h"

void CalculateSingleThread(double md0, vector<vector<double>>& koeff, int block_number, int Interval,
	map<double, vector<double>>& Obj, int lenght_obj, vector<int>& body, ofstream& obj) {
	for (double MD = md0; MD < koeff[block_number - 2][1]; MD += 0.25) {
		int numer_block = (MD - md0) / Interval + 1;
		string date_check = Get_gregorian_date(MD);
		//cout << date_check << endl;

		Obj[MD] = Calc_coord(md0, MD, body, lenght_obj, koeff[numer_block - 1], numer_block);
		PrintIntoFile(obj, Obj, MD);
	}
}

void Get_positionSingleThread(double md0, vector<vector<double>>& koeff, int block_number, int Interval, 
	map<double, vector<double>>& Obj, map<double, vector<double>>& Sun, map<double, vector<double>>& Earth, ofstream& obj) {
	// returned the vector from body_2 to body_1
	for (double MD = md0; MD < koeff[block_number - 2][1]; MD += 0.25) {
		int numer_block = (MD - md0) / Interval + 1;
		string date_check = Get_gregorian_date(MD);
		//cout << date_check << endl;

		Obj[MD] = Get_position(Sun[MD], Earth[MD]);
		PrintIntoFile(obj, Obj, MD);
	}
}

void Get_position_earthSingleThread(double md0, vector<vector<double>>& koeff, int block_number, int Interval, 
	map<double, vector<double>>& Obj, map<double, vector<double>>& EM_barycenter, map<double, vector<double>>& Moon, ofstream& obj, double emrat) {
	// returned the vector from body_2 to body_1
	for (double MD = md0; MD < koeff[block_number - 2][1]; MD += 0.25) {
		int numer_block = (MD - md0) / Interval + 1;
		string date_check = Get_gregorian_date(MD);
		//cout << date_check << endl;

		Obj[MD] = Get_position_earth(EM_barycenter[MD], Moon[MD], emrat);
		PrintIntoFile(obj, Obj, MD);
	}
}

/*
 * 2.  Reading files by date and writing to array
 */
void Reading_ascp(const int initial_date, const int ending_date, const int ncoeff,
	double emrat, Planets_structure& struct_1
) {
	ofstream moon("results\\Moon_geocenter.txt");
	ofstream em("results\\EarthMoon_barycenter.txt");
	ofstream sun("results\\Sun.txt");
	ofstream se("results\\Sun_Earth.txt");
	ofstream earth("results\\Earth.txt");

	int block_counter = 0;
	for (int type_file = initial_date; type_file <= ending_date; type_file += 20) {

		block_counter++;
		std::cout << "Current calculating date = " << type_file << " Ending date = " << ending_date << endl;

		string file_name = "files\\ascp"; // path to the ascp file
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
			std::cout << "File ascp is open" << endl;
		}
		else {
			cerr << "File aspc is not open" << endl;
			throw 1;
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

		string buffer;

		while (!ascp.eof()) {
			block_number++;
			ascp >> buffer;
			//std::cout << buffer << endl;
			ascp >> buffer;
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
		}

		//	double MD = 57390.2299558333 + 2400000.5;
		//	double MD = 2458850.5;
		double md0 = koeff[0][0];
		int Interval = 32;

		thread EM_barycenter_thread(CalculateSingleThread, md0, ref(koeff), block_number, Interval, ref(EM_barycenter), lenght_EM, ref(struct_1.EarthMoon_barycenter), ref(em));
		thread Sun_thread(CalculateSingleThread, md0, ref(koeff), block_number, Interval, ref(Sun), lenght_Sun, ref(struct_1.Sun), ref(sun));
		thread Moon_thread(CalculateSingleThread, md0, ref(koeff), block_number, Interval, ref(Moon), lenght_Moon, ref(struct_1.Moon_geocentric), ref(moon));
		
		EM_barycenter_thread.join();
		Sun_thread.join();
		Moon_thread.join();

		thread Earth_thread(Get_position_earthSingleThread, md0, ref(koeff), block_number, Interval, ref(Earth), ref(EM_barycenter), ref(Moon),
			ref(earth), emrat);
		this_thread::sleep_for(chrono::milliseconds(500));		// it's not good way

		for (double MD = md0; MD < koeff[block_number - 2][1]; MD += 0.25) {
			int numer_block = (MD - md0) / Interval + 1;
			string date_check = Get_gregorian_date(MD);

			//Earth[MD] = Get_position_earth(EM_barycenter[MD], Moon[MD], emrat);
			SunEarth[MD] = Get_position(Sun[MD], Earth[MD]);

			//PrintIntoFile(earth, Earth, MD);
			PrintIntoFile(se, SunEarth, MD);			
		}

		Earth_thread.join();

		ascp.close();

	}

	moon.close();
	sun.close();
	em.close();
	se.close();
	earth.close();
}