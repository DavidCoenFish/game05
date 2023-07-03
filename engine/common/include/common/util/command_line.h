#pragma once

class CommandLine
{
public:
	static std::shared_ptr< CommandLine > Factory( const std::string& in_command_line );

private:
	class IToken
	{
	public:
		virtual const std::string& GetName() const = 0;
	};
	class TokenFlag : public IToken
	{
	public:
		explicit TokenFlag( const std::string& in_name = std::string());
		virtual const std::string& GetName() const override;
	private:
		std::string _name;
	};
	class TokenValueString : public IToken
	{
	public:
		explicit TokenValueString( const std::string& in_name = std::string(), const std::string& in_value = std::string() );
		virtual const std::string& GetName() const override;
		const std::string& GetValue() const;
	private:
		std::string _name;
		std::string _value;
	};
	class TokenValueInt : public IToken
	{
	public:
		explicit TokenValueInt( const std::string& in_name = std::string(), const int in_value = 0 );
		virtual const std::string& GetName() const override;
		const int GetValue() const;
	private:
		std::string _name;
		int _value;
	};
	class Tokeniser
	{
	public:
		Tokeniser(std::vector< std::shared_ptr< IToken > >& in_token_array);
		~Tokeniser();
		void AddCharacter(unsigned char in_c);
	private:
		void PushCharacter(unsigned char in_c);
		void Flush();
	private:
		std::vector< std::shared_ptr< IToken > >& _token_array;
		bool _in_quote;
		bool _in_token;
		bool _prev_quote;
		std::string _token;
		std::string _value;
	};

public:
	explicit CommandLine( const std::vector< std::shared_ptr< IToken > >& in_token_array = std::vector< std::shared_ptr< IToken > >() );
	~CommandLine();

	const int GetParamCount() const;
	const std::string GetParam(const int in_index) const;
	const bool GetFlag(const std::string& in_flag) const;
	const bool GetValueString(const std::string& in_token, std::string& out_value) const;
	const bool GetValueInt(const std::string& in_token, int& out_value) const;

private:
	std::vector< std::shared_ptr< IToken > > _param_array;
	std::map< std::string, std::shared_ptr< IToken > > _token_map;

};
