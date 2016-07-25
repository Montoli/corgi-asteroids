#include "math_common.h"
#include <cstdlib>
#include <ctime>



float rnd() {
	//std::srand(static_cast<unsigned int>(std::time(0)));
	return (std::rand()) / static_cast<float>(RAND_MAX);

}

