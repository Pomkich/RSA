#include <iostream>
#include <vector>
#include <time.h>
#include <bitset>
#include <string>
#include <fstream>

using namespace std;

// нахождение наибольщего общего делителя
int gcd(int a, int b) {
	if (b == 0)
		return a;
	return gcd(b, a % b);
}

// расширенный алгоритм Евклида для нахождения коэффициентов
std::pair<int, int> gcd_ext(int a, int b) {
	std::vector<int> r;
	std::vector<int> s;
	std::vector<int> t;

	r.push_back(a); r.push_back(b);
	s.push_back(1); s.push_back(0);
	t.push_back(0); t.push_back(1);

	int i = 1;
	while (r.back() != 0) {
		int q = (r[i - 1] / r[i]);	// целая часть от деления
		r.push_back(r[i - 1] - (q * r[i]));
		s.push_back(s[i - 1] - (q * s[i]));
		t.push_back(t[i - 1] - (q * t[i]));
		i++;
	}

	return std::make_pair(s[i - 1], t[i - 1]);
}

// алгоритм быстрого возведения в степень по модулю
int fast_pow(long long x, unsigned int y, int p) {
	int res = 1;     // Initialize result

	x = x % p; // Update x if it is more than or
				// equal to p

	if (x == 0) return 0; // In case x is divisible by p;

	while (y > 0)
	{
		// If y is odd, multiply x with result
		if (y & 1)
			res = (res * x) % p;

		// y must be even now
		y = y >> 1; // y = y/2
		x = (x * x) % p;
	}
	return res;
}

// функция преобразования сообщения в блоки, представленных в бинарном виде
std::string convert_to_bits(std::string message, int block_len) {
	std::string bits;
	// конвертация сообщения в бинарный вид
	for (int i = 0; i < message.size(); i++) {
		std::bitset<8> bt(message[i]);
		bits += bt.to_string();
	}

	// если остался неполный блок, дополняем его нулями
	if (bits.size() % block_len != 0) {
		bits.insert(bits.end(), block_len - (bits.size() % block_len), '0');
	}

	return bits;
}

// перегруженная функция, в этот раз преобразует число, а не строку
std::string convert_to_bits(int number, int block_len) {
	std::string bits;
	int read_mask = std::pow(2, block_len); // маска вида 00...00100...00 повзоляет 
											// читать один бит, начиная с самого первого относительно размера блока

	for (int i = 1; i <= block_len; i++) {
		bool bit = (number << i) & read_mask;
		if (bit) bits.push_back('1');
		else bits.push_back('0');
	}

	return bits;
}


std::string convert_to_bytes(std::string bits) {
	std::string res;
	for (int i = 0; i < bits.size() / 8; i++) {
		char byte = NULL;
		for (int j = 0; j < 8; j++) {
			bool bit = (bits[8 * i + j] == '1' ? true : false);
			byte |= bit << (8 - (j + 1));	// при первой итерации не будет смещения, поэтому +1
		}
		res.push_back(byte);
	}

	return res;
}

// преобразует поток битов в поток чисел
std::vector<int> convert_to_nums(std::string bits, int blocks, int block_len) {
	std::vector<int> res;
	for (int i = 0; i < blocks; i++) {
		int num = 0;
		for (int j = 0; j < block_len; j++) {	// в m записываются block_len бит
			bool bit = (bits[block_len * i + j] == '1' ? true : false);
			num |= bit << (block_len - (j + 1));	// при первой итерации не будет смещения, поэтому +1
		}
		res.push_back(num);
	}
	return res;
}

// функция подсчёта количества блоков в шифруемом сообщении
int count_blocks(std::string bits, int block_len) {
	int blocks = 0;	// общее количество блоков, полученное в ходе шифрования
	for (int i = 0; i < bits.size(); i++) {
		if (i % block_len == 0) 	// подсчёт созданных блоков
			blocks++;
	}

	return blocks;
}

// функция возвращает число, соответствующее зашифрованному блоку
int encrypt_block(std::string bits, int block_len, int n, int exp, int modulus) {	// n - номер блока
	int m = 0;
	for (int j = 0; j < block_len; j++) {	// в m записываются block_len бит
		bool bit = (bits[block_len * n + j] == '1' ? true : false);
		m |= bit << (block_len - (j + 1));	// при первой итерации не будет смещения, поэтому +1
	}
	return fast_pow(m, exp, modulus);
}

std::string decrypt_block(int encrypted_block, int block_len, int exp, int modulus) {
	int m = fast_pow(encrypted_block, exp, modulus);	// находим исходное число
	return convert_to_bits(m, block_len);	// и преобразуем его в поток битов
}

void generate_keys() {
	int p = 5323;
	int q = 12343;

	std::cout << "enter two different prime numbers" << std::endl;
	std::cin >> p >> q;

	long long n = p * q;	// модуль

	long long Fn = (p - 1) * (q - 1);
	int e = 0;
	int d = 0;

	srand(time(NULL));
	while (d < 2) {	// d обязательно должно быть положительным?
		e = rand() % Fn + 1;
		// подбор взаимно простого числа для F(n)
		while (true) {
			e++;
			if (e >= n) e = rand() % Fn + 1;	// если дошли до верхнего предела, снова выбираем число
			if (gcd(Fn, e) == 1) break;	// если НОД == 1 -> простое число найдено
		}
		d = gcd_ext(Fn, e).second;	// d - коэффициент перед e
	}

	std::cout << "open key:   (" << e << ", " << n << ")" << std::endl;
	std::cout << "secret key: (" << d << ", " << n << ")" << std::endl;
}

