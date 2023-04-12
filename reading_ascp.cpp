#include "reading_ascp.h"

void Reading_ascp(Ephemerides& eph)
{
	for (int i = 0; i < eph.bodies.size(); i++) {
		std::string path = "results\\" + eph.bodies[i].object_name + ".txt";
		eph.bodies[i].obj_file.open(path);
	}

	int block_number = 0;
	double current_date = eph.julian_start; // дата начала нового блока
	double old_date = eph.julian_start; // дата окончани€ предыдущего блока
	for (int type_file = eph.start_year; type_file <= eph.end_year; type_file += 20) {
		std::cout << "Current calculating date = " << type_file << " Ending date = " << eph.end_year << " block number = " << block_number << std::endl;

		std::string file_name = "files\\ascp"; // path to the ascp file
		std::string end_file_name = ".405";

		std::string full_name = std::string(file_name) + std::to_string(type_file) + std::string(end_file_name);
		std::ifstream ascp(full_name);

		if (ascp.is_open()) {
			std::cout << "File ascp is open" << std::endl;
		}
		else {
			std::cerr << "File aspc is not open" << std::endl;
			throw 1;
		}		

		std::string buffer;

		while (!ascp.eof()) { // чтение файла по блокам, запись каждого блока ведЄтс€ в koeff(вектор векторов)			
			ascp >> buffer;
			if (ascp.eof()) { break; }
			
			//std::cout << buffer << endl;
			ascp >> buffer;
			buffer.clear();

			for (int i = 0; i < eph.ncoeff + 2; i++) { // Filling vector with coefficients
				ascp >> buffer;
				if (i == 1) { current_date = eph.coef[block_number][0]; }

				// дл€ выполнени€ записи необходимо, чтобы буффер был заполнен и выполн€лось одно из двух условий (либо первое вхождение, 
				// либо дата начала нового блока была больше чем дата окончани€ предыдущего. ѕоследнее условие дл€ того, чтобы блоки не повтор€лись) 
				if (!buffer.empty() && (i == 0 || current_date > old_date)) {
					auto it = buffer.end() - 4.;
					*it = 'E';
					std::stringstream ss(buffer);
					ss >> eph.coef[block_number][i];
					buffer.clear();
				}
			}
			if (current_date > old_date) {
				block_number++;
				eph.blocks_number = block_number;
				old_date = current_date;
			}			
		}
	}

}
