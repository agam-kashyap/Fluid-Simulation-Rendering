#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include "timeutil.hpp"

class SherLog;
extern SherLog SherInstance;

#define SherLogInit (settings) SherInstance.init(settings);
#define SherLogShutdown() SherInstance.shutdown();
#define sherLog (SherInstance << SherInstance.prefix(__FILE__, __LINE__))

class SherLogSettings
{
    public:
        std::string path;
        std::string filename;

        bool b_overwriteFile;

        bool b_file;
        bool b_stdout;

        bool b_datetime;
        bool b_fileline;

        SherLogSettings()
        {
            this->path = "./SherLog/";
            this->filename = "Sher.log";
            this->b_overwriteFile = true;
            this->b_file = true;
            this->b_stdout = true;
            this->b_datetime = true;
            this->b_fileline = true;
        }
};

class SherLog
{
    public:
        SherLog();
        ~SherLog();

        bool init( const SherLogSettings& settings);
        void shutdown();

        template<typename T> SherLog& operator<<(T t);
        SherLog& operator << (std::ostream& (*fun)(std::ostream&));

        std::string prefix(const std::string& file, const int line);

    private:
        bool b_init;
        SherLogSettings settings;
        std::ofstream ofs;
};

template< typename T > inline SherLog& SherLog::operator<<( T t )
{
	if ( this->b_init )
	{
		if ( this->settings.b_file ) this->ofs << t;
		if ( this->settings.b_stdout ) std::cout << t;
	}
	return *this;
}

inline SherLog& SherLog::operator<<( std::ostream& (*fun)( std::ostream& ) )
{
	if ( this->b_init )
	{
		if ( this->settings.b_file ) this->ofs << std::endl;
		if ( this->settings.b_stdout ) std::cout << std::endl;
	}
	return *this;
}