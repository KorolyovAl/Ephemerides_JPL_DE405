#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <string>
#include <iomanip>

#include "structures.h"

/*
 * функция перевода из юлианского формата в григорианский
 * Function for converting Julian date to Gregorian date
 */
std::string Get_gregorian_date(double j_date);

/*
 * функция перевода из григорианского формата в юлианский
 * Function for converting Gregorian date to Julian date
 */
void Get_julian_date(int year, int month, int day, int hour, int minute);

std::vector<double> Calc_coord(double local_start_j, double time, std::vector<int>& body, int lenght, std::vector<double>& coeff, int block_number);

std::vector<double> Get_position(std::vector<double> body_1, std::vector<double> body_2);

std::vector<double> Get_position_earth(std::vector<double> body_1, std::vector<double> body_2, double emrat);

void PrintIntoFile(std::ofstream& stream, std::vector<double>& body, const double MD);