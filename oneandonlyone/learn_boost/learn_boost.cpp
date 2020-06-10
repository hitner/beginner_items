// learn_boost.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "boost/parameter.hpp"
#include "boost/lambda/lambda.hpp"

#include <iostream>
#include <iterator>
#include <algorithm>

#include <boost/program_options.hpp>
using namespace std;

void learn_lambda()
{
	using namespace boost::lambda;
	typedef std::istream_iterator<int> in;

	std::for_each(in(std::cin), in(), std::cout << (_1 * 3) << " ");
}

void learn_parameter()
{

}

void learn_program_options(int argc, char * argv[])
{
	namespace po = boost::program_options;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "hi,what care?")
		("age", po::value<double>(), "set age");
	po::variables_map varMap;
	po::store(po::parse_command_line(argc, argv, desc), varMap);
	po::notify(varMap);

	if (varMap.count("help"))
	{
		cout << desc << "\n";
		return;
	}

	if (varMap.count("age"))
	{
		cout << "age:" << varMap["age"].as<double>() << endl;
	}

}
int _tmain(int argc, _TCHAR* argv[])
{
	learn_program_options(argc, argv);
	return 0;
}