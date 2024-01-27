#include "common/common_pch.h"
#include "common/bezier/bezier_file_helper.h"

#include "common/bezier/bezier_curve.h"
#include "common/file_system/file_system.h"

#include "pugixml.hpp"

/*
namespace
{
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
    //    TInitial,

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

const bool BezierFileHelper::SegmentDataFromSvg(
    std::vector<BezierCurve::BezierSegment>& out_segment_data, 
    const std::filesystem::path& in_file_path
    )
{
    auto file_data = FileSystem::SyncReadFile(in_file_path);
    if ((nullptr == file_data) || (0 == file_data->size()))
    {
        return false;
    }
    const std::string file_string = FileSystem::DataToString(file_data);

    Tokenizer tokenizer(file_string);



    return true;
}

