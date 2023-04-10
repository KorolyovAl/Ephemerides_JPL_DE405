#include "calculating_coordinates.h"

void CalculateSingleThread(Ephemerides& eph, int i) {
	
	std::vector<std::vector<double>>& coef = eph.coef;
	std::map<double, std::vector<double>>& Obj = eph.bodies[i].object_data;
	std::vector<int>& object_header_data = eph.bodies[i].object_header_data;
	double md0 = eph.coef[0][0];

	for (double MD = md0; MD < coef[eph.blocks_number - 2][1]; MD += 0.25) {
		int numer_block = (MD - md0) / eph.interval + 1;
		std::string date_check = Get_gregorian_date(MD);
		//cout << date_check << endl;
		
		Obj[MD] = Calc_coord(md0, MD, object_header_data, eph.bodies[i].object_lenght, coef[numer_block - 1], numer_block);
		PrintIntoFile(eph.bodies[i].obj_file, Obj, MD);
	}
	eph.bodies[i].Clear_data();
}

void Run_calculating_coordinates(Ephemerides& eph)
{
	std::vector<std::thread> vectorOfThreads;
	vectorOfThreads.reserve(eph.bodies.size());
	for (int i = 0; i < eph.bodies.size(); i++) {
		if (eph.bodies[i].object_state) {
			std::thread th(CalculateSingleThread, std::ref(eph), i); 
			vectorOfThreads.push_back(std::move(th)); // Move all thread objects to vector
		}
	}

	for (std::thread& th : vectorOfThreads) {
		if (th.joinable()) {
			th.join();
		}
	}
}
