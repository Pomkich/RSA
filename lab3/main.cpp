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

std::vector<unsigned char> read_file(std::string file_name) {
	std::ifstream infile(file_name, std::ios::binary);

	//get length of file
	infile.seekg(0, std::ios::end);
	size_t length = infile.tellg();
	infile.seekg(0, std::ios::beg);
	
	char* buffer = new char[length];
	memset(buffer, 0, length);

	//read file
	infile.read(buffer, length);

	std::vector<unsigned char> uns_buf;
	for (int i = 0; i < length; i++)
		uns_buf.push_back((unsigned char)buffer[i]);

	delete[] buffer;
	return uns_buf;
}

std::string bytes_to_bits(std::vector<unsigned char> bytes) {
	std::string bits;
	for (int i = 0; i < bytes.size(); i++) {
		unsigned char byte = bytes[i];
		for (int j = 0; j < 8; j++) {
			if ((byte & 0b10000000) == 128) bits.push_back('1');
			else bits.push_back('0');
			byte = byte << 1;
		}
	}
	return bits;
}

std::string nums_to_bits(std::vector<unsigned int> nums, int block_len, int blocks) {
	std::string bits;
	for (int i = 0; i < blocks; i++) {
		std::string sub_bits;
		for (int bit_offset = block_len - 1; bit_offset >= 0; bit_offset--) {
			bool bit = ((nums[i] >> bit_offset) & 1);
			if (bit) sub_bits.push_back('1');
			else sub_bits.push_back('0');
		}
		bits += sub_bits;
	}
	return bits;
}

/*std::vector<unsigned char> bits_to_bytes(std::string bits) {
	std::vector<unsigned int> bytes;

	int to_fill = 8 - (bits.size() % 8);	// 8 - количество битов в байте
	if (to_fill != 8) {
		for (int i = 0; i < to_fill; i++)
			bits.push_back('0');
	}

	for (int i = 0; i < bits.size() / 8; i++) {
		for (int bit_offset = 0; bit_offset < 8; bit_offset++) {
			
		}
	}
}*/

unsigned int bstr_to_num(std::string bits) {
	unsigned int num = 0;
	for (int i = bits.size() - 1, j = 0; i >= 0; i--, j++)
		if (bits[j] == '1') num |= (1 << i);
	return num;
}

void encrypt(std::string bits, int e, int n) {
	int block_len = std::ceil(std::log(n) / std::log(2));
	std::vector<unsigned int> source_message;

	// дополнить нулями биты
	int to_fill = block_len - (bits.size() % block_len);
	if (to_fill != block_len) {
		for (int i = 0; i < to_fill; i++)
			bits.push_back('0');
	}

	// разбить на блоки
	int blocks = std::ceil((double)bits.size() / block_len);
	for (int i = 0; i < blocks; i++) {
		std::string sb_num = bits.substr(i * block_len, block_len);
		source_message.push_back(bstr_to_num(sb_num));	// превратить строки в числа, добавить в вектор
	}

	std::vector<unsigned int> encrypted_message;
	// зашифровать
	for (auto num : source_message) {
		encrypted_message.push_back(fast_pow(num, e, n));
		std::cout << encrypted_message.back() << std::endl;
	}

	// перевести зашифрованное сообщение в биты
	nums_to_bits(encrypted_message, block_len, blocks);
	
}

int main() {
	/*auto buffer = read_file("1.txt");
	std::ofstream out_file("2.txt", std::ios::binary);
	for (int i = 0; i < buffer.size(); i++) {
		out_file << buffer[i];
	}

	auto bits = bytes_to_bits(buffer);
	std::cout << bits;*/

	std::string bits = "111110010011111110010011111110010011111110010011111110010011111110010011010101";
	encrypt(bits, 16427, 65701789);
	//std::cout << bstr_to_num(bits);

	return 0;
}

/*char cmd;
	while (true) {
		std::cin >> cmd;
		std::string file_name;
		int e(0), d(0), n(0);
		switch (cmd) {
		case 'g':
			generate_keys();
			break;

		case 'e':
			break;

		case 'd':
			break;

		case 'q':
			exit(0);
			break;
		}
	}*/