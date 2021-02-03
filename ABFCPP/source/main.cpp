#include "abf.h" 
#include <vector>


int main() {
	ABF abf = ABF("../ABFCPP/test/test2.abf");
	std::vector<float> data = abf.data();
	float* t = data.data();
	abf.save("../ABFCPP/test/test.abf", t);
	abf.~ABF();
	ABF abf2 = ABF("../ABFCPP/test/test.abf");
	std::vector<float> data2 = abf2.data();
	return 1;
}