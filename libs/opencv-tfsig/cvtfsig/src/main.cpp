#include <cpluslogger.hpp>
#include <cplusutil.hpp>

#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/version.hpp>

// DeFUSE (Version 1.0) *******************************
// ****************************************************
//        _    _      ()_()
//       | |  | |    |(o o)
//    ___| | _| | ooO--`o'--Ooo
//   / __| |/ / |/ _ \ __|_  /
//   \__ \   <| |  __/ |_ / /
//   |___/_|\_\_|\___|\__/___|
//
// ****************************************************


using namespace std;

int main(int argc, char* argv[])
{
	std::cout << "DeFUSE (Fusion of several Dynamic Content Descriptors)" << std::endl;
	std::cout << "Check dependencies (cpluslogger, cplusutil, boost, opencv):" << std::endl;
	LOG_INFO("cpluslogger rocks ...");
	std::cout << "cplusutil toString(100) =" << cplusutil::String::toStirng(100) << "rocks ..." << std::endl;
	std::cout << "Using Boost "
		<< BOOST_VERSION / 100000 << "."  // major version
		<< BOOST_VERSION / 100 % 1000 << "."  // minor version
		<< BOOST_VERSION % 100                // patch level
		<< std::endl;
	std::cout << "The OpenCV Version is " << CV_VERSION << std::endl;

}
