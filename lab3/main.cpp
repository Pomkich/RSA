#include <iostream>
#include <vector>
#include <time.h>
#include <bitset>
#include <string>

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
int fast_pow(int base, int index_n, int modulus) {
	int c = 1;
	for (int i = 0; i < index_n; i++) {
		c = (c * base) % modulus;
	}
	return c;
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



int main() {
	int p = 23;	// DEBUG -> числа должны выбираться рандомно
	int q = 53;
	int n = p * q;	// модуль

	int Fn = (p - 1) * (q - 1);
	int e = 0;
	int d = 0;

	srand(time(NULL));
	while (d < 2) {	// d обязательно должно быть положительным?
		e = rand() % Fn + 1;
		// подбор взаимно простого числа для F(n)
		while (true) {
			if (e >= n) e = rand() % Fn + 1;	// если дошли до верхнего предела, снова выбираем число
			if (gcd(Fn, e) == 1) break;	// если НОД == 1 -> простое число найдено
			e++;
		}
		d = gcd_ext(Fn, e).second;	// d - коэффициент перед e
	}
	//	 DEBUG
	// std::cout << Fn << gcd_ext(Fn, e).first << std::endl;
	// std::cout << "e: " << e << std::endl;
	std::cout << "open key:   (" << e << ", " << n << ")" << std::endl;
	std::cout << "secret key: (" << d << ", " << n << ")" << std::endl;

	// вычисляем размер блока в битах
	int block_len = std::log(n) / std::log(2);

	std::string message = "abcqwe";	// сообщение, которое нужно защифровать
	std::string bits = convert_to_bits(message, block_len);	// двоичное представление сообщения
	int blocks = count_blocks(bits, block_len);

	std::vector<int> encrypted_blocks;	// зашифрованное сообщение представлено набором чисел
	for (int i = 0; i < blocks; i++) {	// вычисляем для каждого блока соответствующее число
		int e_block = encrypt_block(bits, block_len, i, e, n);
		encrypted_blocks.push_back(e_block);
	}
	std::cout << std::endl;

	std::string encrypted_bits;
	for (auto block : encrypted_blocks) {
		encrypted_bits += convert_to_bits(block, block_len);
	}

	// дешифровка сообщения
	for (int i = 0; i < encrypted_blocks.size(); i++) {
		std::cout << decrypt_block(encrypted_blocks[i], block_len, d, n);
	}
	std::cout << std::endl;
	std::cout << bits << std::endl;


	return 0;
}