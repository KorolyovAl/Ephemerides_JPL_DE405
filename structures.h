#pragma once
#include <vector>
#include <map>
#include <fstream>

class Date {
public:
	double j_date; // юлианская дата, в днях
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;

	Date() {
		j_date = 0.;
		year = 0;
		month = 0;
		day = 0;
		hour = 0;
		minute = 0;
		second = 0;
	}
};

class Space_object {
public:
	bool object_state;
	std::vector<int> object_header_data;
	std::map<double, std::vector<double>> object_data; // key - date; value - coordinates vector
	std::string object_name;
	std::ofstream obj_file;
	int object_lenght;
	bool first_step;

	Space_object() {
		object_state = false;
		first_step = false;
		object_lenght = 0;
	}

	void Clear_data() {
		object_data.clear();
	}
};

class Ephemerides {
public:
	Date date;
	std::vector<Space_object> bodies;
	std::vector<std::vector<double>> coef; // вектор коэффициентов по блокам
	int start_year;	// start calculating date
	int end_year;	// end calculating date
	int ncoeff;
	double emrat;
	int blocks_number;	// number of blocks
	int interval;
	double julian_start; // начальная дата эфемерид в юлианском формате
	double julian_end;	// конечная дата эфемерид в юлианском формате

	Ephemerides() {		
		start_year = 0;
		end_year = 0;
		ncoeff = 0;
		emrat = 0;
		blocks_number = 0;
		interval = 0;
		julian_start = 0;
		julian_end = 0;

		coef = std::vector<std::vector<double>>(230 * 9); // 9 файлов по 230 блоков
		for (auto& x : coef) {
			x = std::vector<double>(1020); // 1020 элементов в блоке
		}
	}
};
