/** TRECVid (Version 1.0) ******************************
* ******************************************************
*       _    _      ()_()
*      | |  | |    |(o o)
*   ___| | _| | ooO--`o'--Ooo
*  / __| |/ / |/ _ \ __|_  /
*  \__ \   <| |  __/ |_ / /
*  |___/_|\_\_|\___|\__/___|
*
* ******************************************************
* Purpose: 
* Input/Output:

* @author skletz
* @version 1.0 24/05/17
*
**/

#include <cpluslogger.hpp>
#include <cplusutil.hpp>

#include <boost/program_options.hpp>

#include "structs.hpp"
#include "tool.hpp"
#include "data_info.hpp"

using namespace boost;
using namespace trecvid;

static std::string prog = "TREVid";

enum Task{ datainfo, msbconverter, msbdextraction};
program_options::variables_map processProgramOptions(const int argc, const char *const argv[]);


int main(int argc, char const *argv[]) {

	program_options::variables_map args = processProgramOptions(argc, argv);

	Tool* tool = nullptr;

	if(args["task"].as< int >() == 0)
	{
		tool = new DataInfo();
		static_cast<DataInfo*>(tool)->setProgrammOptions(args);
		
	}else if(args["task"].as< int >() == 1)
	{
		
	}
	else if (args["task"].as< int >() == 2)
	{
		
	}else
	{
		LOG_FATAL("Task " << args["task"].as< int >() << " is not defined");
	}

	delete tool;

	return EXIT_SUCCESS;
}

void showHelp(const program_options::options_description& desc)
{
	LOG_INFO(desc);
	exit(EXIT_SUCCESS);
}

program_options::variables_map processProgramOptions(const int argc, const char *const argv[])
{
	program_options::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Show brief usage message")
		("task,t", program_options::value<int>()->default_value(Task::datainfo), "task to do")
		;

	if (argc < 2)
	{
		LOG_ERROR("For the execution of " << prog << " there are too few command line arguments!");
		showHelp(desc);
	}

	program_options::variables_map args;

	try
	{
		program_options::store(program_options::parse_command_line(argc, argv, desc), args);
	}
	catch (program_options::error const& e)
	{
		LOG_ERROR(e.what());
		showHelp(desc);
		exit(EXIT_FAILURE);
	}

	program_options::notify(args);

	if (args.count("help")) {
		showHelp(desc);
	}

	if (args.count("task"))
	{
		LOG_INFO("Task is: " << args["task"].as< int >());
	}

	//if (args.count("collection-file"))
	//{
	//	LOG_INFO("Index files is: " << args["collection-file"].as< std::string >());
	//}


	//if (args.count("msb-directory"))
	//{
	//	LOG_INFO("MSB Directory is: " << args["msb-directory"].as< std::string >());
	//}

	//if (args.count("output-directory"))
	//{
	//	LOG_INFO("Output Directory is: " << args["output-directory"].as< std::string >());
	//}

	return args;
}
