#include "parser_csv.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>

int main()
{

    CSVParser<int, std::string> parser("test.csv", 0);
    

    for (std::tuple<int, std::string> rs : parser) {
        std::cout << rs << std::endl;
    }

    /*std::cout << a;*/

    return 0;
}

