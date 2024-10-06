#include "common/common_pch.h"
#include "common/bezier/bezier_file_helper.h"

#include "common/bezier/bezier_curve.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"

#include "pugixml.hpp"

namespace
{
	const VectorFloat2 ConsumeVector(int& out_index, const std::vector<std::string>& in_token_array)
	{
		VectorFloat2 result;
		if (out_index + 1 < in_token_array.size())
		{
			result[0] = std::stof(in_token_array[out_index + 0]);
			result[1] = std::stof(in_token_array[out_index + 1]);
		}
		out_index += 2;
		return result;
	}
	void ConsumeCurveTo(
		int& out_index, 
		const std::vector<std::string>& in_token_array, 
		const VectorFloat2& cursor, 
		std::vector<BezierCurve::BezierSegment>& out_segment_data,
		const bool in_absolute
		)
	{
		bool absolute = in_absolute;
		while (out_index + 5 < in_token_array.size())
		{
			BezierCurve::BezierSegment segment;
			const VectorFloat2 v0 = ConsumeVector(out_index, in_token_array);
			const VectorFloat2 v1 = ConsumeVector(out_index, in_token_array);
			const VectorFloat2 v2 = ConsumeVector(out_index, in_token_array);

			segment._p0 = cursor; 
			segment._p1 = (true == in_absolute) ? v0 : cursor + v0;
			// our shader assumes v0 == v1
			segment._p2 = (true == in_absolute) ? v2 : cursor + v2; 

			segment._line_thickness_p0 = 1.0f;
			segment._line_thickness_p2 = 1.0f;
			out_segment_data.push_back(segment);

			if (out_index < in_token_array.size())
			{
				const std::string peek_next = in_token_array[out_index];
				if ((peek_next == "m") || (peek_next == "M"))
				{
					return;
				}
				if (peek_next == "c")
				{
					out_index += 1;
					absolute = false;
				}
				if (peek_next == "C")
				{
					out_index += 1;
					absolute = true;
				}
			}
		}

	}

	class Tokeniser
	{
	public:
		Tokeniser(std::vector<std::string>& out_string_array)
		: _string_array(out_string_array)
		{
			// nop
		}
		~Tokeniser()
		{
			if (false == _current.empty())
			{
				_string_array.push_back(_current);
				_current = std::string();
			}
		}

		void AddCharacter(const char in_character)
		{
			if ((',' == in_character) || (0 != std::isspace(in_character)))
			{
				if (false == _current.empty())
				{
					_string_array.push_back(_current);
					_current = std::string();
				}
			}
			else
			{
				_current += in_character;
			}
		}

	private:
		std::string _current;
		std::vector<std::string>& _string_array;
	};

	class SimpleWalker: public pugi::xml_tree_walker
	{
	public:
		SimpleWalker(std::vector<BezierCurve::BezierSegment>& out_segment_data)
			: _segment_data(out_segment_data)
		{
			// nop
		}

		void AppendPath(const pugi::char_t* const in_value)
		{
			if (nullptr == in_value)
			{
				return;
			}

			std::vector<std::string> token_array;
			{
				Tokeniser tokeniser(token_array);
				for (const auto character : std::string(in_value))
				{
					tokeniser.AddCharacter(character);
				}
			}

			//example "m 115.41665,140.47261 c 8,-12 4,-16 0,-16 -4,0 -3.57791,2.66042 -3.57791,2.66042"
			//"m 100,99.999997 c 100,0 100,0 100,100.000003 0,100 0,99.99999 -100,100.16393"
			//"M 0,0 C 98.999999,0 100,0 100,99.999997"
			VectorFloat2 cursor = _last_move;
			int index = 0;
			while (index < token_array.size())
			{
				const std::string& token = token_array[index];
				if (token == "m")
				{
					index += 1;
					cursor = _last_move + ConsumeVector(index, token_array);
				}
				else if (token == "M")
				{
					index += 1;
					_last_move = ConsumeVector(index, token_array);
					cursor = _last_move;
				}
				else if (token == "c")
				{
					index += 1;
					ConsumeCurveTo(index, token_array, cursor, _segment_data, false);
				}
				else if (token == "C")
				{
					index += 1;
					ConsumeCurveTo(index, token_array, cursor, _segment_data, true);
				}
				else if (token == "Z")
				{
					index += 1;
					return;
				}
				else if (token == "z")
				{
					index += 1;
					return;
				}
				else
				{
					LOG_MESSAGE_WARNING("parse bezier svg path, unknown token [%s]", token_array[index].c_str());
					index += 1;
				}
			}
		}

