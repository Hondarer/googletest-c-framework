#include <gmock/gmock.h>

#include <test_com.h>

#include <fstream>
#include <iostream>

using namespace std;

testing::AssertionResult testing::FileExists(const string &file_path)
{
    struct stat buffer;
    if (stat(file_path.c_str(), &buffer) == 0)
    {
        return testing::AssertionSuccess();
    }
    else
    {
        return testing::AssertionFailure() << "File " << file_path << " does not exist.";
    }
}

testing::AssertionResult testing::FileNotExists(const string &file_path)
{
    struct stat buffer;
    if (stat(file_path.c_str(), &buffer) != 0)
    {
        return testing::AssertionSuccess();
    }
    else
    {
        return testing::AssertionFailure() << "File " << file_path << " does exist.";
    }
}

testing::AssertionResult testing::FileContains(const string &file_path, const string &expected_content)
{
    ifstream file(file_path);
    if (!file.is_open())
    {
        return testing::AssertionFailure() << "File " << file_path << " does not exist.";
    }

    string line;
    while (getline(file, line))
    {
        if (line.find(expected_content) != string::npos)
        {
            return testing::AssertionSuccess();
        }
    }

    return testing::AssertionFailure() << "String \"" << expected_content << "\" not found in file " << file_path;
}
