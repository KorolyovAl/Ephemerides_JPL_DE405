#pragma once
#include <vector>

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