		virtual bool for_each(pugi::xml_node& node)
		{
			if (0 == strcmp("path", node.name()))
			{
				pugi::xml_attribute attribute = node.attribute("d");
				AppendPath(attribute.value());
			}

			return true; // continue traversal
		}
	private:
		std::vector<BezierCurve::BezierSegment>& _segment_data;
		VectorFloat2 _last_move;

	};

/*
	class Tokenizer;

	struct XmlEscapeValues
	{
		const char* const key;
		const char* const value;
	};

	//https://www.liquid-technologies.com/Reference/Glossary/XML_EscapingData.html#:~:text=Attribute%20Data,-When%20attribute%20data&text=The%20ampersand%20%26%20character%20must%20be,good%20practice%20to%20do%20it.&text=The%20double%20quotes%20in%20the%20data%20must%20be%20escaped.&text=The%20double%20quotes%20do%20not,within%20a%20single%20quoted%20attribute.
	const XmlEscapeValues s_xml_escape_values[] = {
		{ "&lt;", "<" },
		{ "&gt;", ">" },
		{ "&quot;", "\"" },
		{ "&apos;", "'" },
		{ "&amp;", "&" },
	};
	const int s_xml_escape_values_count = ARRAY_LITERAL_SIZE(s_xml_escape_values);

	// why am i writing a minimal xml parser? because we jsut want the SVG path data
	enum class DSCTokenXml
	{
		TNone,
		TData, // white space seperated string, could be tag, attribute name, element data
		TString,
		TMetaStart, // <?
		TMetaEnd, // ?>
		TCommentStart, // <!--
		TComment,
		TCommentEnd, // -->
		TElementStart, // <
		TElementStartClose, // </
		TElementEnd, // >
		TElementEndClose, // />
		TAttributeAssign, // =

		TError
	};

	//enum class DSCTokenMode
	//{
	//	TInitial,

	//}

	template <typename TOKEN_KEY>
	class Tokenizer;

	const Tokenizer<DSCTokenXml> FactoryXml(const std::string& in_string)
	{
		return Tokenizer<DSCTokenXml>(
			in_string,
			std::vector<std::pair<std::string, DSCTokenXml>>({
				{"<?", DSCTokenXml::TMetaStart},
				{"?>", DSCTokenXml::TMetaEnd},
				{"<!--", DSCTokenXml::TCommentStart},
				{"-->", DSCTokenXml::TCommentEnd},
				{"</", DSCTokenXml::TElementStartClose},
				{"<", DSCTokenXml::TElementStart},
				{"/>", DSCTokenXml::TElementEndClose},
				{">", DSCTokenXml::TElementEnd},
				{"=", DSCTokenXml::TAttributeAssign},
			}),
			'\"',
			std::vector<std::pair<std::string, std::string>>({
				{ "&lt;", "<" },
				{ "&gt;", ">" },
				{ "&quot;", "\"" },
				{ "&apos;", "'" },
				{ "&amp;", "&" },
			}),
			DSCTokenXml::TString,
			DSCTokenXml::TData,
			DSCTokenXml::TCommentStart,
			DSCTokenXml::TComment,
			DSCTokenXml::TCommentEnd,
			DSCTokenXml::TError
			);
	}

	template <typename TOKEN_KEY>
	class Tokenizer
	{
	public:
		Tokenizer(
			const std::string& in_string,
			const std::vector<std::pair<std::string, TOKEN_KEY>>& in_token_map, // the given sequences unless in a string, are to be treated as a token
			const char in_string_char, // if you hit this char, it is a string till you hit it again, 
			const std::vector<std::pair<std::string, std::string>>& in_string_escape,
			const TOKEN_KEY in_quote_string_token_key,
			const TOKEN_KEY in_data_token_key, // anything not a token, string or comment
			const TOKEN_KEY in_comment_start,
			const TOKEN_KEY in_comment,
			const TOKEN_KEY in_comment_end,
			const TOKEN_KEY in_error
			)
			: _string(in_string)
			, _token_map(in_token_map)
			, _string_char(in_string_char)
			, _string_escape(in_string_escape)
			, _quote_string_token_key(in_quote_string_token_key)
			, _data_token_key(in_data_token_key)
			, _comment_start(in_comment_start)
			, _comment(in_comment)
			, _comment_end(in_comment_end)
			, _error(in_error)
			, _cursor(0)
			, _previous_token(DSCTokenType::TInitial)
		{
			// Nop
		}

		const bool IsWhitespace(const char in_value)
		{
			if ((' ' == in_value) || ('\n' == in_value) || ('\t' == in_value))
			{
				return true;
			}
			return false;
		}

		const std::string ConsumeString()
		{
			std::string result;
			_cursor += 1;
			while (_cursor < _string.length())
			{
				//unescape the string

				const char value = _string[_cursor];
				_cursor += 1;

				if (value == _string_char)
				{
					break;
				}
				else
				{
					result += value;
				}
			}
			return result;
		}

		// return true if any token/ data/ comment/ string is found
		const bool ConsumeToken(
			std::string& out_token,
			TOKEN_KEY& out_token_type
			)
		{
			out_token.clear();
			out_token_type = DSCTokenType::TError;

			if (_string.length() <= _cursor)
			{
				return false;
			}

			int cursor_delta = 0;
			while (_cursor < _string.length())
			{
				TOKEN_KEY token_key = _error;
				int step = 0;
				std::string token;
				if (true == LookForToken(token_key, step, token))
				{
					if (_previous_token == _comment_start) 
					{
						if (token_key == _comment_end)
						{
							out_token_type = _comment;
							_previous_token = out_token_type; 
							return true;
						}
						out_token += token;
						_cursor += step;
						continue;
					}

					out_token_type = token_key;
					_previous_token = out_token_type; 
					out_token = token;
					_cursor += step;
					return true;
				}

				const char value = _string[_cursor];

				if (value == _string_char)
				{
					out_token = ConsumeString();
					out_token_type = _quote_string_token_key;
					_previous_token = out_token_type; 
					return true;
				}

				if (true == out_token.empty())
				{
					if (true == IsWhitespace(value))
					{
						_cursor += 1;
						continue;
					}
					out_token += value;
					_cursor += 1;
					continue;
				}
				else
				{
					if (true == IsWhitespace(value))
					{
						_cursor += 1;
						continue;
					}

					// hit whitespace end of data?
					_cursor += 1;
					break;
				}
			}

			if (false == out_token.empty())
			{
				out_token_type = _data;
				_previous_token = out_token_type; 
				return true;
			}

			return false;
		}

	private:
		const std::string& _string;
		int _cursor;
		DSCTokenType _previous_token;
	};
}
*/
}

const bool BezierFileHelper::SegmentDataFromSvg(
	std::vector<BezierCurve::BezierSegment>& out_segment_data, 
	const std::filesystem::path& in_file_path
	)
{
#if 0
	auto file_data = FileSystem::SyncReadFile(in_file_path);
	if ((nullptr == file_data) || (0 == file_data->size()))
	{
		return false;
	}
	const std::string file_string = FileSystem::DataToString(file_data);

	Tokenizer tokenizer(file_string);
#else
	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file(in_file_path.wstring().c_str());

	if (pugi::status_ok != result.status)
	{
		LOG_MESSAGE_ERROR("error loading %s [%s]", in_file_path.string().c_str(), result.description());
		return false;
	}

	SimpleWalker simple_walker(out_segment_data);
	doc.traverse(simple_walker);

	//for (pugi::xml_node_iterator it = doc.begin(); it != doc.end(); ++it)
	//{
	//	LOG_CONSOLE(" xml:%s %d", it->name(), static_cast<int>(it->type()));
	//}

	//std::cout << "Load result: " << result.description() << ", mesh name: " << doc.child("mesh").attribute("name").value() << std::endl;


#endif
	return true;
}

