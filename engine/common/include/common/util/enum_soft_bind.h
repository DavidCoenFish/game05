#pragma once

template<class ENUM>
class EnumSoftBind
{
public:
    static const ENUM EnumFromString(const std::string& in_string);
    static const std::string EnumToString(const ENUM in_enum);

private:
    EnumSoftBind()=delete;
    EnumSoftBind(const EnumSoftBind&)=delete;
    EnumSoftBind(EnumSoftBind&&)=delete;

};


// https://stackoverflow.com/questions/1723537/template-specialization-of-a-single-method-from-a-templated-class
// Declaration (header), where ENUM is you enum class, 
//template <> 
//const ENUM EnumSoftBind<ENUM>::EnumFromString(const std::string& in_locale);
//template <> 
//const const std::string EnumSoftBind<ENUM>::EnumToString(const ENUM in_locale);

// Definition (source)
//template <>
//const ENUM EnumSoftBind<ENUM>::EnumFromString(const std::string& in_locale) {...}
//template <> 
//const const std::string EnumSoftBind<ENUM>::EnumToString(const ENUM in_locale) {...}
