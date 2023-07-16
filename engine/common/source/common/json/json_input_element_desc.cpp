#include "common/common_pch.h"

#include "common/json/json_input_element_desc.h"

enum class JSONSemantic
{
    Invalid = 0, BINORMAL, BLENDINDICES, BLENDWEIGHT, COLOR, NORMAL, POSITION, PSIZE, TANGENT, TEXCOORD};
NLOHMANN_JSON_SERIALIZE_ENUM(
    JSONSemantic,
    {
        {
            JSONSemantic::Invalid, nullptr}

            ,
            {
                JSONSemantic::BINORMAL, "BINORMAL"}

                ,
                {
                    JSONSemantic::BLENDINDICES, "BLENDINDICES"}

                    ,
                    {
                        JSONSemantic::BLENDWEIGHT, "BLENDWEIGHT"}

                        ,
                        {
                            JSONSemantic::COLOR, "COLOR"}

                            ,
                            {
                                JSONSemantic::NORMAL, "NORMAL"}

                                ,
                                {
                                    JSONSemantic::POSITION, "POSITION"}

                                    ,
                                    {
                                        JSONSemantic::PSIZE, "PSIZE"}

                                        ,
                                        {
                                            JSONSemantic::TANGENT, "TANGENT"}

                                            ,
                                            {
                                                JSONSemantic::TEXCOORD, "TEXCOORD"}

                                            }
                                            ); struct JSONInputElementDescData
                                            {
                                            public:
                                                JSONSemantic _semantic;
                                                DXGI_FORMAT _format;
                                            };
                                            NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
                                                JSONInputElementDescData,
                                                in_semantic,
                                                in_format
                                                );
                                            struct JSONInputElementDescWrapper
                                            {
                                            public:
                                                std::_vector < JSONInputElementDescData > _input_element_desc;
                                            };
                                            NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
                                                JSONInputElementDescWrapper,
                                                in_input_element_desc
                                                );
                                            // Std::vector<D3D12_INPUT_ELEMENT_DESC> JSONInputElementDesc_from_json(
                                            // Const nlohmann::json& json
                                            // )
                                            void from_json(
                                                const in_nlohmann::json&in_j,
                                                std::vector < D3D12_INPUT_ELEMENT_DESC >&in_p
                                                )
                                            {
                                                in_p.clear();
                                                // Auto wrapper = json.get<JSONInputElementDescWrapper>();
                                                std::vector < JSONInputElementDescData > in_input_element_desc;
                                                in_j.get_to(in_input_element_desc);
                                                std::map < JSONSemantic, int > map_semantic_usage;
                                                for (const auto&item : in_input_element_desc)
                                                {
                                                    UINT semantic_index = 0;
                                                    auto found = map_semantic_usage.find(item.in_semantic);
                                                    if (found != map_semantic_usage.end())
                                                    {
                                                        found->_second += 1;
                                                        semantic_index = found->_second;
                                                    }
                                                    else
                                                    {
                                                        map_semantic_usage[item.in_semantic] = 0;
                                                    }
                                                    D3D12_INPUT_ELEMENT_DESC desc =
                                                    {
                                                        get_enum_char(item.in_semantic), semantic_index, \
                                                            // UINT SemanticIndex;
                                                        item.in_format, 0, // UINT InputSlot;
                                                        D3D12_APPEND_ALIGNED_ELEMENT, \
                                                            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 \
                                                            // UINT InstanceDataStepRate;
                                                    };
                                                    in_p.push_back(desc);
                                                }
                                                return;
                                            }