void encrypt_file(std::string file_name, int e, int n) {
	std::ifstream input(file_name, std::ios::binary);
	std::string file_bytes(std::istreambuf_iterator<char>(input), {});
	int block_len = std::ceil(std::log(n) / std::log(2));

	std::string bits = convert_to_bits(file_bytes, block_len);

	int blocks = count_blocks(bits, block_len);
	
	std::vector<int> encrypted_blocks;	// зашифрованное сообщение представлено набором чисел
	for (int i = 0; i < blocks; i++) {	// вычисляем для каждого блока соответствующее число
		int e_block = encrypt_block(bits, block_len, i, e, n);
		encrypted_blocks.push_back(e_block);
	}

	std::string encrypted_bits;
	for (int i = 0; i < blocks; i++) {
		encrypted_bits += convert_to_bits(encrypted_blocks[i], block_len);
	}

	std::string encrypted_file_bytes = convert_to_bytes(encrypted_bits);

	std::ofstream output("encrypted_" + file_name, std::ios::binary);
	output << encrypted_file_bytes;

	input.close();
	output.close();
}

void decrypt_file(std::string file_name, int d, int n) {
	std::ifstream input(file_name, std::ios::binary);
	std::string file_bytes(std::istreambuf_iterator<char>(input), {});
	int block_len = std::ceil(std::log(n) / std::log(2));

	std::string encrypted_bits = convert_to_bits(file_bytes, block_len);
	
	int blocks = count_blocks(encrypted_bits, block_len);

	auto encrypted_blocks = convert_to_nums(encrypted_bits, blocks, block_len);

	std::string decrypted_blocks;
	for (int i = 0; i < encrypted_blocks.size(); i++) {
		decrypted_blocks += decrypt_block(encrypted_blocks[i], block_len, d, n);
	}

	std::string decrypted_file_bytes = convert_to_bytes(decrypted_blocks);

	std::ofstream output("decrypted_" + file_name, std::ios::binary);
	output << decrypted_file_bytes;

	input.close();
	output.close();
}

int main() {
	char cmd;
	while (true) {
		std::cin >> cmd;
		std::string file_name;
		int e(0), d(0), n(0);
		switch (cmd) {
		case 'g':
			generate_keys();
			break;

		case 'e':
			std::cout << "which file encrypt?" << std::endl;
			cin >> file_name;
			std::cout << "enter public key please" << std::endl;
			cin >> e >> n;
			encrypt_file(file_name, e, n);
			std::cout << "file encrypted" << std::endl;
			break;

		case 'd':
			std::cout << "which file decrypt?" << std::endl;
			cin >> file_name;
			std::cout << "enter secret key please" << std::endl;
			cin >> d >> n;
			decrypt_file(file_name, d, n);
			std::cout << "file decrypted" << std::endl;
			break;

		case 'q':
			exit(0);
			break;
		}
	}

	return 0;
}

/*int p = 5323;	// DEBUG -> числа должны выбираться рандомно
	int q = 12343;
	long long n = p * q;	// модуль

	long long Fn = (p - 1) * (q - 1);
	int e = 0;
	int d = 0;

	srand(time(NULL));
	while (d < 2) {	// d обязательно должно быть положительным?
		e = rand() % Fn + 1;
		// подбор взаимно простого числа для F(n)
		while (true) {
			e++;
			if (e >= n) e = rand() % Fn + 1;	// если дошли до верхнего предела, снова выбираем число
			if (gcd(Fn, e) == 1) break;	// если НОД == 1 -> простое число найдено
		}
		d = gcd_ext(Fn, e).second;	// d - коэффициент перед e
	}

	// DEBUG
	std::cout << "open key:   (" << e << ", " << n << ")" << std::endl;
	std::cout << "secret key: (" << d << ", " << n << ")" << std::endl;

	// вычисляем размер блока в битах
	int block_len = std::ceil(std::log(n) / std::log(2));
	std::cout << "block lenght: " << block_len << std::endl;

	std::string message = "bwebewqeqwewqe";	// сообщение, которое нужно защифровать
	std::string bits = convert_to_bits(message, block_len);	// двоичное представление сообщения
	int blocks = count_blocks(bits, block_len);	// подсчёт количества блоков

	std::vector<int> encrypted_blocks;	// зашифрованное сообщение представлено набором чисел
	for (int i = 0; i < blocks; i++) {	// вычисляем для каждого блока соответствующее число
		int e_block = encrypt_block(bits, block_len, i, e, n);
		encrypted_blocks.push_back(e_block);
	}
	// с этого момента сообщение зашифровано, его можно передать
	std::cout << std::endl;

	// представляем набор чисел как набор битов
	std::string encrypted_bits;
	for (auto block : encrypted_blocks) {
		encrypted_bits += convert_to_bits(block, block_len);
	}

	// дешифровка сообщения
	std::string decrypted_bits;
	for (int i = 0; i < encrypted_blocks.size(); i++) {
		decrypted_bits += decrypt_block(encrypted_blocks[i], block_len, d, n);
	}
	std::cout << std::endl;
	std::cout << "source:    " << bits << std::endl;
	std::cout << "crypted:   " << encrypted_bits << std::endl;
	std::cout << "decrypted: " << decrypted_bits << std::endl;

	// если передали не набор чисел а строку битов, то преобразуем их в поток чисел
	auto new_blocks = convert_to_nums(encrypted_bits, blocks, block_len);
	for (auto block : encrypted_blocks) {
		std::cout << block << " ";
	}

	std::cout << std::endl;

	for (auto block : new_blocks) {
		std::cout << block << " ";
	}

	std::string str;
	for (int i = 0; i < new_blocks.size(); i++) {
		str += decrypt_block(new_blocks[i], block_len, d, n);
	}
	std::cout << std::endl;
	str = convert_to_bytes(str);
	std::cout << str << std::endl;*/