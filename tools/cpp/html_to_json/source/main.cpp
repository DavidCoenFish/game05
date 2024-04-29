#include "main.h"
#include "state.h"
#include "string_to_worksheet.h"

#include <fstream>
#include <sstream>
#include <stdio.h>
#include <locale>
#include <codecvt>
#include <string>

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
        TParseSource = -64
    };

    void Help()
    {
        printf(R"(
usage:
	drive-json [options]
		
sample:
	html_to_json.exe --input ./input/character_only.html --sheet3rd toc --outjson ./output/character.json
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

    const int DealInputUrl(std::map<std::string, std::shared_ptr<Worksheet>>& out_source_data, const std::string& in_input_url)
    {
        printf("Input url:%s\n", in_input_url.c_str());
        //get the source string
        std::string source_data;

        //convert source string to Worksheets
        if (false == StringToWorksheet::DealSourceData(out_source_data, source_data))
        {
            return ApplicationExitCode::TParseSource;
        }

        //return ApplicationExitCode::TNone;
        return ApplicationExitCode::TInputURLFail;
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

    if (false == state.input_file_path.empty())
    {
        result |= DealInputFile(state.source_data, state.input_file_path);
    }

    if (false == state.input_url.empty())
    {
        result |= DealInputUrl(state.source_data, state.input_url);
    }




    if (false == state.output_file_path.empty())
    {
        result |= DealOutput(state.output_data, state.output_file_path);
    }

    return result;
}

