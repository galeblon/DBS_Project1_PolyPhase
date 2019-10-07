/*
 * Program generating records in form of Coefficients of polynomial
 * W(x) = ax^2 + bx + c, where delta >= 0)
 * Usage:
 * ./program [num_of_rows] [filename]
 * num_of_rows is number of records to generate, filename is name of the file to save records to.
 * If parameters are not provided, user is asked to provide them during program.
 * */

#include<ctime>
#include<iostream>
#include<fstream>

#define COEFFICIENT_MAX 50

int generateRecords(std::string resultFileName, int records);
int generateRecord(std::fstream& fs);

int main(int argc, char* argv[]){
	std::srand(std::time(NULL));
	int recordsToGenerate;
	std::string fileName;
	int name_index = 1;
	if(argc > 1){
		try{
			recordsToGenerate = std::stoi(argv[1]);
			name_index++;
		} catch(const std::exception &){
			std::cerr << "Provided invalid number of records to generate";
			return 1;
		}
	} else {
		std::cout << "Provide number of records to generate: ";
		std::cin >> recordsToGenerate;
	}
	if(name_index < argc){
		fileName = std::string(argv[name_index]);
	} else{
		std::cout << "Provide name of the file to save records to: ";
		std::cin >> fileName;
	}

	return generateRecords(fileName, recordsToGenerate);
}


int generateRecords(std::string resultFileName, int records){
	std::fstream fs = std::fstream();
	fs.open(resultFileName, std::fstream::out);
	for(int i=0; i<records;){
		i += generateRecord(fs);
	}
	fs.flush();
	fs.close();
	return 0;
}

int generateRecord(std::fstream& fs){
	double a = (double)std::rand() / RAND_MAX * COEFFICIENT_MAX;
	double b = (double)std::rand() / RAND_MAX * COEFFICIENT_MAX;
	double c = (double)std::rand() / RAND_MAX * COEFFICIENT_MAX;
	if((b*b - 4*a*c) >= 0){
		fs << a << " " << b << " " << c << '\n';
		return 1;
	}
	return 0;
}
