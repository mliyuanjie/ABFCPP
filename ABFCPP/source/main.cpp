#include "abf.h" 
#include <vector>


int main() {
	ABF abf = ABF("../ABFCPP/test/test3.abf");
	std::vector<float> data = abf.data();
	float* t = data.data();
	std::vector<unsigned int> v1{ 0 };
	std::vector<unsigned int> v2{ 2000 };
	abf.save(v1, v2);
	abf.~ABF();
	ABF abf2 = ABF("../ABFCPP/test/test3_new.abf");
	std::vector<float> data2 = abf2.data();
	return 1;
}