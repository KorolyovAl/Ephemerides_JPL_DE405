#include "ephemerides.h"

void Ephemerides_start()
{
	auto start = steady_clock::now();

	Ephemerides eph;
	Initialization(eph);
	Reading_ascp(eph);
	Run_calculating_coordinates(eph);

	auto end = steady_clock::now();
	auto elapsed = duration_cast<milliseconds>(end - start);
	std::ofstream time_count("results\\time.txt");
	time_count << elapsed.count() << " ms";
	time_count.close();
}