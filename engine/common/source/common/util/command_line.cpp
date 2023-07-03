#include "common/common_pch.h"
#include "common/util/command_line.h"
#include "common/log/log.h"

std::shared_ptr< CommandLine > CommandLine::Factory( const std::string& in_command_line )
{
	LOG_CONSOLE("CommandLine:");
	std::vector< std::shared_ptr< IToken > > token_array;
	{
		Tokeniser tokeniser(token_array);
		for (const auto character : in_command_line)
		{
			tokeniser.AddCharacter(character);
		}
	}

	return std::make_shared< CommandLine >(token_array);
}

CommandLine::TokenFlag::TokenFlag( const std::string& in_name)
: _name(in_name)
{
	//nop
}
const std::string& CommandLine::TokenFlag::GetName() const
{
	return _name;
}

CommandLine::TokenValueString::TokenValueString( const std::string& in_name, const std::string& in_value)
	: _name(in_name)
	, _value(in_value)
{
	//nop
}
const std::string& CommandLine::TokenValueString::GetName() const
{
	return _name;
}

const std::string& CommandLine::TokenValueString::GetValue() const
{
	return _value;
}

CommandLine::TokenValueInt::TokenValueInt( const std::string& in_name, const int in_value )
	: _name(in_name)
	, _value(in_value)
{
	//nop
}
const std::string& CommandLine::TokenValueInt::GetName() const
{
	return _name;
}
const int CommandLine::TokenValueInt::GetValue() const
{
	return _value;
}

CommandLine::Tokeniser::Tokeniser(std::vector< std::shared_ptr< IToken > >& in_token_array)
: _token_array(in_token_array)
, _in_quote(false)
, _in_token(true)
, _prev_quote(false)
, _token()
, _value()
{
	//nop
}

CommandLine::Tokeniser::~Tokeniser()
{
	Flush();
}

void CommandLine::Tokeniser::AddCharacter(unsigned char in_c)
{
	if (_in_quote)
	{
		if (in_c == '"')
		{
			_in_quote = false;
			_prev_quote = true;
		}
		else
		{
			PushCharacter(in_c);
			_prev_quote = false;
		}
	}
	else
	{
		if (0 != std::isspace(in_c))
		{
			Flush();
		}
		else if (in_c == '"')
		{
			if (true == _prev_quote)
			{
			PushCharacter('"');
			_prev_quote = false;
			}
			_in_quote = true;
		}
		else if (in_c == '=')
		{
			if (_in_token)
			{
			_in_token = false;
			}
			else
			{
			PushCharacter(in_c);
			}
		}
		else
		{
			PushCharacter(in_c);
		}
	}
}

void CommandLine::Tokeniser::PushCharacter(unsigned char in_c)
{
	if (_in_token)
	{
		_token += in_c;
	}
	else
	{
		_value += in_c;
	}
}

static const bool IsNumber(std::string& in_value, int& out_number)
{
	out_number = 0;
	bool bNegative = false;
	bool bAllDigits = true;
	int index = 0;
	if (in_value[index] == '-')
	{
		index += 1;
		bNegative = true;
		if (1 == (int)in_value.size())
		{
			return false;
		}
	}
	while(index < (int)in_value.size())
	{
		unsigned char character = in_value[index];
		index += 1;
		switch(character)
		{
		default:
			return false;
		case '0':
			out_number *= 10;
			break;
		case '1':
			out_number = (out_number * 10) + 1;
			break;
		case '2':
			out_number = (out_number * 10) + 2;
			break;
		case '3':
			out_number = (out_number * 10) + 3;
			break;
		case '4':
			out_number = (out_number * 10) + 4;
			break;
		case '5':
			out_number = (out_number * 10) + 5;
			break;
		case '6':
			out_number = (out_number * 10) + 6;
			break;
		case '7':
			out_number = (out_number * 10) + 7;
			break;
		case '8':
			out_number = (out_number * 10) + 8;
			break;
		case '9':
			out_number = (out_number * 10) + 9;
			break;
		}
	}

	if (true == bNegative)
	{
		out_number = -out_number;
	}
	return bAllDigits;
}


void CommandLine::Tokeniser::Flush()
{
	if (false == _token.empty())
	{
		int number = 0;
		if (true == _value.empty())
		{
			LOG_CONSOLE(" %s", _token.c_str());
			_token_array.push_back(std::make_shared< TokenFlag >( _token ));
		}
		else if (true == IsNumber(_value, number))
		{
			LOG_CONSOLE(" %s=%d", _token.c_str(), number);
			_token_array.push_back(std::make_shared< TokenValueInt >( _token, number ));
		}
		else
		{
			LOG_CONSOLE(" %s=%s", _token.c_str(), _value.c_str());
			_token_array.push_back(std::make_shared< TokenValueString >( _token, _value ));
		}
	}

	_token.clear();
	_value.clear();
	_in_quote = false;
	_in_token = true;
	_prev_quote = false;
}


CommandLine::CommandLine( const std::vector< std::shared_ptr< IToken > >& in_token_array )
	: _param_array(in_token_array)
{
	for(const auto& iter : in_token_array)
	{
		_token_map[iter->GetName()] = iter;
	}
}

CommandLine::~CommandLine()
{
	//nop
}

const int CommandLine::GetParamCount() const
{
	return (int)_param_array.size();
}
const std::string CommandLine::GetParam(const int in_index) const
{
	if ((0 <= in_index) && (in_index < (int)_param_array.size()))
	{
		return _param_array[in_index]->GetName();
	}
	return std::string();
}
const bool CommandLine::GetFlag(const std::string& in_flag) const
{
	auto found = _token_map.find(in_flag);
	return (found != _token_map.end());
}

const bool CommandLine::GetValueString(const std::string& in_token, std::string& out_value) const
{
	auto found = _token_map.find(in_token);
	if (found != _token_map.end())
	{
		auto token_value = dynamic_cast< TokenValueString* >( found->second.get() );
		if (nullptr != token_value)
		{
			out_value = token_value->GetValue();
			return true;
		}
	}
	return false;
}

const bool CommandLine::GetValueInt(const std::string& in_token, int& out_value) const
{
	auto found = _token_map.find(in_token);
	if (found != _token_map.end())
	{
		auto pTokenValue = dynamic_cast< TokenValueInt* >( found->second.get() );
		if (nullptr != pTokenValue)
		{
			out_value = pTokenValue->GetValue();
			return true;
		}
	}
	return false;
}
