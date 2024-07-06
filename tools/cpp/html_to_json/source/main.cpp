#include "main.h"
#include "state.h"
#include "string_to_worksheet.h"
#include "worksheet_to_json.h"
#include "cursor.h"

#include <curl\curl.h>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <locale>
#include <codecvt>
#include <string>

/*
-i "C:\development\game05\tools\cpp\html_to_json\input\character_only.html" -o "C:\development\game05\tools\cpp\html_to_json\output\character.json" --sheet5th toc --dataSet en

-u https://docs.google.com/spreadsheets/d/e/2PACX-1vR2_sq9ul16UTDL72R36B5esxO7lH7F6A_LXuaycE3VLl3R-9OTGEyNeMrhQ4-gxwUfGTrRGzvjdiLw/pubhtml -o "C:\development\game05\tools\cpp\html_to_json\output\character_url.json" --sheet5th toc --dataSet en

https://stackoverflow.com/questions/53861300/how-do-you-properly-install-libcurl-for-use-in-visual-studio-2017
Get latest vcpkg zip file from https://github.com/microsoft/vcpkg/releases (e.g. https://github.com/microsoft/vcpkg/archive/2019.09.zip) and extract it to a folder of your choice (e.g. C:\vcpkg\)
Open Developer Command Prompt for VS 2017 (see Windows Start menu or %PROGRAMDATA%\Microsoft\Windows\Start Menu\Programs\Visual Studio 2017\Visual Studio Tools\) and cd to C:\vcpkg\
Run bootstrap-vcpkg.bat
Run vcpkg.exe integrate install
Run vcpkg.exe install curl
*/

namespace
{
    enum ApplicationExitCode : int
    {
        TNone = 0,
        TBadParam = -1,
        TOutputFileCreateFailed = -2,
        TOutputFileWriteFailed = -4,
        TInputFileFail = -8,
        TInputURLFail = -16,
        TUnimplemented = -32,
        TParseSource = -64,
        TWorksheetError = -128
    };

    void Help()
    {
        printf(R"(
usage:
	drive-json [options]
		
sample:
	html_to_json.exe --input ./input/character_only.html --sheet5th toc --outjson ./output/character.json
		add the data from the saved webpage at the given path in 3rd normalised form, and save result data as json file at output
		
options:
-h --help -?
	generate this text
-i --input <input filepath>
-u --url <input url>
-s --dataSet <lable>
	add lable to the allowed dataSet for filtering sheet3rdKeyValue sheets
-3 --sheet3rd <worksheet name>
	append the result data with data sourced from google spread sheet worksheet in 3rd normalised form and trated as id : object
-5 --sheet5th <worksheet name>
	append the result data with data sourced from google spread sheet worksheet in 5th normalised form and treated as key : value
-v --sheet3rdKeyValue <worksheet name>
	append the result data with data sourced from google spread sheet worksheet in 3rd normalised form filtered by dataset and treated as key : value
-o --outjson <output json filepath>
	specify the file location to save a json  representation of the result data
)");
    }

    const int DealInputFile(std::map<std::string, std::shared_ptr<Worksheet>>& out_source_data, const std::string& in_input_file_path)
    {
        printf("Input file:%s\n", in_input_file_path.c_str());

        //get the source string
        //https://stackoverflow.com/questions/20902945/reading-a-string-from-file-c
        std::string source_data;
        {
            const std::ifstream input_stream(in_input_file_path, std::ios_base::binary);

            if (input_stream.fail()) 
            {
                return ApplicationExitCode::TInputFileFail;
            }

            std::stringstream buffer;
            buffer << input_stream.rdbuf();

            source_data = buffer.str();
        }

        //convert source string to Worksheets
        if (false == StringToWorksheet::DealSourceData(out_source_data, source_data))
        {
            return ApplicationExitCode::TParseSource;
        }

        return ApplicationExitCode::TNone;
    }

    size_t WriteData(void *ptr, size_t size, size_t nmemb, void* source_data_raw)
    {
        std::string& source_data = *(std::string*)source_data_raw;
        size_t written = 0;
        //size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
        if (size == 1)
        {
            char* trace = (char*)ptr;
            for (int index = 0; index < nmemb; ++index)
            {
                source_data += trace[index];
                written += 1;
            }
        }

        return written;
    }

    const int DealInputUrl(std::map<std::string, std::shared_ptr<Worksheet>>& out_source_data, const std::string& in_input_url)
    {
        printf("Input url:%s\n", in_input_url.c_str());
        //get the source string
        std::string source_data;

        //https://curl.se/libcurl/c/url2file.html
        curl_global_init(CURL_GLOBAL_ALL);
        CURL *curl;
        curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, in_input_url.c_str());

