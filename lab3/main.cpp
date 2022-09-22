#include <iostream>
#include <vector>
#include <time.h>
#include <bitset>
#include <string>

using namespace std;

// ���������� ����������� ������ ��������
int gcd(int a, int b) {
	if (b == 0)
		return a;
	return gcd(b, a % b);
}

// ����������� �������� ������� ��� ���������� �������������
std::pair<int, int> gcd_ext(int a, int b) {
	std::vector<int> r;
	std::vector<int> s;
	std::vector<int> t;

	r.push_back(a); r.push_back(b);
	s.push_back(1); s.push_back(0);
	t.push_back(0); t.push_back(1);

	int i = 1;
	while (r.back() != 0) {
		int q = (r[i - 1] / r[i]);	// ����� ����� �� �������
		r.push_back(r[i - 1] - (q * r[i]));
		s.push_back(s[i - 1] - (q * s[i]));
		t.push_back(t[i - 1] - (q * t[i]));
		i++;
	}

	return std::make_pair(s[i - 1], t[i - 1]);
}

// �������� �������� ���������� � ������� �� ������
int fast_pow(int base, int index_n, int modulus) {
	int c = 1;
	for (int i = 0; i < index_n; i++) {
		c = (c * base) % modulus;
	}
	return c;
}

// ������� �������������� ��������� � �����, �������������� � �������� ����
std::string convert_to_bits(std::string message, int block_len) {
	std::string bits;
	// ����������� ��������� � �������� ���
	for (int i = 0; i < message.size(); i++) {
		std::bitset<8> bt(message[i]);
		bits += bt.to_string();
	}

	// ���� ������� �������� ����, ��������� ��� ������
	if (bits.size() % block_len != 0) {
		bits.insert(bits.end(), block_len - (bits.size() % block_len), '0');
	}

	return bits;
}

// ������������� �������, � ���� ��� ����������� �����, � �� ������
std::string convert_to_bits(int number, int block_len) {
	std::string bits;
	int read_mask = std::pow(2, block_len); // ����� ���� 00...00100...00 ��������� 
											// ������ ���� ���, ������� � ������ ������� ������������ ������� �����

	for (int i = 1; i <= block_len; i++) {
		bool bit = (number << i) & read_mask;
		if (bit) bits.push_back('1');
		else bits.push_back('0');
	}

	return bits;
}

// ������� �������� ���������� ������ � ��������� ���������
int count_blocks(std::string bits, int block_len) {
	int blocks = 0;	// ����� ���������� ������, ���������� � ���� ����������
	for (int i = 0; i < bits.size(); i++) {
		if (i % block_len == 0) 	// ������� ��������� ������
			blocks++;
	}

	return blocks;
}

// ������� ���������� �����, ��������������� �������������� �����
int encrypt_block(std::string bits, int block_len, int n, int exp, int modulus) {	// n - ����� �����
	int m = 0;
	for (int j = 0; j < block_len; j++) {	// � m ������������ block_len ���
		bool bit = (bits[block_len * n + j] == '1' ? true : false);
		m |= bit << (block_len - (j + 1));	// ��� ������ �������� �� ����� ��������, ������� +1
	}
	return fast_pow(m, exp, modulus);
}

std::string decrypt_block(int encrypted_block, int block_len, int exp, int modulus) {
	int m = fast_pow(encrypted_block, exp, modulus);	// ������� �������� �����
	return convert_to_bits(m, block_len);	// � ����������� ��� � ����� �����
}



int main() {
	int p = 23;	// DEBUG -> ����� ������ ���������� ��������
	int q = 53;
	int n = p * q;	// ������

	int Fn = (p - 1) * (q - 1);
	int e = 0;
	int d = 0;

	srand(time(NULL));
	while (d < 2) {	// d ����������� ������ ���� �������������?
		e = rand() % Fn + 1;
		// ������ ������� �������� ����� ��� F(n)
		while (true) {
			if (e >= n) e = rand() % Fn + 1;	// ���� ����� �� �������� �������, ����� �������� �����
			if (gcd(Fn, e) == 1) break;	// ���� ��� == 1 -> ������� ����� �������
			e++;
		}
		d = gcd_ext(Fn, e).second;	// d - ����������� ����� e
	}
	//	 DEBUG
	// std::cout << Fn << gcd_ext(Fn, e).first << std::endl;
	// std::cout << "e: " << e << std::endl;
	std::cout << "open key:   (" << e << ", " << n << ")" << std::endl;
	std::cout << "secret key: (" << d << ", " << n << ")" << std::endl;

	// ��������� ������ ����� � �����
	int block_len = std::log(n) / std::log(2);

	std::string message = "abcqwe";	// ���������, ������� ����� �����������
	std::string bits = convert_to_bits(message, block_len);	// �������� ������������� ���������
	int blocks = count_blocks(bits, block_len);

	std::vector<int> encrypted_blocks;	// ������������� ��������� ������������ ������� �����
	for (int i = 0; i < blocks; i++) {	// ��������� ��� ������� ����� ��������������� �����
		int e_block = encrypt_block(bits, block_len, i, e, n);
		encrypted_blocks.push_back(e_block);
	}
	std::cout << std::endl;

	std::string encrypted_bits;
	for (auto block : encrypted_blocks) {
		encrypted_bits += convert_to_bits(block, block_len);
	}

	// ���������� ���������
	for (int i = 0; i < encrypted_blocks.size(); i++) {
		std::cout << decrypt_block(encrypted_blocks[i], block_len, d, n);
	}
	std::cout << std::endl;
	std::cout << bits << std::endl;


	return 0;
}