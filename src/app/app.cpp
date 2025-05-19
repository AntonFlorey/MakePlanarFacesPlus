#include <iostream>
#include <pybind11/pybind11.h>

void say_hello ()
{
	std::cout << "Hello World" << std::endl;
}

PYBIND11_MODULE (testmodule, m)
{
	m.doc() = "This is a module created with pybind11";
	m.def("say_hello", &say_hello, "A function that prints Hello World");
}