        /* Switch on full protocol/debug output while testing */
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
 
        /* disable progress meter, set to 0L to enable it */
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &source_data);
        CURLcode res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);
        curl_global_cleanup();

        if (CURLE_OK != res)
        {
            return ApplicationExitCode::TInputURLFail;
        }

        //convert source string to Worksheets
        if (false == StringToWorksheet::DealSourceData(out_source_data, source_data))
        {
            return ApplicationExitCode::TParseSource;
        }

        return ApplicationExitCode::TNone;
    }

    const int DealOutput(const nlohmann::json& in_output_data, const std::string& in_output_file_path)
    {
        printf("Output:%s\n", in_output_file_path.c_str());

        std::ofstream outFile(in_output_file_path, std::ios::out | std::ios::binary | std::ios::ate);
        if (!outFile)
        {
            printf("output file creation failed:%s\n", in_output_file_path.c_str());
            return ApplicationExitCode::TOutputFileCreateFailed;
        }

        //const std::string data = in_output_data.template get<std::string>();
        const std::string data = in_output_data.dump(1, '\t');
        outFile.write(data.c_str(), data.size());
        if (!outFile)
        {
            printf("output file write failed:%s\n", in_output_file_path.c_str());
            return ApplicationExitCode::TOutputFileWriteFailed;
        }

        outFile.close();

        return ApplicationExitCode::TNone;
    }

}


int main(const int in_argc, const char* const in_argv[]) 
{
    int result = ApplicationExitCode::TNone;

    State state;

    // consume cmd line
    if (in_argc <= 1)
    {
        state.do_help = true;
    }
    else
    {
        for (int index = 1; index < in_argc; ++index)
        {
            const std::string token = in_argv[index];
            if ((token == "-h") || (token == "--help") || (token == "-?"))
            {
                state.do_help = true;
            }
            else if (((token == "-i") || (token == "--input")) && (index + 1 < in_argc))
            {
                index += 1;
                state.input_file_path = in_argv[index];
            }
            else if (((token == "-u") || (token == "--url")) && (index + 1 < in_argc))
            {
                index += 1;
                state.input_url = in_argv[index];
            }
            else if (((token == "-s") || (token == "--dataSet")) && (index + 1 < in_argc))
            {
                index += 1;
                state.data_set.insert(in_argv[index]);
            }
            else if (((token == "-3") || (token == "--sheet3rd")) && (index + 1 < in_argc))
            {
                index += 1;
                state.sheet_3rd_worksheet_name = in_argv[index];
            }
            else if (((token == "-5") || (token == "--sheet5th")) && (index + 1 < in_argc))
            {
                index += 1;
                state.sheet_5th_worksheet_name = in_argv[index];
            }
            else if (((token == "-v") || (token == "--sheet3rdKeyValue")) && (index + 1 < in_argc))
            {
                index += 1;
                state.sheet_3rd_key_value_worksheet_name = in_argv[index];
            }
            else if (((token == "-o") || (token == "--outjson")) && (index + 1 < in_argc))
            {
                index += 1;
                state.output_file_path = in_argv[index];
            }
            else
            {
                printf("unknown cmd line param:%s", token.c_str());
                state.do_help = true;
                result |= ApplicationExitCode::TBadParam;
            }
        }
    }

    // perform actions

    if (true == state.do_help)
    {
        Help();
    }

    // input
    if (false == state.input_file_path.empty())
    {
        result |= DealInputFile(state.source_data, state.input_file_path);
    }
    if (false == state.input_url.empty())
    {
        result |= DealInputUrl(state.source_data, state.input_url);
    }

    // process
    if (false == state.sheet_3rd_worksheet_name.empty())
    {
        if (false == WorksheetToJson::Deal3rd(
            state.output_data,
            state.source_data,
            state.sheet_3rd_worksheet_name,
            Cursor(state.data_set, false)
            ))
        {
            result |= TWorksheetError;
        }
    }
    if (false == state.sheet_5th_worksheet_name.empty())
    {
        if (false == WorksheetToJson::Deal5th(
            state.output_data,
            state.source_data,
            state.sheet_5th_worksheet_name,
            Cursor()
            ))
        {
            result |= TWorksheetError;
        }
    }
    if (false == state.sheet_3rd_key_value_worksheet_name.empty())
    {
        if (false == WorksheetToJson::Deal3rdKeyValue(
            state.output_data,
            state.source_data,
            state.sheet_3rd_key_value_worksheet_name,
            Cursor(state.data_set, true)
            ))
        {
            result |= TWorksheetError;
        }
    }

    // output
    if (false == state.output_file_path.empty())
    {
        result |= DealOutput(state.output_data, state.output_file_path);
    }

    return result;
}

