#define PROB 2

#if PROB == 1

#include <iostream>
#include <random>
#include <array>
#include <limits>
#define SCALE 4

std::random_device rd;
std::default_random_engine dre{ rd()};
std::uniform_int_distribution uid{ 1, 9 };


template<class ARR>
int determinate(ARR mat);


class Matrix {
	std::array<int, SCALE*SCALE> matrix{};
	std::array<int, SCALE*SCALE> eCapture{}, fCapture{};
	std::array<int, SCALE*SCALE> operand{};
	std::array<int, SCALE*SCALE> result{};

public:
	Matrix();

	bool command(const char& c);
	void command(std::array<int, SCALE* SCALE>& arr);

	bool printMat();

	bool printOpnd();

	bool MatComp();

	bool MatAdd();

	bool MatSub();

	std::array<int, 2> MatDet();

	bool MatTransp();

	bool MatMinRowSub();

	bool MatMaxColAdd();

	bool PlusMod();
	bool SubMod();
};



Matrix::Matrix() {
	for (int& a : matrix) {
		a = uid(dre);
	}
}



bool Matrix::command(const char& c) {
	switch (c) {
	
	case 'e':
		MatMaxColAdd();
		break;
	case 'f':
		MatMinRowSub();
		break;
	case '+':
		PlusMod();
		break;
	case '-':
		SubMod();
		break;

	default:
		std::cout << "command not found\n";
		break;

	}
	return true;
}



void Matrix::command(std::array<int, SCALE* SCALE>& arr) {

	operand = arr;
	char c;
	bool isContinue = true;
	while (1) {
		std::cout << "please enter command: ";
		std::cin >> c;
		std::cout << '\n';

		switch (c) {
		case 't':
			MatTransp();
			break;

		case 'm':
			MatComp();
			break;

		case 'a':
			MatAdd();
			break;

		case 'd':
			MatSub();
			break;

		case 'r':
			MatDet();
			break;

		case 's':
			for (int& a : matrix) {
				a = uid(dre);
			}
			break;

		case 'p':
			printMat();
			break;

		case 'q':
			return;

		default:
			command(c);
			break;

		}
	}
}

bool Matrix::printMat() {
	short counter = 0;

	for (int& a : matrix) {
		std::cout << a << ' ';
		counter++;
		if (counter % SCALE == 0) {
			std::cout << '\n';
			counter = 0;
		}
	}

	return true;
}


bool Matrix::printOpnd() {
	short counter = 0;

	for (int& a : operand) {
		std::cout << a << ' ';
		counter++;
		if (counter % SCALE == 0) {
			std::cout << '\n';
			counter = 0;
		}
	}

	return true;
}


bool Matrix::MatComp() {
	int col = 0, row = 0;
	

	for (int& b : result) {
		for (int i = 0; i < SCALE -1; ++i)
			b += matrix[i + row* SCALE] * operand[i * SCALE + col];

		++col;
		if (col % SCALE == 0) { col = 0; ++row; }
	}
	return true;
}





bool Matrix::MatAdd() {
	for (int i = 0; i < SCALE * SCALE; ++i)
		result[i] = matrix[i] + operand[i];
	return true;
}





bool Matrix::MatSub() {
	for (int i = 0; i < SCALE * SCALE; ++i)
		result[i] = matrix[i] - operand[i];
	return true;
}

std::array<int, 2> Matrix::MatDet() {
	std::array<int, 2> matdet{};
	matdet[0] = determinate(matrix);
	matdet[1] = determinate(operand);
	std::cout << "mat1 determine: " << matdet[0] << std::endl;
	std::cout << "mat2 determine: " << matdet[1] << std::endl;

	return matdet;
}







template<class ARR>
int determinate(ARR mat) {

	if (scale == 2) {
		return (mat[0] * mat[3] - mat[1] * mat[2]);
	}

	int det = 0;
	for (int row = 0; row < SCALE; ++row) {
		int counter = 0;
		std::array<int, SCALE* SCALE> arr{};

		for (int i = 1; i < SCALE; ++i)
			for (int j = 0; j < SCALE; ++j) {
				if (j == row) continue;
				
				arr[counter] = mat[j * SCALE + i];
				++counter;
			}

		if(row%2 == 0)
			det += mat[row] * determinate(arr, SCALE - 1);
		else
			det += -mat[row] * determinate(arr, SCALE - 1);
	}
	return det;
}







bool Matrix::MatTransp() { //scale = 4

	MatDet();

	for (int i = 0; i < SCALE; ++i)
		for (int j = i + 1; j < SCALE; ++j) {
			std::swap(matrix[i * SCALE + j], matrix[j * SCALE + i]);
			std::swap(operand[i * SCALE + j], operand[j * SCALE + i]);
		}

	printMat();
	printOpnd();
	
	return true;
}


bool Matrix::MatMinRowSub() {
	static bool isE = false;
	if (isE) {
		matrix = eCapture;
		return true;
	}
	eCapture = matrix;
	
	for (int i = 0; i < SCALE; ++i) {
		int min = std::numeric_limits<int>::max();
		for (int j = 0; j < SCALE; ++j) {
			if (min > matrix[i * SCALE + j])
				min = matrix[i * SCALE + j];
		}
		for (int j = 0; j < SCALE; ++j)
			matrix[i * SCALE + j] -= min;
	}
	

	return true;
}



bool Matrix::MatMaxColAdd() {
	static bool isF = false;
	if (isF) {
		matrix = fCapture;
		return true;
	}

	fCapture = matrix;

	for (int i = 0; i < SCALE; ++i) {
		int max = std::numeric_limits<int>::min();
		for (int j = 0; j < SCALE; ++j) {
			if (max < matrix[j * SCALE + i])
				max = matrix[j * SCALE + i];
		}
		for (int j = 0; j < SCALE; ++j)
			matrix[j * SCALE + i] += max;
	}

	return true;
}

bool Matrix::PlusMod() {
	for (int& a : matrix)
		a = (a + 1) % 10;
	return true;
}

bool Matrix::SubMod() {
	for (int& a : matrix) 
		a = (a - 1 + 10) % 10;
	return true;
}



int main()
{
	Matrix matrix{};
	std::array<int, SCALE* SCALE> opnd;
	for (int& a : opnd) {
		a = uid(dre);
	}


	matrix.command(opnd);



}


#elif PROB == 2

#include <iostream>
#include <fstream>
#include <string>




class StringEditor {
	std::string origin;
	std::string aCapture;


public:

};

int main()
{

}

#endif
