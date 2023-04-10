#include "initialization.h"

void Read_init_data(Ephemerides& tmp)
{
	std::ifstream indat("files//Initial_data.dat");

	if (indat.is_open()) {
		std::cout << "Initial data file is open" << std::endl;
	}
	else {
		std::cout << "Initial data file is not open" << std::endl;
	}

	while (!indat.eof()) {
		std::string buffer;
		indat >> buffer;

		if (buffer == "BODIES:") {
			while (buffer != "DATE:") {
				indat >> buffer;
				if (buffer == "DATE:") { break; }
				Space_object so;
				so.object_name = buffer;
				indat >> std::boolalpha >> so.object_state;
				tmp.bodies.push_back(std::move(so));
			}
		}
		
		if (buffer == "DATE:") {
			indat >> buffer;
			if (buffer == "Start_year") { indat >> tmp.start_year; }

			indat >> buffer;
			if (buffer == "End_year") { indat >> tmp.end_year; }
		}
	}
	indat.close();
}

void Read_header_ascp(Ephemerides& tmp)
{
	std::string buffer;
	std::ifstream header("files\\header.405");

	if (header.is_open()) {
		std::cout << "File header is open" << std::endl;
	}
	else {
		std::cerr << "File header is not open" << std::endl;
		/*
			выкинуть исключение
		*/
	}

	int number_emrat = 0;

	while (!header.eof()) {
		header >> buffer;
		if (buffer == "NCOEFF=") {
			buffer.clear();
			header >> tmp.ncoeff;
		}

		if (buffer == "GROUP") {
			buffer.clear();
			header >> buffer;

			if (buffer == "1030") {
				header >> tmp.julian_start;
				header >> tmp.julian_end;
				header >> tmp.interval;
			}
			else if (buffer == "1040") {
				int size = 0;
				header >> size;
				for (int i = 0; i < size; i++) {
					std::string tmp;
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
				header >> buffer;
				auto it = buffer.end() - 4.;
				*it = 'E';
				std::stringstream ss(buffer);
				ss >> tmp.emrat;
			}
			else if (buffer == "1050") {
				for (int i = 0; i < 3; i++) {

					for (int i = 0; i < tmp.bodies.size(); i++) {
						int buf;
						header >> buf;
						tmp.bodies[i].object_header_data.push_back(buf);
					}
				}
			}
		}

		buffer.clear();
	}
}

void Calculate_number_of_elements(Ephemerides& eph) {
	for (int i = 0; i < eph.bodies.size() - 1; i++) {
		eph.bodies[i].object_lenght = eph.bodies[i + 1].object_header_data[0] - eph.bodies[i].object_header_data[0];
	}
	eph.bodies[eph.bodies.size() - 1].object_lenght = eph.ncoeff - eph.bodies[eph.bodies.size() - 1].object_header_data[0];
}

void Initialization(Ephemerides& eph)
{
	Read_init_data(eph);
	Read_header_ascp(eph);
	Calculate_number_of_elements(eph);
}
