#include <cctype>
#include <string>
#include <functional>
#include <iostream>

int main(){
std::string s = "Hello World!";
s.erase(std::remove_if(s.begin(), s.end(),
    std::not1(std::ptr_fun(std::isalnum)), s.end()), s.end());
std::cout << s << std::endl;

}
