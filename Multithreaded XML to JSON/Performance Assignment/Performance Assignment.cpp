// Performance Assignment.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <iomanip>

/// Libraries needed for my test loading of data
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <numeric>
#include <future>
#include <thread>
#include "xmltojson.h"
/// end of libraries needed for my test

class TIMER
{
	LARGE_INTEGER t_;

	__int64 current_time_;

public:
	TIMER()	// Default constructor. Initialises this timer with the current value of the hi-res CPU timer.
	{
		QueryPerformanceCounter(&t_);
		current_time_ = t_.QuadPart;
	}

	TIMER(const TIMER& ct)	// Copy constructor.
	{
		current_time_ = ct.current_time_;
	}

	TIMER& operator=(const TIMER& ct)	// Copy assignment.
	{
		current_time_ = ct.current_time_;
		return *this;
	}

	TIMER& operator=(const __int64& n)	// Overloaded copy assignment.
	{
		current_time_ = n;
		return *this;
	}

	~TIMER() {}		// Destructor.

	static __int64 get_frequency()
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		return frequency.QuadPart;
	}

	__int64 get_time() const
	{
		return current_time_;
	}

	void get_current_time()
	{
		QueryPerformanceCounter(&t_);
		current_time_ = t_.QuadPart;
	}

	inline bool operator==(const TIMER& ct) const
	{
		return current_time_ == ct.current_time_;
	}

	inline bool operator!=(const TIMER& ct) const
	{
		return current_time_ != ct.current_time_;
	}

	__int64 operator-(const TIMER& ct) const		// Subtract a TIMER from this one - return the result.
	{
		return current_time_ - ct.current_time_;
	}

	inline bool operator>(const TIMER& ct) const
	{
		return current_time_ > ct.current_time_;
	}

	inline bool operator<(const TIMER& ct) const
	{
		return current_time_ < ct.current_time_;
	}

	inline bool operator<=(const TIMER& ct) const
	{
		return current_time_ <= ct.current_time_;
	}

	inline bool operator>=(const TIMER& ct) const
	{
		return current_time_ >= ct.current_time_;
	}
};

bool isFinishedReading = false;
std::queue<std::future<std::string>> chunkQueue;
std::ofstream fou("output.json", std::ios::app);

// gets the file size
std::ifstream::pos_type filesize(const char* filename) {
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

void handleWritingToFile() {
	fou << "{\n";
	while (!isFinishedReading) {
		// it wasn't breaking at the end
		if (isFinishedReading && chunkQueue.size() < 1)
			break;
		if (chunkQueue.size() >= 1) {
			for (int i = 0; i < chunkQueue.size(); i++) {
				fou << chunkQueue.front().get();
				chunkQueue.pop();
			}
		}
	}
	fou << "\n}";
	fou.close();
}

int main()
{
	// Application starts here...

	// Time the application's execution time.
	TIMER start;	// DO NOT CHANGE THIS LINE. Timing will start here.

					//--------------------------------------------------------------------------------------
					// Insert your code from here...
	const char* path("K:/Storage/Folders/xml_demo/testdata.xml");
	int bufferSize(0);
	std::string line;
	int size = filesize(path);
	std::ifstream fin(path, std::ifstream::binary);

	// If the file is greater than 204MB then set the buffer size to 40.96MB
	if (size > 204800000) {
		bufferSize = 20480000;
	}
	else {
		bufferSize = size / 10;
	}

	std::vector<char> buffer(bufferSize, 0); //reads only the first 2048000 bytes

	auto writeThread = std::thread(handleWritingToFile);

	// read the data until the end of the file
	while (fin.read(buffer.data(), buffer.size())) {
		std::getline(fin, line); // gets the rest of the line to stop the buffer finishing halfway through a line
		for (int i = 0; i < line.size(); i++) buffer.push_back(line[i]); // makes it easier to parse the xml

		while (chunkQueue.size() > 15) {} // quick fix to stop the program creating a billion threads

		chunkQueue.push(std::async(ConvertXMLToJSON, buffer));
	}

	isFinishedReading = true;

	writeThread.join();

	//-------------------------------------------------------------------------------------------------------
	// How long did it take?...   DO NOT CHANGE FROM HERE...

	TIMER end;

	TIMER elapsed;

	elapsed = end - start;

	__int64 ticks_per_second = start.get_frequency();

	// Display the resulting time...

	double elapsed_seconds = (double)elapsed.get_time() / (double)ticks_per_second;

	std::cout.precision(17);
	std::cout << "Elapsed time (seconds): " << std::fixed << elapsed_seconds;
	std::cout << std::endl;
	std::cout << "Press a key to continue" << std::endl;

	char c;
	std::cin >> c;

	return 0;
}