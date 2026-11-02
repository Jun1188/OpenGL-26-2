// [1번 수정 전 핵심 코드]
/*
#define PROB 2
*/
// [1번 수정 후]
#ifndef PROB
#define PROB 4 // 실행할 문제 번호: 1, 2, 3, 4
#endif

#if PROB == 1

#include <iostream>
#include <random>
#include <array>
#include <limits>
#define SCALE 4

std::random_device rd;
std::default_random_engine dre{rd()};
std::uniform_int_distribution uid{1, 9};

// [1번 수정 전 핵심 코드]
/*
template<class ARR>
int determinate(ARR mat);
*/
// [1번 수정 후]
template <class ARR> long long determinate(const ARR& mat, int scale = SCALE);

class Matrix {
    std::array<int, SCALE * SCALE> matrix{};
    // [1번 수정 전 핵심 코드]
    /*
std::array<int, SCALE*SCALE> eCapture{}, fCapture{};
*/
    // [1번 수정 후]
    std::array<int, SCALE * SCALE> eCapture{}, fCapture{};
    std::array<int, SCALE * SCALE> eOperandCapture{}, fOperandCapture{};
    bool isE = false, isF = false; // 객체별 복원 상태

    void printResult() const {
        std::cout << "Result:\n";

        for (int i = 0; i < SCALE * SCALE; ++i)
            std::cout << result[i] << (i % SCALE == SCALE - 1 ? '\n' : ' ');
    }

    std::array<int, SCALE * SCALE> operand{};
    std::array<int, SCALE * SCALE> result{};

public:
    Matrix();

    bool command(const char& c);

    void command(std::array<int, SCALE * SCALE>& arr);

    bool printMat();

    bool printOpnd();

    bool MatComp();

    bool MatAdd();

    bool MatSub();

    std::array<long long, 2> MatDet();

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

