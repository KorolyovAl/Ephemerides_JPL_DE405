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
#include "reading_header.h"

using namespace std;
using namespace std::chrono;



int main() {
	auto start = steady_clock::now();

	Planets_structure struct_1;
	int ncoeff = 0;
	double emrat = 0;

	/* 
	 * 1. Reading the hiderfile and extracting the neccecary data:
	 * ncoef, group 1030, group 1050.
	 */

	Reading_header("files\\header.405", struct_1, emrat, ncoeff);	
	
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