        // [1번 수정 전 핵심 코드]
    /*
case 'e':
		MatMaxColAdd();
		break;
	case 'f':
		MatMinRowSub();
*/
    // [1번 수정 후]
    case 'e':
        MatMinRowSub();
        break;
    case 'f':
        MatMaxColAdd();
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

void Matrix::command(std::array<int, SCALE * SCALE>& arr) {

    operand = arr;

    char c;
    // [1번 수정 전 핵심 코드]
    /*
bool isContinue = true;
	while (1) {
		std::cout << "please enter command: ";
		std::cin >> c;
*/
    // [1번 수정 후]
    printMat();
    printOpnd();

    while (true) {
        std::cout << "m/a/d/r/t/e/f/+/-/s/p/q: ";

        if (!(std::cin >> c))
            return;
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

            // [1번 수정 전 핵심 코드]
        /*
case 's':
			for (int& a : matrix) {
				a = uid(dre);
			}
			break;
*/
        // [1번 수정 후]
        case 's':
            for (int& a : matrix)
                a = uid(dre);

            for (int& a : operand)
                a = uid(dre);
            // [이전 코드] isE = isF = false;
            // [수정 후] s 이후에도 e/f 직전의 복원 지점과 토글 상태를 유지한다.
            printMat();
            printOpnd();
            break;

        case 'p':
            printMat();
            printOpnd();
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
    std::cout << "Matrix 1:\n";

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
    std::cout << "Matrix 2:\n";

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

// [1번 수정 전 핵심 코드]
/*
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
*/
// [1번 수정 후]
bool Matrix::MatComp() {
    result.fill(0);

    for (int row = 0; row < SCALE; ++row)
        for (int col = 0; col < SCALE; ++col)
            for (int k = 0; k < SCALE; ++k)
                result[row * SCALE + col] += matrix[row * SCALE + k] * operand[k * SCALE + col];
    printResult();

    return true;
}

// [1번 수정 전 핵심 코드]
/*
bool Matrix::MatAdd() {
	for (int i = 0; i < SCALE * SCALE; ++i)
		result[i] = matrix[i] + operand[i];
	return true;
}
*/
// [1번 수정 후]
bool Matrix::MatAdd() {
    for (int i = 0; i < SCALE * SCALE; ++i)
        result[i] = matrix[i] + operand[i];
    printResult();

    return true;
}

// [1번 수정 전 핵심 코드]
/*
bool Matrix::MatSub() {
	for (int i = 0; i < SCALE * SCALE; ++i)
		result[i] = matrix[i] - operand[i];
	return true;
}
*/
// [1번 수정 후]
bool Matrix::MatSub() {
    for (int i = 0; i < SCALE * SCALE; ++i)
        result[i] = matrix[i] - operand[i];
    printResult();

    return true;
}

// [1번 수정 전 핵심 코드]
/*
std::array<int, 2> Matrix::MatDet() {
	std::array<int, 2> matdet{};
	matdet[0] = determinate(matrix);
	matdet[1] = determinate(operand);
	std::cout << "mat1 determine: " << matdet[0] << std::endl;
	std::cout << "mat2 determine: " << matdet[1] << std::endl;

	return matdet;
}
*/
// [1번 수정 후]
std::array<long long, 2> Matrix::MatDet() {
    std::array<long long, 2> values{determinate(matrix), determinate(operand)};

    std::cout << "mat1 determinant: " << values[0] << '\n';
    std::cout << "mat2 determinant: " << values[1] << '\n';

    return values;
}

// [1번 수정 전 핵심 코드]
/*
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
*/
// [1번 수정 후]
template <class ARR> long long determinate(const ARR& mat, int scale) {
    if (scale == 1)
        return mat[0];

    if (scale == 2)
        return 1LL * mat[0] * mat[3] - 1LL * mat[1] * mat[2];

    long long det = 0;

    for (int col = 0; col < scale; ++col) {
        std::array<int, SCALE * SCALE> minor{};
        int index = 0;

        for (int row = 1; row < scale; ++row)
            for (int j = 0; j < scale; ++j)
                if (j != col)
                    minor[index++] = mat[row * scale + j];
        det += (col % 2 == 0 ? 1LL : -1LL) * mat[col] * determinate(minor, scale - 1);
    }

    return det;
}

// [1번 수정 전 핵심 코드]
/*
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
*/
// [1번 수정 후]
bool Matrix::MatTransp() {
    // 현재 행렬만 전치한다. e/f 실행 직전의 스냅샷은 전치하지 않는다.
    for (int i = 0; i < SCALE; ++i)
        for (int j = i + 1; j < SCALE; ++j) {
            std::swap(matrix[i * SCALE + j], matrix[j * SCALE + i]);
            std::swap(operand[i * SCALE + j], operand[j * SCALE + i]);
        }
    printMat();
    printOpnd();
    MatDet();

    return true;
}

// [1번 수정 전 핵심 코드]
/*
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
*/
// [1번 수정 후]
bool Matrix::MatMinRowSub() {
    if (isE) {
        matrix = eCapture;
        operand = eOperandCapture;
        isE = false;
    } else {
        eCapture = matrix;
        eOperandCapture = operand;

        for (auto* mat : {&matrix, &operand}) {
            for (int i = 0; i < SCALE; ++i) {
                int value = std::numeric_limits<int>::max();

                for (int j = 0; j < SCALE; ++j)
                    if ((*mat)[i * SCALE + j] < value)
                        value = (*mat)[i * SCALE + j];

                for (int j = 0; j < SCALE; ++j)
                    (*mat)[i * SCALE + j] -= value;
            }
        }
        isE = true;
    }
    printMat();
    printOpnd();

    return true;
}

// [1번 수정 전 핵심 코드]
/*
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
*/
// [1번 수정 후]
bool Matrix::MatMaxColAdd() {
    if (isF) {
        matrix = fCapture;
        operand = fOperandCapture;
        isF = false;
    } else {
        fCapture = matrix;
        fOperandCapture = operand;

        for (auto* mat : {&matrix, &operand}) {
            for (int i = 0; i < SCALE; ++i) {
                int value = std::numeric_limits<int>::min();

                for (int j = 0; j < SCALE; ++j)
                    if ((*mat)[j * SCALE + i] > value)
                        value = (*mat)[j * SCALE + i];

                for (int j = 0; j < SCALE; ++j)
                    (*mat)[j * SCALE + i] += value;
            }
        }
        isF = true;
    }
    printMat();
    printOpnd();

    return true;
}

// [1번 수정 전 핵심 코드]
/*
bool Matrix::PlusMod() {
	for (int& a : matrix)
		a = (a + 1) % 10;
	return true;
}
*/
// [1번 수정 후]
bool Matrix::PlusMod() {
    for (auto* mat : {&matrix, &operand})
        for (int& a : *mat)
            a = ((a % 10 + (1)) % 10 + 10) % 10;
    printMat();
    printOpnd();

    return true;
}

// [1번 수정 전 핵심 코드]
/*
bool Matrix::SubMod() {
	for (int& a : matrix) 
		a = (a - 1 + 10) % 10;
	return true;
}
*/
// [1번 수정 후]
bool Matrix::SubMod() {
    for (auto* mat : {&matrix, &operand})
        for (int& a : *mat)
            a = ((a % 10 + (-1)) % 10 + 10) % 10;
    printMat();
    printOpnd();

    return true;
}

int main() {
    Matrix matrix{};
    std::array<int, SCALE * SCALE> opnd;

    for (int& a : opnd) {
        a = uid(dre);
    }

    matrix.command(opnd);
}

#elif PROB >= 2 && PROB <= 4

// ===== [2~4번 공통 입출력 및 콘솔 색상 시작] =====
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <optional>
#include <algorithm>
#include <sstream>
#include <random>
#include <cmath>
#include <iomanip>
#include <limits>
#define NOMINMAX
#include <windows.h>

class Console {
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD original = 7;
    bool available = false;

public:
    Console() {
        CONSOLE_SCREEN_BUFFER_INFO info{};

        available = GetConsoleScreenBufferInfo(handle, &info) != 0;

        if (available)
            original = info.wAttributes;
    }

    void color(bool highlighted) const {
        std::cout.flush();

        if (available)
            SetConsoleTextAttribute(handle, highlighted ? static_cast<WORD>((original & 0xF0) | FOREGROUND_GREEN |
                                                                            FOREGROUND_RED | FOREGROUND_INTENSITY)
                                                        : original);
    }

    void clear() const {
        std::cout.flush();

        CONSOLE_SCREEN_BUFFER_INFO info{};

        if (!available || !GetConsoleScreenBufferInfo(handle, &info))
            return;

        DWORD written = 0;
        const DWORD size = static_cast<DWORD>(info.dwSize.X) * info.dwSize.Y;

        FillConsoleOutputCharacterA(handle, ' ', size, {0, 0}, &written);
        FillConsoleOutputAttribute(handle, original, size, {0, 0}, &written);
        SetConsoleCursorPosition(handle, {0, 0});
    }

    ~Console() {
        color(false);
    }
};

bool noExtraInput(std::istringstream& input) {
    input >> std::ws;

    return input.eof();
}

// ===== [작성: 공통 코드 끝] =====

#if PROB == 2
// ===== [작성: 2번 파일 문자열 편집 시작] =====
class StringEditor {
    // 원래 파일의 '*'와 e 명령으로 삽입한 '*'를 구별한다.
    struct Character {
        char value;
        bool insertedStar = false;
    };

    using Line = std::vector<Character>;
    using Text = std::vector<Line>;

    Text lines;
    // a/d/e/f/g/h는 해당 명령 직전의 문자열만 복원한다.
    // c의 강조 옵션은 문자열 스냅샷과 독립적으로 유지한다.
    std::array<std::optional<Text>, 256> captures{};
    bool capitalColor = false;
    Console console;

    static bool upper(char c) {
        return c >= 'A' && c <= 'Z';
    }

    static char lower(char c) {
        return upper(c) ? static_cast<char>(c + 'a' - 'A') : c;
    }

    static bool separator(const Character& c) {
        return c.value == ' ' || c.insertedStar;
    }

    static std::string word(const Line& line, size_t begin, size_t end) {
        std::string text;

        for (size_t i = begin; i < end; ++i)
            text += lower(line[i].value);

        return text;
    }

    void print(bool counts = false, const std::string& search = "") {
        size_t total = 0;

        for (const auto& line : lines) {
            size_t count = 0;

            for (size_t i = 0; i < line.size();) {
                if (separator(line[i])) {
                    std::cout << line[i++].value;
                    continue;
                }
                const size_t begin = i;

                while (i < line.size() && !separator(line[i]))
                    ++i;
                ++count;

                const bool found = !search.empty() && word(line, begin, i) == search;
                const bool capital = capitalColor && upper(line[begin].value);

                if (search.empty() ? capital : found)
                    ++total;
                console.color(found || capital);

                for (size_t j = begin; j < i; ++j)
                    std::cout << line[j].value;
                console.color(false);
            }

            if (counts)
                std::cout << " [words: " << count << ']';
            std::cout << '\n';
        }

        if (!search.empty())
            std::cout << "Matches: " << total << '\n';
        else if (capitalColor)
            std::cout << "Capitalized words: " << total << '\n';
    }

public:
    bool load(const std::string& filename) {
        std::ifstream file(filename);

        if (!file) {
            std::cout << "Cannot open file.\n";

            return false;
        }

        Text loaded;
        std::string text;

        while (std::getline(file, text)) {
            if (!text.empty() && text.back() == '\r')
                text.pop_back();

            if (loaded.empty() && text.compare(0, 3, "\xEF\xBB\xBF") == 0)
                text.erase(0, 3);

            Line line;

            for (char c : text)
                line.push_back({c, false});
            loaded.push_back(line);
        }

        if (file.bad() || loaded.size() != 10) {
            std::cout << "The input file must contain exactly 10 lines.\n";

            return false;
        }
        lines = loaded;
        print();

        return true;
    }

    void run() {
        std::string input;

        while (std::cout << "a/b/c/d/e/f/g old new/h/i word/j/q: ", std::getline(std::cin, input)) {

            std::istringstream stream(input);
            char command;

            if (!(stream >> command))
                continue;

            if (command == 'q' && noExtraInput(stream))
                return;

            if (command == 'i') {
                std::string search;

                if (!(stream >> search) || !noExtraInput(stream)) {
                    std::cout << "Use: i word\n";
                    continue;
                }

                for (char& c : search)
                    c = lower(c);
                print(false, search);
                continue;
            }

            if (command < 'a' || command > 'j') {
                std::cout << "Unknown command.\n";
                continue;
            }

            const auto key = static_cast<unsigned char>(command);
            char from = 0, to = 0;

            if (command == 'g' && !captures[key]) {
                std::string oldChar, newChar;

                if (noExtraInput(stream)) {
                    std::cout << "Old character and new character: ";

                    if (!std::getline(std::cin, input))
                        return;
                    stream.clear();
                    stream.str(input);
                }

                if (!(stream >> oldChar >> newChar) || oldChar.size() != 1 || newChar.size() != 1 ||
                    !noExtraInput(stream)) {
                    std::cout << "Enter two single characters.\n";
                    continue;
                }
                from = oldChar[0];
                to = newChar[0];
            } else if (!noExtraInput(stream)) {
                std::cout << "Unexpected arguments.\n";
                continue;
            }

            if (command == 'b') {
                print(true);
                continue;
            }

            if (command == 'c') {
                capitalColor = !capitalColor;
                print(); // 현재 문자열에서 대문자로 시작하는 단어를 다시 찾는다.
                continue;
            }

            if (command == 'j') {
                // 1번 -> 2번, ..., 9번 -> 10번, 10번 -> 1번으로 순환한다.
                if (!lines.empty())
                    std::rotate(lines.begin(), lines.end() - 1, lines.end());
                print();
                continue;
            }

            if (captures[key]) {
                lines = *captures[key];
                captures[key].reset();
                print();
                continue;
            }
            captures[key] = lines;

            for (auto& line : lines) {
                if (command == 'd')
                    std::reverse(line.begin(), line.end());

                if (command == 'f') {
                    for (size_t i = 0; i < line.size();) {
                        if (separator(line[i])) {
                            ++i;
                            continue;
                        }

                        const auto begin = i;

                        while (i < line.size() && !separator(line[i]))
                            ++i;
                        std::reverse(line.begin() + begin, line.begin() + i);
                    }
                }

                for (auto& c : line) {
                    if (command == 'a') {
                        if (upper(c.value))
                            c.value = lower(c.value);
                        else if (c.value >= 'a' && c.value <= 'z')
                            c.value = static_cast<char>(c.value - 'a' + 'A');
                    } else if (command == 'e' && c.value == ' ')
                        c = {'*', true};
                    else if (command == 'g' && c.value == from)
                        c = {to, false};
                }
            }

            if (command == 'h') {
                Text split;

                for (const auto& line : lines) {
                    Line part;
                    bool afterDigit = false;

                    for (const auto& c : line) {
                        if (afterDigit && separator(c))
                            continue;
                        afterDigit = false;
                        part.push_back(c);

                        if (c.value >= '0' && c.value <= '9') {
                            split.push_back(part);
                            part.clear();
                            afterDigit = true;
                        }
                    }

                    if (!part.empty() || line.empty())
                        split.push_back(part);
                }
                lines = split;
            }
            print();
            std::cout << '\n';
        }
    }
};

int main() {
    StringEditor editor;
    std::string filename;

    while (std::cout << "Input data file name (q: quit): ", std::getline(std::cin, filename)) {
        if (filename == "q")
            return 0;

        if (editor.load(filename)) {
            editor.run();

            return 0;
        }
    }
}

// ===== [작성: 2번 끝] =====

#elif PROB == 3
// ===== [작성: 3번 구조체 점 리스트 시작] =====
struct Point {
    int x, y, z;
};

class PointList {
    using Slots = std::array<std::optional<Point>, 10>;

    Slots points{};
    std::optional<Slots> beforeSort;
    bool showDistances = false;

    static double distance(const Point& a, const Point& b = {0, 0, 0}) {
        const double dx = static_cast<double>(a.x) - b.x;
        const double dy = static_cast<double>(a.y) - b.y;
        const double dz = static_cast<double>(a.z) - b.z;

        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    static void printPoint(const Point& p) {
        std::cout << '(' << p.x << ", " << p.y << ", " << p.z << ')';
    }

    int count() const {
        return static_cast<int>(std::count_if(points.begin(), points.end(), [](const auto& p) {
            return p.has_value();
        }));
    }

    void print() const {
        for (int i = 9; i >= 0; --i) {
            std::cout << i << ": ";

            if (points[i]) {
                printPoint(*points[i]);

                if (showDistances)
                    std::cout << " distance=" << distance(*points[i]);
            } else
                std::cout << "-";
            std::cout << '\n';
        }
    }

    void pairs() const {
        double nearest = std::numeric_limits<double>::max(), farthest = -1;
        int ni = -1, nj = -1, fi = -1, fj = -1;

        for (int i = 0; i < 10; ++i)
            if (points[i])
                for (int j = i + 1; j < 10; ++j)
                    if (points[j]) {
                        const double d = distance(*points[i], *points[j]);

                        printPoint(*points[i]);
                        std::cout << " <-> ";
                        printPoint(*points[j]);
                        std::cout << " distance=" << d << '\n';

                        if (d < nearest) {
                            nearest = d;
                            ni = i;
                            nj = j;
                        }

                        if (d > farthest) {
                            farthest = d;
                            fi = i;
                            fj = j;
                        }
                    }

        if (ni < 0) {
            std::cout << "At least two points are required.\n";

            return;
        }
        std::cout << "Nearest: ";
        printPoint(*points[ni]);
        std::cout << " <-> ";
        printPoint(*points[nj]);
        std::cout << " distance=" << nearest << '\n';
        std::cout << "Farthest: ";
        printPoint(*points[fi]);
        std::cout << " <-> ";
        printPoint(*points[fj]);
        std::cout << " distance=" << farthest << '\n';
    }

public:
    void run() {
        print();

        std::string line;

        while (std::cout << "+ x y z/-/e x y z/d/a/b/c/f/g/q: ", std::getline(std::cin, line)) {
            std::istringstream input(line);
            char cmd;
            Point p{};

            if (!(input >> cmd))
                continue;

            if ((cmd == '+' || cmd == 'e') && !(input >> p.x >> p.y >> p.z)) {
                std::cout << "Enter three integers.\n";
                print();
                continue;
            }

            if (!noExtraInput(input)) {
                std::cout << "Unexpected arguments.\n";
                print();
                continue;
            }

            if (cmd == 'q')
                return;

            if (cmd == '+' || cmd == 'e') {
                if (cmd == '+') {
                    int highest = -1;

                    for (int i = 0; i < 10; ++i)
                        if (points[i])
                            highest = i;

                    if (count() == 10) {
                        // 빈칸이 없으면 다음 순환 위치(0번)를 덮어쓴다.
                        std::cout << "Overflow: replacing the point at index 0.\n";
                        points[0] = p;
                    }
                    // 맨 위 다음 칸부터 순환 탐색하여 아래쪽 빈칸도 사용한다.
                    else
                        for (int k = 1; k <= 10; ++k) {
                            const int index = (highest + k) % 10;

                            if (!points[index]) {
                                points[index] = p;
                                break;
                            }
                        }
                } else {
                    // 빈칸도 함께 올린다. 9번 데이터는 경고 후 버리고 삽입을 계속한다.
                    if (points[9])
                        std::cout << "Overflow: discarding the point at index 9.\n";

                    for (int i = 9; i > 0; --i)
                        points[i] = points[i - 1];
                    points[0] = p;
                }
            } else if (cmd == '-' || cmd == 'd') {
                if (count() == 0)
                    std::cout << "List is empty.\n";
                else
                    for (int k = 0; k < 10; ++k) {
                        const int i = cmd == '-' ? 9 - k : k;

                        if (points[i]) {
                            points[i].reset();
                            break;
                        }
                    }
            } else if (cmd == 'a')
                std::cout << "Count: " << count() << '\n';
            else if (cmd == 'b')
                std::rotate(points.begin(), points.begin() + 1, points.end());
            // c는 현재 리스트만 비운다. 대기 중인 f의 복원 지점은 유지한다.
            else if (cmd == 'c') {
                points = {};
            } else if (cmd == 'f') {
                if (beforeSort) {
                    points = *beforeSort;
                    beforeSort.reset();
                    showDistances = false;
                } else {
                    beforeSort = points;

                    std::vector<Point> sorted;

                    for (const auto& point : points)
                        if (point)
                            sorted.push_back(*point);
                    std::stable_sort(sorted.begin(), sorted.end(), [](const Point& a, const Point& b) {
                        return distance(a) < distance(b);
                    });
                    points = {};

                    for (size_t i = 0; i < sorted.size(); ++i)
                        points[i] = sorted[i];
                    showDistances = true;
                }
            } else if (cmd == 'g')
                pairs();
            else
                std::cout << "Unknown command.\n";
            print();
        }
    }
};

int main() {
    PointList list;

    list.run();
}

// ===== [작성: 3번 끝] =====

#elif PROB == 4
// ===== [작성: 4번 카드 짝 맞추기 시작] =====
class CardGame {
    int width = 0, height = 0, attempts = 0, score = 0;
    std::vector<char> cards;
    std::vector<bool> matched;
    std::mt19937 random{std::random_device{}()};
    Console console;

    int limit() const {
        return width * height * 2;
    }

    bool complete() const {
        return std::all_of(matched.begin(), matched.end(), [](bool v) {
            return v;
        });
    }

    void reset() {
        cards.clear();

        for (int i = 0; i < width * height / 2; ++i) {
            cards.push_back(static_cast<char>('a' + i));
            cards.push_back(static_cast<char>('a' + i));
        }

        if (width * height % 2)
            cards.push_back('@');
        std::shuffle(cards.begin(), cards.end(), random);
        matched.assign(cards.size(), false);
        attempts = score = 0;
    }

    int position(const std::string& token) const {
        if (token.size() != 2 || token[0] < 'a' || token[0] >= 'a' + width || token[1] < '1' ||
            token[1] >= '1' + height)
            return -1;

        return (token[1] - '1') * width + token[0] - 'a';
    }

    void print(bool hint = false, int first = -1, int second = -1) {
        std::cout << "    ";

        for (int x = 0; x < width; ++x)
            std::cout << static_cast<char>('a' + x) << "   ";
        std::cout << '\n';

        for (int y = 0; y < height; ++y) {
            std::cout << y + 1 << "   ";

            for (int x = 0; x < width; ++x) {
                const int i = y * width + x;
                char c = '*';

                if (hint || matched[i] || i == first || i == second)
                    c = cards[i];

                if (matched[i] && c >= 'a' && c <= 'z')
                    c = static_cast<char>(c - 'a' + 'A');
                console.color(matched[i] || i == first || i == second);
                std::cout << c << "   ";
                console.color(false);
            }
            std::cout << '\n';
        }
        std::cout << "Attempts: " << attempts << '/' << limit() << "  Score: " << score << '\n';
    }

    bool pause() {
        std::cout << "Press Enter to continue (q: quit): ";

        std::string line;

        return static_cast<bool>(std::getline(std::cin, line)) && line != "q";
    }

public:
    void run() {
        std::string line;

        while (true) {
            std::cout << "Rows and columns (3..6, e.g. 3 4 = 3 rows x 4 columns, q: quit): ";

            if (!std::getline(std::cin, line) || line == "q")
                return;

            std::istringstream input(line);
            // PDF 예시 기준: 3 4 입력 -> 세로 3행, 가로 4열 (a~d, 1~3).
            if ((input >> height >> width) && noExtraInput(input) && width >= 3 && width <= 6 && height >= 3 &&
                height <= 6)
                break;
            std::cout << "Enter two integers from 3 to 6.\n";
        }
        reset();
        std::cout << "Match: +10 points. Mismatch: 0. Limit: twice the number of cells.\n"
                     "Joker (@) opens the selected letter and its matching card.\n";
        print();

        while (std::cout << "Select two cells (a1 c3), r/h/q: ", std::getline(std::cin, line)) {
            std::istringstream input(line);
            std::string a, b;

            if (!(input >> a))
                continue;

            if ((a == "q" || a == "r" || a == "h") && noExtraInput(input)) {
                if (a == "q")
                    return;

                if (a == "r") {
                    reset();
                    console.clear();
                    print();
                } else {
                    console.clear();
                    print(true);

                    if (!pause())
                        return;
                    console.clear();
                    print();
                }
                continue;
            }

            if (complete() || attempts >= limit()) {
                std::cout << "Game over. Use r or q.\n";
                continue;
            }

            if (!(input >> b) || !noExtraInput(input)) {
                std::cout << "Use: a1 c3\n";
                continue;
            }

            const int first = position(a), second = position(b);

            if (first < 0 || second < 0 || first == second || matched[first] || matched[second]) {
                std::cout << "Select two different, hidden cells inside the board.\n";
                continue;
            }

            ++attempts;
            console.clear();
            print(false, first, second);

            const bool joker = cards[first] == '@' || cards[second] == '@';

            if (cards[first] == cards[second] || joker) {
                matched[first] = matched[second] = true;

                if (joker) {
                    // PDF 그림: 조커와 선택한 카드뿐 아니라 그 카드의 짝도 공개한다.
                    const char letter = cards[first] == '@' ? cards[second] : cards[first];

                    for (size_t i = 0; i < cards.size(); ++i)
                        if (cards[i] == letter)
                            matched[i] = true;
                }
                score += 10;
                // 모든 일반 카드를 맞추고 조커만 남았다면 자동 공개한다.
                bool onlyJoker = true;

                for (size_t i = 0; i < cards.size(); ++i)
                    if (!matched[i] && cards[i] != '@')
                        onlyJoker = false;

                if (onlyJoker)
                    std::fill(matched.begin(), matched.end(), true);
                std::cout << "Match! +10\n";
            } else
                std::cout << "Mismatch.\n";

            if (!pause())
                return;
            console.clear();
            print();

            if (complete())
                std::cout << "All cards matched! Final score: " << score << '\n';
            else if (attempts >= limit())
                std::cout << "No attempts left. Final score: " << score << '\n';
        }
    }
};

int main() {
    CardGame game;

    game.run();
}

// ===== [작성: 4번 끝] =====
#endif
#else
#error PROB must be 1, 2, 3, or 4.
#endif
