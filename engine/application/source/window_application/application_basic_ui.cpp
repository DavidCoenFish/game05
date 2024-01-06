#include "application_pch.h"

#include "window_application/application_basic_ui.h"
#include "build.h"
#include "default_locale.h"
#include "default_ui_component_factory.h"

#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/i_resource.h"
#include "common/file_system/file_system.h"
#include "common/locale/locale_system.h"
#include "common/log/log.h"
#include "common/macro.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_int4.h"
#include "common/text/text_manager.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"
#include "common/ui/i_ui_model.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_component/ui_component_canvas.h"
#include "common/ui/ui_component/ui_component_string.h"
#include "common/ui/ui_data/ui_data.h"
#include "common/ui/ui_data/ui_data_button.h"
#include "common/ui/ui_data/ui_data_canvas.h"
#include "common/ui/ui_data/ui_data_combo_box.h"
#include "common/ui/ui_data/ui_data_combo_box_layer.h"
#include "common/ui/ui_data/ui_data_disable.h"
#include "common/ui/ui_data/ui_data_slider.h"
#include "common/ui/ui_data/ui_data_list_box.h"
#include "common/ui/ui_data/ui_data_manual_scroll.h"
#include "common/ui/ui_data/ui_data_scroll.h"
#include "common/ui/ui_data/ui_data_stack.h"
#include "common/ui/ui_data/ui_data_string.h"
#include "common/ui/ui_data/ui_data_text_run.h"
#include "common/ui/ui_data/ui_data_toggle.h"
#include "common/ui/ui_data/ui_data_tooltip_layer.h"
#include "common/ui/ui_effect/ui_effect_data.h"
#include "common/util/timer.h"
#include "common/util/vector_helper.h"
#include "common/window/window_application_param.h"

#if defined(DRAW_SYSTEM_RESOURCE_ALIVE_COUNT)
extern int s_draw_system_resource_alive_count;
#endif

class UIModel;

namespace
{
    constexpr float s_default_margin = 8.0f;
/*
    std::vector<std::shared_ptr<UIData>> BuildButtonData(const std::string& in_body)
    {
        return std::vector<std::shared_ptr<UIData>>({

            std::make_shared<UIData>(
                "effect_gloss",
                std::vector<std::shared_ptr<UIData>>({

                    std::make_shared<UIData>(
                        "effect_fill",
                        std::vector<std::shared_ptr<UIData>>({

                            std::make_shared<UIData>(
                                "effect_corner",
                                std::vector<std::shared_ptr<UIData>>({
                                    std::make_shared<UIData>(
                                        "canvas_grey"
                                    )
                                })
                            )
                        })
                    )
                })
            ),

            std::make_shared<UIData>(
                "effect_drop_glow",
                std::vector<std::shared_ptr<UIData>>({
                std::make_shared<UIData>(
                    "UIData",
                    std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIDataString>(
                            in_body,
                            LocaleISO_639_1::Default,
                            "string_middle_em"
                        )
            }))}))
        });
    }

    std::vector<std::shared_ptr<UIData>> BuildCheckboxTrue()
    {
        return std::vector<std::shared_ptr<UIData>>({

            //std::make_shared<UIData>(
            //    "effect_drop_shadow",
            //    std::vector<std::shared_ptr<UIData>>({

            std::make_shared<UIData>(
                "effect_gloss",
                std::vector<std::shared_ptr<UIData>>({

                    std::make_shared<UIData>(
                        "effect_fill",
                        std::vector<std::shared_ptr<UIData>>({

                            std::make_shared<UIData>(
                                "UIData",
                                std::vector<std::shared_ptr<UIData>>({

                            //std::make_shared<UIData>(
                            //    "canvas_margin_tiny",
                            //    std::vector<std::shared_ptr<UIData>>({

                            std::make_shared<UIData>(
                                "effect_corner",
                                std::vector<std::shared_ptr<UIData>>({
                                    std::make_shared<UIData>(
                                        "canvas_grey"
                                    )
                                })
                            )
                            //    })
                            //)
                                })
                            )
                        })
                    )
                        })
                //    )
                //})
            ),

            std::make_shared<UIData>(
                "effect_drop_glow",
                std::vector<std::shared_ptr<UIData>>({
                std::make_shared<UIData>(
                    "UIData",
                    std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIDataString>(
                            "\xe2""\x9c""\x93",
                            LocaleISO_639_1::Default,
                            "string_middle"
                            )
                        })
                    )
                })
            )
        });
    }

    std::vector<std::shared_ptr<UIData>> BuildCheckboxFalse()
    {
        return std::vector<std::shared_ptr<UIData>>();
    }

    std::shared_ptr<UIData> BuildSliderButton(
        const std::shared_ptr<UIDataSlider>& in_data_float, 
        const float in_step_mul,
        const std::string& in_text
        )
    {
        return std::make_shared<UIDataButton>(
                [in_data_float, in_step_mul](const VectorFloat2&){
                    if (nullptr != in_data_float)
                    {
                        const float step = in_data_float->GetStep() * in_step_mul;
                        const float value = in_data_float->GetValue() + step;
                        in_data_float->SetValue(value);
                    }
                }, 
                [in_data_float](){
                    std::stringstream stream;
                    stream << std::fixed;
                    stream << std::setprecision(1);
                    stream << in_data_float->GetValue();
                    std::string stream_string = stream.str();
                    return stream_string;
                },
                true,
                "button_default",
                std::vector<std::shared_ptr<UIData>>({

                    std::make_shared<UIData>(
                        "effect_gloss",
                        std::vector<std::shared_ptr<UIData>>({

                            std::make_shared<UIData>(
                                "effect_fill",
                                std::vector<std::shared_ptr<UIData>>({

                                    std::make_shared<UIData>(
                                        "effect_corner",
                                        std::vector<std::shared_ptr<UIData>>({
                                            std::make_shared<UIData>(
                                                "canvas_grey"
                                            )
                                        })
                                    )
                                })
                            )
                        })
                    ),
                    std::make_shared<UIData>(
                        "effect_drop_glow",
                        std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIData>(
                            "UIData",
                            std::vector<std::shared_ptr<UIData>>({
                                std::make_shared<UIDataString>(
                                    in_text,
                                    LocaleISO_639_1::Default,
                                    "string_middle"
                                    )
                                })
                            )
                        })
                    )
                })
            );
    }

    std::shared_ptr<UIData> BuildSliderKnot()
    {
#if 0
        return std::make_shared<UIData>(
            "canvas_widget"
            );

#else
        return std::make_shared<UIData>(
        "canvas_knot",
            std::vector<std::shared_ptr<UIData>>({
                //std::make_shared<UIData>(
                //    "effect_drop_shadow",
                //    std::vector<std::shared_ptr<UIData>>({

                        std::make_shared<UIData>(
                            "effect_gloss",
                            std::vector<std::shared_ptr<UIData>>({

                                std::make_shared<UIData>(
                                    "effect_fill",
                                    std::vector<std::shared_ptr<UIData>>({

                                        std::make_shared<UIData>(
                                            "UIData",
                                            std::vector<std::shared_ptr<UIData>>({

                                        //std::make_shared<UIData>(
                                        //    "canvas_margin_tiny",
                                        //    std::vector<std::shared_ptr<UIData>>({

                                        std::make_shared<UIData>(
                                            "effect_corner",
                                            std::vector<std::shared_ptr<UIData>>({
                                                std::make_shared<UIData>(
                                                    "canvas_grey"
                                                )
                                            })
                                        )
                                //    })
                                //)
                            })
                        )
                    })
                                )
                            })

                        )
                //    })
                //)
            })
        );
#endif
    }

    std::shared_ptr<UIData> BuildCheckbox(const std::shared_ptr<UIDataToggle>& in_data_toggle)
    {
        std::shared_ptr<UIData> result = std::make_shared<UIData>(
            "canvas_checkbox_wrapper",
            std::vector<std::shared_ptr<UIData>>({

                std::make_shared<UIData>(
                    "effect_drop_shadow_small",
                    std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIData>(
                            "UIData",
                            std::vector<std::shared_ptr<UIData>>({
                                std::make_shared<UIData>(
                                    "canvas_margin_tiny",

                                    std::vector<std::shared_ptr<UIData>>({
                                        std::make_shared<UIData>(
                                            "effect_corner_hollow_tiny",
                                            std::vector<std::shared_ptr<UIData>>({
                                                std::make_shared<UIData>(
                                                    "canvas_grey"
                                                    )
                                                })
                                            ),

                                            in_data_toggle

                                        })
                                    )
                                })
                            )
                        })
                    )
                })
            );
        return result;
    }

    std::shared_ptr<UIData> BuildScrollKnot()
    {
#if 0
        return std::make_shared<UIData>(
            "canvas_blue"
            );
#else
        return std::make_shared<UIData>(
            "effect_gloss",
            std::vector<std::shared_ptr<UIData>>({
                std::make_shared<UIData>(
                    "effect_fill",
                    std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIData>(
                            "effect_corner",
                            std::vector<std::shared_ptr<UIData>>({
                                std::make_shared<UIData>(
                                    "canvas_grey"
                                    )
                                })
                            )
                        })
                    )
                })
            );
#endif
    }

    std::shared_ptr<UIData> BuildManualScroll(
        const bool in_allow_vertical, 
        const bool in_allow_horizontal,
        std::map<std::string, std::shared_ptr<UIData>>& in_data_map,
        const std::string& in_root_name,
        const std::shared_ptr<UIData>& in_subject
        )
    {
        DSC_ASSERT((true == in_allow_vertical) || (true == in_allow_horizontal), "need to have either or both be true");

        std::shared_ptr<UIData> horizontal_scroll_wrapper;
        std::shared_ptr<UIDataScroll> horizontal_scroll;
        std::shared_ptr<UIData> vertical_scroll_wrapper;
        std::shared_ptr<UIDataScroll> vertical_scroll;

        if (true == in_allow_horizontal)
        {
            const std::string scroll_template = in_allow_vertical ? "canvas_manual_scroll_horizontal" : "canvas_manual_scroll_horizontal_long";
            const std::string name = in_root_name + std::string("_horizontal");
            auto scroll_knot = BuildScrollKnot();
            horizontal_scroll = std::make_shared<UIDataScroll>(
                VectorFloat2(),
                VectorFloat2(),
                [&in_data_map, name](const VectorFloat2& in_value)
                {
                    auto data = std::dynamic_pointer_cast<UIDataScroll>(in_data_map[name]);
                    if (nullptr != data)
                    {
                        data->SetValue(in_value);
                    }
                },
                [&in_data_map, name](const VectorFloat2& in_value)
                {
                    auto data = std::dynamic_pointer_cast<UIDataScroll>(in_data_map[name]);
                    if (nullptr != data)
                    {
                        data->SetRange(in_value);
                    }
                },
                scroll_knot,
                "scroll_horizontal",
                std::vector<std::shared_ptr<UIData>>({
                    scroll_knot
                    })
            );
            in_data_map[name] = horizontal_scroll;

            horizontal_scroll_wrapper = std::make_shared<UIData>(
                scroll_template,
                std::vector<std::shared_ptr<UIData>>({
                    std::make_shared<UIData>(
                        "effect_drop_shadow_small",
                        std::vector<std::shared_ptr<UIData>>({
                            std::make_shared<UIData>(
                                "UIData",
                                std::vector<std::shared_ptr<UIData>>({
                                    horizontal_scroll
                                    })
                                )
                            })
                        )
                    })
                );
        }

        if (true == in_allow_vertical)
        {
            const std::string scroll_template = in_allow_horizontal ? "canvas_manual_scroll_vertical" : "canvas_manual_scroll_vertical_long";

            const std::string name = in_root_name + std::string("_vertical");
            auto scroll_knot = BuildScrollKnot();

            vertical_scroll = std::make_shared<UIDataScroll>(
                VectorFloat2(),
                VectorFloat2(),
                [&in_data_map, name](const VectorFloat2& in_value)
                {
                    auto data = std::dynamic_pointer_cast<UIDataScroll>(in_data_map[name]);
                    if (nullptr != data)
                    {
                        data->SetValue(in_value);
                    }
                },
                [&in_data_map, name](const VectorFloat2& in_value)
                {
                    auto data = std::dynamic_pointer_cast<UIDataScroll>(in_data_map[name]);
                    if (nullptr != data)
                    {
                        data->SetRange(in_value);
                    }
                },
                scroll_knot,
                "scroll_vertical",
                std::vector<std::shared_ptr<UIData>>({
                    scroll_knot
                    })
            );
            in_data_map[name] = vertical_scroll;

            vertical_scroll_wrapper = std::make_shared<UIData>(
                scroll_template,
                std::vector<std::shared_ptr<UIData>>({
                    std::make_shared<UIData>(
                        "effect_drop_shadow_small",
                        std::vector<std::shared_ptr<UIData>>({
                            std::make_shared<UIData>(
                                "UIData",
                                std::vector<std::shared_ptr<UIData>>({
                                    vertical_scroll
                                    })
                                )
                            })
                        )
                    })
                );

        }

        return std::make_shared<UIDataManualScroll>(
            horizontal_scroll,
            vertical_scroll,
            "UIDataManualScroll",
            std::vector<std::shared_ptr<UIData>>({
                in_subject,
                horizontal_scroll_wrapper,
                vertical_scroll_wrapper
                })
            );
    }

    std::shared_ptr<UIData> BuildListBox(
        std::map<std::string, std::shared_ptr<UIData>>& in_data_map,
        const std::string& in_root_name,
        const std::vector<std::shared_ptr<UIData>>& in_array_items
        )
    {
        std::vector<std::shared_ptr<UIData>> content_array;
        std::vector<std::shared_ptr<UIDataButton>> item_host_button_array;
        for (auto& item : in_array_items)
        {
            auto data_button = std::make_shared<UIDataButton>(
                nullptr,
                nullptr,
                false,
                "button_listbox_item"
                );

            auto listbox_item = std::make_shared<UIData>(
                "canvas_row",
                std::vector<std::shared_ptr<UIData>>({
                    data_button,
                    item
                    })
                );

            content_array.push_back(listbox_item);
            item_host_button_array.push_back(data_button);
        }

        auto list_box = std::make_shared<UIDataListBox>(
                    [in_root_name, &in_data_map](int in_selected_index){
                        auto data = std::dynamic_pointer_cast<UIDataListBox>(in_data_map[in_root_name]);
                        if (nullptr != data)
                        {
                            data->SetSelectedIndex(in_selected_index);
                        }
                    },
                    1,
                    item_host_button_array,
                    "UIDataListBox",
                    std::vector<std::shared_ptr<UIData>>({
                        BuildManualScroll(
                            true,
                            false,
                            in_data_map,
                            in_root_name + "_manual",
                            std::make_shared<UIData>(
                                "stack_list_box", //"stack", //, //"stack_top_down",
                                content_array
                                )
                            )
                        })
                );
        in_data_map[in_root_name] = list_box;

        return std::make_shared<UIData>(
            "UIData",
            std::vector<std::shared_ptr<UIData>>({

                std::make_shared<UIData>(
                    "canvas_margin_listbox",
                    std::vector<std::shared_ptr<UIData>>({
                        list_box
                        })
                    ),

                std::make_shared<UIData>(
                    "effect_drop_shadow_small",
                    std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIData>(
                            "UIData",
                            std::vector<std::shared_ptr<UIData>>({
                                std::make_shared<UIData>(
                                    "canvas_margin_tiny",

                                    std::vector<std::shared_ptr<UIData>>({
                                            std::make_shared<UIData>(
                                                "effect_corner_hollow_tiny",
                                                std::vector<std::shared_ptr<UIData>>({
                                                    std::make_shared<UIData>(
                                                        "canvas_grey"
                                                        )
                                                    })
                                                )

                                        })
                                    )
                                })
                            )
                        })
                    )
                })
            );
    }

    std::shared_ptr<UIData> BuildTooltipLayer()
    {
        std::vector<std::shared_ptr<UIData>> array_child_data;
        UIDataTooltipLayer::TTooltipLayoutTargetArray tooltip_layout_target_array;
        std::vector<std::shared_ptr<UIDataTextRun>> text_run_array;
        for (int index = 0; index < UIDataTooltipLayer::Variables::TMaxTooltipCount; ++index)
        {
            auto text_run = std::make_shared<UIDataTextRun>(
                "<Colour 1 1 1 1>Hello Human", 
                LocaleISO_639_1::Default,
                "text_run_tooltip"
                );
            text_run_array.push_back(text_run);
#if 0
            tooltip_layout_target_array[index] = text_run;
            array_child_data.push_back(text_run);
#elif 1
            auto contents = 

            std::make_shared<UIData>("effect_drop_shadow_small_shrink",
                std::vector<std::shared_ptr<UIData>>({
                    std::make_shared<UIData>("canvas_transparent_margin_shrink",
                        std::vector<std::shared_ptr<UIData>>({
                            std::make_shared<UIData>("canvas_grey_margin_shrink",
                                std::vector<std::shared_ptr<UIData>>({
                                    text_run
                                })
                            )
                        })
                    )
                })
            );

            tooltip_layout_target_array[index] = contents;
            array_child_data.push_back(contents);
#else
            auto contents = std::make_shared<UIData>("UIData", 
                std::vector<std::shared_ptr<UIData>>({
                    std::make_shared<UIData>(
                        "effect_drop_shadow_small",
                        std::vector<std::shared_ptr<UIData>>({
                            std::make_shared<UIData>(
                                "UIData",
                                std::vector<std::shared_ptr<UIData>>({
                                    std::make_shared<UIData>(
                                        "canvas_margin_tiny",

                                        std::vector<std::shared_ptr<UIData>>({
                                            std::make_shared<UIData>(
                                                "effect_corner_tiny",
                                                std::vector<std::shared_ptr<UIData>>({
                                                    text_run
                                                })
                                            )
                                        })
                                    )
                                })
                            )
                        })
                    )
                })
            );
            tooltip_layout_target_array[index] = contents;
            array_child_data.push_back(contents);
#endif
        }

        UIDataTooltipLayer::TOnTooltipChange on_tooltip_change = [text_run_array](const int in_tooltip_index, const VectorFloat2&, const VectorFloat4&, const std::string& in_tooltip){
            if ((in_tooltip_index < 0) || (text_run_array.size() <= in_tooltip_index))
            {
                return;
            }
            text_run_array[in_tooltip_index]->SetMarkupStringUtf8(in_tooltip);
            return;
        };

        return std::make_shared<UIDataTooltipLayer>(
            tooltip_layout_target_array,
            on_tooltip_change,
            UITooltipType::TRelativeToTouch,
            1.0f, 
            "UIDataTooltipLayer",
            array_child_data
            );
    }

    std::shared_ptr<UIData> BuildComboBoxLayer(
        std::map<std::string, std::shared_ptr<UIData>>& in_data_map,
        const std::string& in_root_name
        )
    {
        auto dismiss_button = std::make_shared<UIDataButton>(
                [&in_data_map, in_root_name](const VectorFloat2&){
                    UIDataComboBoxLayer* const layer = dynamic_cast<UIDataComboBoxLayer*>(in_data_map[in_root_name].get());
                    if (nullptr != layer)
                    {
                        layer->SetActive(false);
                    }
                    },
                [](){
                    return std::string("<Locale close_combo_box_dropdown>");
                },
                false,
                "button_background"
                );

        return std::make_shared<UIDataComboBoxLayer>(
            [&in_data_map, in_root_name](){
                UIDataComboBoxLayer* const layer = dynamic_cast<UIDataComboBoxLayer*>(in_data_map[in_root_name].get());
                if (nullptr != layer)
                {
                    layer->SetActive(true);
                }
            },
            false,
            dismiss_button,
            "UIDataComboBoxLayer",
            std::vector<std::shared_ptr<UIData>>({
                dismiss_button
                })
            );
    }

    std::shared_ptr<UIData> BuildComboBox(
        std::map<std::string, std::shared_ptr<UIData>>& in_data_map,
        const std::string& in_combo_box_layer_name,
        const std::string& in_root_name,
        std::vector<std::shared_ptr<UIData>>& in_array_items
        )
    {
        std::shared_ptr<UIData> parent_selected_item;
        std::shared_ptr<UIDataListBox> dropdown_listbox;
        std::shared_ptr<UIData> child_data_dropdown;
        std::vector<std::shared_ptr<UIData>> content_array;
        std::vector<std::shared_ptr<UIDataButton>> item_host_button_array;
        for (auto& item : in_array_items)
        {
            auto data_button = std::make_shared<UIDataButton>(
                nullptr,
                nullptr,
                false,
                "button_listbox_item"
                );

            auto listbox_item = std::make_shared<UIData>(
                "canvas_row",
                std::vector<std::shared_ptr<UIData>>({
                    data_button,
                    item
                    })
                );

            content_array.push_back(listbox_item);
            item_host_button_array.push_back(data_button);
        }

        parent_selected_item = std::make_shared<UIData>();

        dropdown_listbox = std::make_shared<UIDataListBox>(
                [in_root_name, in_combo_box_layer_name, &in_data_map](int in_selected_index){
                    auto data = std::dynamic_pointer_cast<UIDataListBox>(in_data_map[in_root_name + "_list_box"]);
                    if (nullptr != data)
                    {
                        data->SetSelectedIndex(in_selected_index);
                    }

                    auto combo_box = std::dynamic_pointer_cast<UIDataComboBox>(in_data_map[in_root_name + "_combo_box"]);
                    if (nullptr != combo_box)
                    {
                        combo_box->OnSelectItem(in_selected_index);
                    }

                    auto layer = std::dynamic_pointer_cast<UIDataComboBoxLayer>(in_data_map[in_combo_box_layer_name]);
                    if (nullptr != layer)
                    {
                        layer->SetActive(false);
                    }
                },
                1,
                item_host_button_array,
                "UIDataListBox",
                std::vector<std::shared_ptr<UIData>>({
                    BuildManualScroll(
                        true,
                        false,
                        in_data_map,
                        in_root_name + "_manual",
                        std::make_shared<UIData>(
                            "combo_box_list_box",
                            content_array
                            )
                        )
                    })
            );

        std::vector<std::shared_ptr<UIData>> content_array_combo = std::vector<std::shared_ptr<UIData>>({

            std::make_shared<UIData>(
                "effect_gloss",
                std::vector<std::shared_ptr<UIData>>({

                    std::make_shared<UIData>(
                        "effect_fill",
                        std::vector<std::shared_ptr<UIData>>({

                            std::make_shared<UIData>(
                                "effect_corner",
                                std::vector<std::shared_ptr<UIData>>({
                                    std::make_shared<UIData>(
                                        "canvas_grey"
                                    )
                                })
                            )
                        })
                    )
                })
            ),

            std::make_shared<UIData>(
                "effect_drop_glow",
                std::vector<std::shared_ptr<UIData>>({
                std::make_shared<UIData>(
                    "UIData",
                    std::vector<std::shared_ptr<UIData>>({
                        parent_selected_item
            }))}))
        });

        in_data_map[in_root_name + "_list_box"] = dropdown_listbox;

        // TODO: put some sort of frame, dropshadow around listbox
        child_data_dropdown = dropdown_listbox;

        return std::make_shared<UIDataComboBox>(
            parent_selected_item,
            dropdown_listbox,
            "UIDataComboBox",
            content_array_combo,
            child_data_dropdown,
            in_array_items
            );
    }
*/

    void BuildModelData00_OneRedQuad(
        std::map<std::string, std::shared_ptr<UIData>>& in_out_data_map
        )
    {
        auto data_main = std::make_shared<UIData>(
#ifdef _DEBUG
            "Root data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryRoot(true)
            );

        data_main->AddChild(
            std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
                "child",
#endif
                UILayout::FactoryParentMiddleQuater(),
                UIBaseColour::FactoryRedBackground()
                )
            );

        in_out_data_map["main"] = data_main;
        //in_out_data_array_map["main"] = std::vector<std::shared_ptr<UIData>>({
        //    data_main
        //    });
    }

    void BuildModelData01_ChildRedQuad(
        std::map<std::string, std::shared_ptr<UIData>>& in_out_data_map
        )
    {
        auto data_main = std::make_shared<UIData>(
#ifdef _DEBUG
            "Root data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryRoot(true)
            );
        auto child_0 = std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
            "child full screen transparent canvas",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryDefault()
            );

        data_main->AddChild(
            child_0
            );

        child_0->AddChild(
            std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
                "child quater screen red canvas",
#endif
                UILayout::FactoryParentMiddleQuater(),
                UIBaseColour::FactoryRedBackground()
                )
            );

        in_out_data_map["main"] = data_main;
    }

    void BuildModelData010_ChildRedMargin(
        std::map<std::string, std::shared_ptr<UIData>>& in_out_data_map
        )
    {
        auto data_main = std::make_shared<UIData>(
#ifdef _DEBUG
            "Root data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryRoot(true)
            );
        auto child_0 = std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
            "child 0 blue with margin",
#endif
            UILayout(
                UICoord(UICoord::TSource::ParentX, 1.0f, 0.0f),
                UICoord(UICoord::TSource::ParentY, 1.0f, 0.0f),
                VectorFloat2::s_zero,
                UILayout::TAdjustmentType::GrowTextureToLayout,
                UILayout::TAdjustmentType::GrowTextureToLayout,
                VectorInt4(8,16,32,64) // left, top, right, bottom
            ),
            UIBaseColour::FactoryBlueBackground()
            );

        data_main->AddChild(
            child_0
            );

        child_0->AddChild(
            std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
                "child 1 red",
#endif
                UILayout::FactoryFull(),
                UIBaseColour::FactoryRedBackground()
                )
            );

        child_0->AddChild(
            std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
                "child 2 green top right",
#endif
                UILayout(
                    UICoord(UICoord::TSource::None, 0.0f, 32.0f),
                    UICoord(UICoord::TSource::None, 0.0f, 32.0f),
                    VectorFloat2(1.0f, 1.0f)
                    ),
                UIBaseColour::FactoryGreenBackground()
                )
            );

        child_0->AddChild(
            std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
                "child 3 green top left",
#endif
                UILayout(
                    UICoord(UICoord::TSource::None, 0.0f, 32.0f),
                    UICoord(UICoord::TSource::None, 0.0f, 32.0f),
                    VectorFloat2(0.0f, 1.0f)
                    ),
                UIBaseColour::FactoryGreenBackground()
                )
            );

        child_0->AddChild(
            std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
                "child 4 green bottom right",
#endif
                UILayout(
                    UICoord(UICoord::TSource::None, 0.0f, 32.0f),
                    UICoord(UICoord::TSource::None, 0.0f, 32.0f),
                    VectorFloat2(1.0f, 0.0f)
                    ),
                UIBaseColour::FactoryGreenBackground()
                )
            );

        child_0->AddChild(
            std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
                "child 5 green bottom left",
#endif
                UILayout(
                    UICoord(UICoord::TSource::None, 0.0f, 32.0f),
                    UICoord(UICoord::TSource::None, 0.0f, 32.0f),
                    VectorFloat2(0.0f, 0.0f)
                    ),
                UIBaseColour::FactoryGreenBackground()
                )
            );

        in_out_data_map["main"] = data_main;
    }

    void BuildModelData02_Text(
        std::map<std::string, std::shared_ptr<UIData>>& in_out_data_map
        )
    {
        auto data_main = std::make_shared<UIData>(
#ifdef _DEBUG
            "Root data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryRoot(true)
            );

        auto data_string = std::make_shared<UIDataString>(
#ifdef _DEBUG
            "String data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryDefault(),
            UITintColour::FactoryBlack(),
            UIData::s_empty_effect_data_array,
            "hello human",
            LocaleISO_639_1::Default,
            false,
            TextEnum::HorizontalLineAlignment::Middle,
            TextEnum::VerticalBlockAlignment::Middle
            );

        data_main->AddChild(data_string);

        in_out_data_map["main"] = data_main;
    }

    void BuildModelData020_TextAlignment(
        std::map<std::string, std::shared_ptr<UIData>>& in_out_data_map
        )
    {
        auto data_main = std::make_shared<UIData>(
#ifdef _DEBUG
            "Root data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryRoot(true)
            );

        struct ChildData
        {
            const char* const _text;
            const float _attach[2];
            const TextEnum::HorizontalLineAlignment _horizontal;// = TextEnum::HorizontalLineAlignment::Left,
            const TextEnum::VerticalBlockAlignment _vertical;// = TextEnum::VerticalBlockAlignment::Bottom

        };
        const ChildData data_array[] = {
            { "g top left", { 0.0, 1.0 }, TextEnum::HorizontalLineAlignment::Left, TextEnum::VerticalBlockAlignment::Top },
            { "g top middle", { 0.5, 1.0 }, TextEnum::HorizontalLineAlignment::Middle, TextEnum::VerticalBlockAlignment::Top },
            { "g top right", { 1.0, 1.0 }, TextEnum::HorizontalLineAlignment::Right, TextEnum::VerticalBlockAlignment::Top },

            { "g middle left", { 0.0, 0.5 }, TextEnum::HorizontalLineAlignment::Left, TextEnum::VerticalBlockAlignment::Middle },
            { "g middle middle", { 0.5, 0.5 }, TextEnum::HorizontalLineAlignment::Middle, TextEnum::VerticalBlockAlignment::Middle },
            { "g middle right", { 1.0, 0.5 }, TextEnum::HorizontalLineAlignment::Right, TextEnum::VerticalBlockAlignment::Middle },

            { "g bottom left", { 0.0, 0.0 }, TextEnum::HorizontalLineAlignment::Left, TextEnum::VerticalBlockAlignment::Bottom },
            { "g bottom middle", { 0.5, 0.0 }, TextEnum::HorizontalLineAlignment::Middle, TextEnum::VerticalBlockAlignment::Bottom },
            { "g bottom right", { 1.0, 0.0 }, TextEnum::HorizontalLineAlignment::Right, TextEnum::VerticalBlockAlignment::Bottom },

            { "g em top", { 0.25, 1.0 }, TextEnum::HorizontalLineAlignment::Middle, TextEnum::VerticalBlockAlignment::TopEM },
            { "g em middle", { 0.25, 0.5 }, TextEnum::HorizontalLineAlignment::Middle, TextEnum::VerticalBlockAlignment::MiddleEM },
            { "g em bottom", { 0.25, 0.0 }, TextEnum::HorizontalLineAlignment::Middle, TextEnum::VerticalBlockAlignment::BottomEM },
        };
        const int data_array_count = ARRAY_LITERAL_SIZE(data_array);

        for (int index = 0; index < data_array_count; ++index)
        {
            auto data_child = std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
                std::string("child_") + std::to_string(index),
#endif
                UILayout(
                    UICoord(UICoord::TSource::None, 0.0f, 192.0f),
                    UICoord(UICoord::TSource::None, 0.0f, 64.0f),
                    VectorFloat2(data_array[index]._attach[0], data_array[index]._attach[1])
                    ),
                UIBaseColour::FactoryDefault()
                );

            data_child->AddChild(
                std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
                    std::string("grand_child_") + std::to_string(index),
#endif
                    UILayout::FactoryFull(),
                    UIBaseColour::FactoryDefault(),
                    UITintColour::FactoryDefault(),
                    std::vector<std::shared_ptr<UIEffectData>>({
                        std::make_shared<UIEffectData>(
                            UIEffectEnum::TDebugGrid
                            )
                        })
                    )
                );

            data_child->AddChild(std::make_shared<UIDataString>(
#ifdef _DEBUG
                std::string("string_") + std::to_string(index),
#endif
                UILayout::FactoryFull(),
                UIBaseColour::FactoryDefault(),
                UITintColour::FactoryBlack(),
                UIData::s_empty_effect_data_array,
                data_array[index]._text,
                LocaleISO_639_1::Default,
                false,
                data_array[index]._horizontal,
                data_array[index]._vertical
                ));

            data_main->AddChild(data_child);
        }

        in_out_data_map["main"] = data_main;
    }

    void BuildModelData03_ShrunkText(
        std::map<std::string, std::shared_ptr<UIData>>& in_out_data_map
        )
    {
        auto data_main = std::make_shared<UIData>(
#ifdef _DEBUG
            "Root data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryRoot(true)
            );


        auto data_text = std::make_shared<UIDataString>(
#ifdef _DEBUG
            "String data",
#endif
            UILayout(
                UICoord(UICoord::TSource::ParentX, 1.0f),
                UICoord(UICoord::TSource::ParentY, 1.0f),
                VectorFloat2(0.5f, 0.5f),
                UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                VectorInt4(8, 8, 8, 8)
                ),
            UIBaseColour::FactoryRedBackground(),
            UITintColour::FactoryDefault(),
            UIData::s_empty_effect_data_array,
            "hello human",
            LocaleISO_639_1::Default,
            false,
            TextEnum::HorizontalLineAlignment::Middle,
            TextEnum::VerticalBlockAlignment::Middle
            );

        data_main->AddChild(
            data_text
            );

        in_out_data_map["main"] = data_main;
    }

    void BuildModelData04_ShrunkTextChildMargin(
        std::map<std::string, std::shared_ptr<UIData>>& in_out_data_map
        )
    {
        auto data_main = std::make_shared<UIData>(
#ifdef _DEBUG
            "Root data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryRoot(true)
            );


        auto data_margin = std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
            "child canvas",
#endif
            UILayout(
                UICoord(UICoord::TSource::ParentX, 1.0f),
                UICoord(UICoord::TSource::ParentY, 1.0f),
                VectorFloat2(0.5f, 0.5f),
                UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                VectorInt4(8, 8, 8, 8)
                ),
            UIBaseColour::FactoryBlueBackground()
            );

        auto data_text = std::make_shared<UIDataString>(
#ifdef _DEBUG
            "child string",
#endif
            UILayout(
                UICoord(UICoord::TSource::ParentX, 1.0f),
                UICoord(UICoord::TSource::ParentY, 1.0f),
                VectorFloat2(0.5f, 0.5f),
                UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                VectorInt4(8, 8, 8, 8)
                ),
            UIBaseColour::FactoryRedBackground(),
            UITintColour::FactoryDefault(),
            UIData::s_empty_effect_data_array,
            "hello human",
            LocaleISO_639_1::Default,
            false,
            TextEnum::HorizontalLineAlignment::Middle,
            TextEnum::VerticalBlockAlignment::Middle
            );

        data_main->AddChild(
            data_margin
            );
        data_margin->AddChild(
            data_text
            );

        in_out_data_map["main"] = data_main;
    }

    void BuildModelData05_Effect(
        std::map<std::string, std::shared_ptr<UIData>>& in_out_data_map
        )
    {
        auto data_main = std::make_shared<UIData>(
#ifdef _DEBUG
            "Root data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryRoot(true)
            );

        auto data_canvas = std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
            "canvas data",
#endif
            UILayout::FactoryParentMiddleQuater(),
            UIBaseColour::FactoryRedBackground(),
            UITintColour::FactoryDefault(),
            std::vector<std::shared_ptr<UIEffectData>>({
                std::make_shared<UIEffectData>(
                    UIEffectEnum::TRoundCorners,
                    // Top right, top left, bottom left, bottom right
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 32),
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 32),
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 32),
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 32)
                    )
                })
            );
        data_main->AddChild(data_canvas);

        in_out_data_map["main"] = data_main;
    }

    void BuildModelData050_CornerMargin(
        std::map<std::string, std::shared_ptr<UIData>>& in_out_data_map
        )
    {
        auto data_main = std::make_shared<UIData>(
#ifdef _DEBUG
            "Root data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryRoot(true)
            );

        //TRoundCorners, // data[bottom left, top left, Top right, bottom right] reference, margin is [left, top, right, bottom]

        auto data_canvas = std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
            "canvas data",
#endif
            UILayout(
                UICoord(UICoord::TSource::ParentX, 1.0f),
                UICoord(UICoord::TSource::ParentY, 1.0f),
                VectorFloat2(0.5f, 0.5f),
                UILayout::TAdjustmentType::GrowTextureToLayout,
                UILayout::TAdjustmentType::GrowTextureToLayout,
                VectorInt4(8,16,32,64) // left, top, right, bottom
            ),
            UIBaseColour::FactoryRedBackground(),
            UITintColour::FactoryDefault(),
            std::vector<std::shared_ptr<UIEffectData>>({
                std::make_shared<UIEffectData>(
                    UIEffectEnum::TRoundCorners,
                    //TRoundCorners, // data[bottom left, top left, Top right, bottom right] reference, margin is [left, top, right, bottom]
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 64),
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 32),
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 16),
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 8)
                    )
                })
            );
        data_main->AddChild(data_canvas);

        in_out_data_map["main"] = data_main;
    }

    void BuildModelData06_Tooltip(
        std::map<std::string, std::shared_ptr<UIData>>& in_out_data_map
        )
    {
        auto data_main = std::make_shared<UIData>(
#ifdef _DEBUG
            "Root data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryRoot(true),
            UITintColour::FactoryFade(10.0f)
            );


        auto data_tooltip = std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
            "canvas data",
#endif
            UILayout(
                UICoord(UICoord::TSource::ParentX, 1.0f),
                UICoord(UICoord::TSource::ParentY, 1.0f),
                VectorFloat2(0.5f, 0.0f),
                UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                VectorInt4(8, 8, 8, 8)
                ),
            UIBaseColour::FactoryDefault(),
            UITintColour::FactoryDefault(),
            std::vector<std::shared_ptr<UIEffectData>>({
                std::make_shared<UIEffectData>(
                    UIEffectEnum::TDropShadow,
                    // offset x, offset y, radius
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 2),
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 2),
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 6),
                    UICoord(),
                    nullptr, //tint_array
                    VectorFloat4::s_black
                    )
                })
            );

        auto data_text = std::make_shared<UIDataString>(
#ifdef _DEBUG
            "string data",
#endif
            UILayout(
                UICoord(UICoord::TSource::ParentX, 1.0f),
                UICoord(UICoord::TSource::ParentY, 1.0f),
                VectorFloat2(0.5f, 0.5f),
                UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                VectorInt4(8, 8, 8, 8)
                ),
            UIBaseColour::FactoryGreyBackground(),
            UITintColour::FactoryDefault(),
            UIData::s_empty_effect_data_array,
            "hello human",
            LocaleISO_639_1::Default,
            false,
            TextEnum::HorizontalLineAlignment::Middle,
            TextEnum::VerticalBlockAlignment::Middle
            );

        data_main->AddChild(
            data_tooltip
            );
        data_tooltip->AddChild(
            data_text
            );


        in_out_data_map["main"] = data_main;
    }

    void BuildModelData07_Stack(
        std::map<std::string, std::shared_ptr<UIData>>& in_out_data_map
        )
    {
        auto data_main = std::make_shared<UIData>(
#ifdef _DEBUG
            "Root data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryRoot(true)
            );

        auto data_stack = std::make_shared<UIDataStack>(
#ifdef _DEBUG
            "stack data",
#endif
            UILayout(
                UICoord(UICoord::TSource::ParentX, 0.5f, 0.0f),
                UICoord(UICoord::TSource::ParentY, 0.5f, 0.0f),
                VectorFloat2(0.5f, 0.5f),
                UILayout::TAdjustmentType::GrowTextureToLayout,
                UILayout::TAdjustmentType::GrowTextureToLayout,
                VectorInt4(8,8,8,8) // left, top, right, bottom
                ),
            //::FactoryParentMiddleQuater(),
            UIBaseColour::FactoryBlueBackground(),
            UITintColour::FactoryDefault(),
            UIData::s_empty_effect_data_array,
            UIOrientation::TVertical,
            UICoord(UICoord::TSource::None, 0.0f, 4.0f)
            );

        for (int index = 0; index < 3; index++)
        {
            auto data_text = std::make_shared<UIDataString>(
#ifdef _DEBUG
                "string data",
#endif
                UILayout(
                    UICoord(UICoord::TSource::ParentX, 1.0f),
                    UICoord(UICoord::TSource::ParentY, 1.0f),
                    VectorFloat2(0.0f, 0.0f),
                    UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                    UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                    VectorInt4(8, 8, 8, 8)
                    ),
                UIBaseColour::FactoryRedBackground(),
                UITintColour::FactoryDefault(),
                UIData::s_empty_effect_data_array,
                "hello human",
                LocaleISO_639_1::Default,
                false,
                TextEnum::HorizontalLineAlignment::Middle,
                TextEnum::VerticalBlockAlignment::Middle
                );

            data_stack->AddChild(data_text);
        }
        data_main->AddChild(data_stack);

        in_out_data_map["main"] = data_main;
    }

    void BuildModelData070_StackMargin(
        std::map<std::string, std::shared_ptr<UIData>>& in_out_data_map
        )
    {
        auto data_main = std::make_shared<UIData>(
#ifdef _DEBUG
            "Root data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryRoot(true)
            );

        auto data_stack = std::make_shared<UIDataStack>(
#ifdef _DEBUG
            "stack data",
#endif
            UILayout(
                UICoord(UICoord::TSource::ParentX, 1.0f, 0.0f),
                UICoord(UICoord::TSource::ParentY, 1.0f, 0.0f),
                VectorFloat2(0.0f, 0.0f),
                UILayout::TAdjustmentType::GrowTextureToLayout,
                UILayout::TAdjustmentType::GrowTextureToLayout,
                VectorInt4(8,16,32,64), // left, top, right, bottom
                VectorFloat2(0.5f, 0.5f)
                ),
            UIBaseColour::FactoryBlueBackground(),
            UITintColour::FactoryDefault(),
            UIData::s_empty_effect_data_array,
            UIOrientation::TVertical,
            UICoord(UICoord::TSource::None, 0.0f, 4.0f)
            );

        const char* text_data[] = {"0 short", "1 medium message", "2 a very long text string that goes on and on"};
        for (int index = 0; index < 3; index++)
        {
            auto data_text = std::make_shared<UIDataString>(
#ifdef _DEBUG
                "string data",
#endif
                UILayout(
                    UICoord(UICoord::TSource::ParentX, 1.0f),
                    UICoord(UICoord::TSource::ParentY, 1.0f),
                    VectorFloat2(0.5f, 0.0f),
                    UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                    UILayout::TAdjustmentType::ShrinkLayoutToTexture
                    ),
                UIBaseColour::FactoryRedBackground(),
                UITintColour::FactoryDefault(),
                UIData::s_empty_effect_data_array,
                text_data[index],
                LocaleISO_639_1::Default,
                false,
                TextEnum::HorizontalLineAlignment::Middle,
                TextEnum::VerticalBlockAlignment::Top
                );

            data_stack->AddChild(data_text);
        }
        data_main->AddChild(data_stack);

        in_out_data_map["main"] = data_main;
    }

    void BuildModelData08_Button(
        std::map<std::string, std::shared_ptr<UIData>>& in_out_data_map
        )
    {
        auto tint_array = std::make_shared<UIData::TStateFlagTintArray>(UIData::TStateFlagTintArray({
            VectorFloat4(1.0f, 0.0f, 0.0f, 1.0f),
            VectorFloat4(0.0f, 1.0f, 0.0f, 1.0f),
            VectorFloat4(0.0f, 0.0f, 1.0f, 1.0f),
            VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f),
            VectorFloat4(0.0f, 1.0f, 1.0f, 1.0f),
            VectorFloat4(1.0f, 0.0f, 1.0f, 1.0f),
            VectorFloat4(1.0f, 1.0f, 0.0f, 1.0f),
            VectorFloat4(1.0f, 1.0f, 1.0f, 1.0f)
            }));

        auto data_main = std::make_shared<UIData>(
#ifdef _DEBUG
            "Root data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryRoot(true)
            );

        auto data_button = std::make_shared<UIDataButton>(
#ifdef _DEBUG
            "button data",
#endif
            UILayout::FactoryParentMiddleQuater(),
            UIBaseColour::FactoryDefault(),
            UITintColour::FactoryDefault()
            );

        auto data_fill = std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
            "fill data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryGreyBackground(),
            UITintColour::FactoryDefault(),
            std::vector<std::shared_ptr<UIEffectData>>({
                std::make_shared<UIEffectData>(
                    UIEffectEnum::TRoundCorners,
                    // Top right, top left, bottom left, bottom right
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 32.0f),
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 32.0f),
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 32.0f),
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 32.0f)
                    ),
                std::make_shared<UIEffectData>(
                    UIEffectEnum::TFill,
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 1.0f),
                    UICoord(),
                    UICoord(),
                    UICoord(),
                    tint_array
                    )
                })
            );

//        auto data_text = std::make_shared<UIDataString>(
        auto data_text = std::make_shared<UIDataTextRun>(
#ifdef _DEBUG
                "string data",
#endif
                UILayout(
                    UICoord(UICoord::TSource::ParentX, 1.0f),
                    UICoord(UICoord::TSource::ParentY, 1.0f),
                    VectorFloat2(0.5f, 0.5f),
                    UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                    UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                    VectorInt4(8,8,8,8)
                    ),
                UIBaseColour::FactoryDefault(),
                UITintColour::FactoryDefault(),
#if 0
                UIData::s_empty_effect_data_array,
#else
                std::vector<std::shared_ptr<UIEffectData>>({
                        std::make_shared<UIEffectData>(
                            UIEffectEnum::TDropShadow,
                            UICoord(UICoord::TSource::ParentMin, 0.0f, 2),
                            UICoord(UICoord::TSource::ParentMin, 0.0f, 2),
                            UICoord(UICoord::TSource::ParentMin, 0.0f, 6),
                            UICoord(),
                            nullptr,
                            VectorFloat4::s_black
                            )
                    }),
#endif
                "<Colour 1.0 1.0 1.0 1.0><Size 64>HELLO HUMAN",
                LocaleISO_639_1::Default,
                false,
                TextEnum::HorizontalLineAlignment::Middle,
                TextEnum::VerticalBlockAlignment::Middle
            );

        data_button->AddChild(data_fill);
        data_button->AddChild(data_text);
        data_main->AddChild(data_button);

        in_out_data_map["main"] = data_main;
        return;
    }

    std::shared_ptr<UIData> BuildButton(
        const std::string& in_text,
        std::shared_ptr<UIData::TStateFlagTintArray>& in_tint_array,
        const UILayout& in_layout
        )
    {
        auto data_button = std::make_shared<UIDataButton>(
#ifdef _DEBUG
            "button data",
#endif
            in_layout,
            UIBaseColour::FactoryDefault(),
            UITintColour::FactoryDefault(),
            std::vector<std::shared_ptr<UIEffectData>>({
                    std::make_shared<UIEffectData>(
                        UIEffectEnum::TDropShadow,
                        UICoord(UICoord::TSource::ParentMin, 0.0f, 2),
                        UICoord(UICoord::TSource::ParentMin, 0.0f, 2),
                        UICoord(UICoord::TSource::ParentMin, 0.0f, 6),
                        UICoord(),
                        nullptr,
                        VectorFloat4::s_black
                        )
                })
            );

        auto data_fill = std::make_shared<UIDataCanvas>(
#ifdef _DEBUG
            "fill data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryGreyBackground(),
            UITintColour::FactoryDefault(),
            std::vector<std::shared_ptr<UIEffectData>>({
                std::make_shared<UIEffectData>(
                    UIEffectEnum::TRoundCorners,
                    // Top right, top left, bottom left, bottom right
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 8.0f),
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 8.0f),
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 8.0f),
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 8.0f)
                    ),
                std::make_shared<UIEffectData>(
                    UIEffectEnum::TFill,
                    UICoord(UICoord::TSource::ParentMin, 0.0f, 1.0f),
                    UICoord(),
                    UICoord(),
                    UICoord(),
                    in_tint_array
                    ),
                std::make_shared<UIEffectData>(
                    UIEffectEnum::TGloss
                    )
                })
            );

//        auto data_text = std::make_shared<UIDataString>(
        auto data_text = std::make_shared<UIDataTextRun>(
#ifdef _DEBUG
                "string data",
#endif
                UILayout(
                    UICoord(UICoord::TSource::ParentX, 1.0f),
                    UICoord(UICoord::TSource::ParentY, 1.0f),
                    VectorFloat2(0.5f, 0.5f),
                    UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                    UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                    VectorInt4(8,8,8,8)
                    ),
                UIBaseColour::FactoryDefault(),
                UITintColour::FactoryDefault(),

                std::vector<std::shared_ptr<UIEffectData>>({
                        std::make_shared<UIEffectData>(
                            UIEffectEnum::TDropShadow,
                            UICoord(UICoord::TSource::ParentMin, 0.0f, 2),
                            UICoord(UICoord::TSource::ParentMin, 0.0f, 2),
                            UICoord(UICoord::TSource::ParentMin, 0.0f, 6),
                            UICoord(),
                            nullptr,
                            VectorFloat4::s_black
                            )
                    }),

                in_text, //"<Colour 1.0 1.0 1.0 1.0><Size 64>HELLO HUMAN",
                LocaleISO_639_1::Default,
                false,
                TextEnum::HorizontalLineAlignment::Middle,
                TextEnum::VerticalBlockAlignment::Middle
            );

        data_button->AddChild(data_fill);
        data_button->AddChild(data_text);

        return data_button;
    }

    void BuildModelData09_NestedButton(
        std::map<std::string, std::shared_ptr<UIData>>& in_out_data_map
        )
    {
        auto tint_array = std::make_shared<UIData::TStateFlagTintArray>(UIData::TStateFlagTintArray({
            VectorFloat4(0.0f, 0.0f, 1.0f, 1.0f), // TNone
            VectorFloat4(0.2f, 0.2f, 1.0f, 1.0f), // THover
            VectorFloat4(0.0f, 0.8f, 0.7f, 1.0f), // TTouch
            VectorFloat4(0.0f, 0.4f, 0.9f, 1.0f), // TTouch THover
            VectorFloat4(1.0f, 0.0f, 0.0f, 1.0f), // TSelected
            VectorFloat4(1.0f, 0.2f, 0.2f, 1.0f), // TSelected THover
            VectorFloat4(0.7f, 0.8f, 0.0f, 1.0f), // TSelected TTouch
            VectorFloat4(0.9f, 0.4f, 0.0f, 1.0f) // TSelected TTouch THover
            }));

        auto data_main = std::make_shared<UIData>(
#ifdef _DEBUG
            "Root data",
#endif
            UILayout::FactoryFull(),
            UIBaseColour::FactoryRoot(true)
            );

        auto data_stack = std::make_shared<UIDataStack>(
#ifdef _DEBUG
            "stack data",
#endif
            UILayout(
                UICoord(UICoord::TSource::ParentX, 0.5f, 0.0f),
                UICoord(UICoord::TSource::ParentY, 0.5f, 0.0f),
                VectorFloat2(0.5f, 0.5f),
                UILayout::TAdjustmentType::GrowTextureToLayout,
                UILayout::TAdjustmentType::GrowTextureToLayout,//,
                VectorInt4(8,8,8,8) // left, top, right, bottom
                ),
            //::FactoryParentMiddleQuater(),
            UIBaseColour::FactoryBlueBackground(),
            UITintColour::FactoryDefault(),
            UIData::s_empty_effect_data_array,
            UIOrientation::TVertical,
            UICoord(UICoord::TSource::None, 0.0f, 4.0f)
            );

        auto data_button_0 = BuildButton(
            "Button 0",
            tint_array,
            UILayout(
                UICoord(UICoord::TSource::ParentX, 0.5f, 0.0f),
                UICoord(UICoord::TSource::None, 0.5f, 32.0f),
                VectorFloat2(0.5f, 0.5f),
                UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                UILayout::TAdjustmentType::ShrinkLayoutToTexture,
                VectorInt4(8, 8, 8, 8) //left, top, right, bottom
            ));
#if 1
        data_stack->AddChild(data_button_0);
        data_stack->AddChild(BuildButton(
            "Button 1",
            tint_array,
            UILayout(
                UICoord(UICoord::TSource::ParentX, 0.5f, 0.0f),
                UICoord(UICoord::TSource::None, 0.5f, 32.0f),
                VectorFloat2(0.5f, 0.5f)
            )));

        data_main->AddChild(data_stack);
#else
        data_main->AddChild(data_button_0);
#endif

        in_out_data_map["main"] = data_main;
        return;
    }

};

class UIModel : public IUIModel
{
public:
    UIModel(IWindowApplication& )//in_application)
    {
        //BuildModelData00_OneRedQuad(_data_map);
        //BuildModelData01_ChildRedQuad(_data_map);
        //BuildModelData010_ChildRedMargin(_data_map);
        //BuildModelData02_Text(_data_map);
        BuildModelData020_TextAlignment(_data_map);
        //BuildModelData03_ShrunkText(_data_map);
        //BuildModelData04_ShrunkTextChildMargin(_data_map);
        //BuildModelData05_Effect(_data_map);
        //BuildModelData050_CornerMargin(_data_map);
        //BuildModelData06_Tooltip(_data_map);
        //BuildModelData07_Stack(_data_map);
        //BuildModelData070_StackMargin(_data_map);
        //BuildModelData08_Button(_data_map);
        //BuildModelData09_NestedButton(_data_map);
        //slider
        //tooltip layer
        //checkbox
        //scroll
        //combo box

/*
        _data_build_version = std::make_shared<UIDataString>(
            std::string(Build::GetBuildTime()) + " " + Build::GetBuildVersion(),
            LocaleISO_639_1::Default,
            "string_small_right_em"
            );

        const std::string build_info_string = std::string(
            DefaultUIComponentFactory::GetTextRunTagSmallFont() + 
            Build::GetBuildHost()) +
            " <Locale " + Build::GetBuildConfiguration() + ">" + 
            " <Locale " + Build::GetBuildPlatform() + ">";

        _data_build_info = std::make_shared<UIDataTextRun>(
            build_info_string,
            LocaleISO_639_1::Default,
            "text_run_small_right"
            );

        _data_build_fps = std::make_shared<UIDataString>(
            "0.0",
            LocaleISO_639_1::Default,
            "string_small_right_fixed"
            );

        _data_build = std::make_shared<UIData>(
            "stack_vertical_bottom_right"
            );

        auto data_ui_scale_knot = BuildSliderKnot();
        auto data_map_ui_scale = std::make_shared<UIDataSlider>(
            1.0f,
            0.05f,
            VectorFloat2(0.25f, 3.0f),
            [this](const float in_new_value){
                auto data = std::dynamic_pointer_cast<UIDataSlider>(_data_map["ui_scale"]);
                if (nullptr != data)
                {
                    data->SetValue(in_new_value);
                }
            },
            [this]()->const std::string{
                auto data = std::dynamic_pointer_cast<UIDataSlider>(_data_map["ui_scale"]);
                if (nullptr != data)
                {
                    std::stringstream stream;
                    stream << std::fixed;
                    stream << std::setprecision(1);
                    stream << data->GetValue();
                    std::string stream_string = stream.str();
                    return stream_string;
                }
                return std::string();
            },
            data_ui_scale_knot,
            "slider_horizontal",
            std::vector<std::shared_ptr<UIData>>({
                std::make_shared<UIData>(
                    "horizontal_rule"
                    ),
                data_ui_scale_knot
                })
            );
        _data_map["ui_scale"] = data_map_ui_scale;

        auto data_map_ui_scale_slider = std::make_shared<UIData>(
            "effect_drop_shadow_small",
            std::vector<std::shared_ptr<UIData>>({
                std::make_shared<UIData>(
                    "UIData",
                    std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIData>(
                            "grid_slider_horizontal",
                            std::vector<std::shared_ptr<UIData>>({
                                BuildSliderButton(data_map_ui_scale, -1.0f, "\xe2""\x8f""\xb4"),
                                data_map_ui_scale,
                                BuildSliderButton(data_map_ui_scale, 1.0f, "\xe2""\x8f""\xb5")
                                })
                            )
                        })
                    )
                })
            );

        auto debug_slider_knot = BuildSliderKnot();
        auto debug_slider_contents = std::make_shared<UIDataSlider>(
            0.5f,
            0.05f,
            VectorFloat2(0.0f, 1.0f),
            [this](const float in_new_value){
                auto data = std::dynamic_pointer_cast<UIDataSlider>(_data_map["debug_slider"]);
                if (nullptr != data)
                {
                    data->SetValue(in_new_value);
                }
            },
            [this]()->const std::string{
                auto data = std::dynamic_pointer_cast<UIDataSlider>(_data_map["debug_slider"]);
                if (nullptr != data)
                {
                    std::stringstream stream;
                    stream << std::fixed;
                    stream << std::setprecision(1);
                    stream << data->GetValue();
                    std::string stream_string = stream.str();
                    return stream_string;
                }
                return std::string();
            },
            debug_slider_knot,
            "slider_horizontal",
            std::vector<std::shared_ptr<UIData>>({
                std::make_shared<UIData>(
                    "horizontal_rule"
                    ),
                debug_slider_knot
                })
            );
        _data_map["debug_slider"] = debug_slider_contents;
        auto debug_slider = std::make_shared<UIData>(
            "effect_drop_shadow_small",
            std::vector<std::shared_ptr<UIData>>({
                std::make_shared<UIData>(
                    "UIData",
                    std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIData>(
                            "grid_slider_horizontal",
                            std::vector<std::shared_ptr<UIData>>({
                                BuildSliderButton(debug_slider_contents, -1.0f, "\xe2""\x8f""\xb4"),
                                debug_slider_contents,
                                BuildSliderButton(debug_slider_contents, 1.0f, "\xe2""\x8f""\xb5")
                                })
                            )
                        })
                    )
                })
            );

        auto debug_scroll_knot = std::make_shared<UIData>(
            "effect_gloss",
            std::vector<std::shared_ptr<UIData>>({
                std::make_shared<UIData>(
                    "effect_fill",
                    std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIData>(
                            "effect_corner",
                            std::vector<std::shared_ptr<UIData>>({
                                std::make_shared<UIData>(
                                    "canvas_grey"
                                    )
                                })
                            )
                        })
                    )
                })
            );

        auto debug_scroll = std::make_shared<UIDataScroll>(
            VectorFloat2(0.5f,1.5f),
            VectorFloat2(0.0f,3.0f),
            [this](const VectorFloat2& in_value)
            {
                auto data = std::dynamic_pointer_cast<UIDataScroll>(_data_map["debug_scroll"]);
                if (nullptr != data)
                {
                    data->SetValue(in_value);
                }
            },
            nullptr,
            debug_scroll_knot,
            "scroll_vertical", //"UIDataScroll",
            std::vector<std::shared_ptr<UIData>>({
                debug_scroll_knot
                })
            );
        _data_map["debug_scroll"] = debug_scroll;

        auto debug_scroll_wrapper = std::make_shared<UIData>(
            //"canvas_scroll_horizontal_wrapper",
            "canvas_manual_scroll_vertical",
            std::vector<std::shared_ptr<UIData>>({
                std::make_shared<UIData>(
                    "effect_drop_shadow_small",
                    std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIData>(
                            "UIData",
                            std::vector<std::shared_ptr<UIData>>({
                                debug_scroll
                                })
                            )
                        })
                    )
                })
            );

        auto locale_name_array = std::vector<std::shared_ptr<UIData>>({
            std::make_shared<UIDataTextRun>(
                "<Locale en>",
                LocaleISO_639_1::English,
                "text_run_middle"
                ),
            std::make_shared<UIDataTextRun>(
                "<Locale es>",
                LocaleISO_639_1::English,
                "text_run_middle"
                ),
            std::make_shared<UIDataTextRun>(
                "<Locale hi>",
                LocaleISO_639_1::English,
                "text_run_middle"
                ),
            std::make_shared<UIDataTextRun>(
                "<Locale ar>",
                LocaleISO_639_1::English,
                "text_run_middle"
                ),
            std::make_shared<UIDataTextRun>(
                "<Locale pt>",
                LocaleISO_639_1::English,
                "text_run_middle"
                ),
            std::make_shared<UIDataTextRun>(
                "<Locale bn>",
                LocaleISO_639_1::English,
                "text_run_middle"
                ),
            std::make_shared<UIDataTextRun>(
                "<Locale ru>",
                LocaleISO_639_1::English,
                "text_run_middle"
                ),
            std::make_shared<UIDataTextRun>(
                "<Locale ja>",
                LocaleISO_639_1::English,
                "text_run_middle"
                ),
            std::make_shared<UIDataTextRun>(
                "<Locale pa>",
                LocaleISO_639_1::English,
                "text_run_middle"
                ),
            std::make_shared<UIDataTextRun>(
                "<Locale de>",
                LocaleISO_639_1::English,
                "text_run_middle"
                ),
            std::make_shared<UIDataTextRun>(
                "<Locale jv>",
                LocaleISO_639_1::English,
                "text_run_middle"
                )
            });

        auto locale_list_box = BuildListBox(
            _data_map,
            "list_box",
            locale_name_array
            );

        auto data_combo_box_layer = BuildComboBoxLayer(
            _data_map,
            "combo_box_layer"
            );
        //std::make_shared<UIDataComboBoxLayer>();
        _data_map["combo_box_layer"] = data_combo_box_layer;

        auto data_combo_box_locale = BuildComboBox(
            _data_map,
            "combo_box_layer",
            "combo_box_locale",
            locale_name_array
            );

        auto data_tooltip_layer = BuildTooltipLayer();
        _data_map["tooltip_layer"] = data_tooltip_layer;

        // Launch Left banner
        auto data_main_launch = std::make_shared<UIData>(
            "canvas_banner_left",
            std::vector<std::shared_ptr<UIData>>({
                std::make_shared<UIData>(
                    "effect_drop_shadow",
                    std::vector<std::shared_ptr<UIData>>({

                        std::make_shared<UIData>(
                            "UIData",
                            std::vector<std::shared_ptr<UIData>>({

                        std::make_shared<UIData>(
                            "drift_from_top",
                            std::vector<std::shared_ptr<UIData>>({

                                std::make_shared<UIData>(
                                    "stack_vertical_middle",
                                    std::vector<std::shared_ptr<UIData>>({

#if 1
                                        std::make_shared<UIDataDisable>(
                                            true,
                                            "UIDataDisable",
                                                    std::vector<std::shared_ptr<UIData>>({

                                                        std::make_shared<UIDataButton>(
                                                            nullptr, 
                                                            nullptr, 
                                                            false,
                                                            "UIDataButton",
                                                            BuildButtonData("New game")
                                                        )
                                                })
                                            ),
                                        std::make_shared<UIDataDisable>(
                                            true,
                                            "UIDataDisable",
                                                    std::vector<std::shared_ptr<UIData>>({

                                                        std::make_shared<UIDataButton>(
                                                            nullptr, 
                                                            nullptr, 
                                                            false,
                                                            "UIDataButton",
                                                            BuildButtonData("Continue")
                                                        )
                                                    })
                                        ),

                                        std::make_shared<UIDataButton>(
                                            [this](const VectorFloat2&){
                                                _data_map["main"]->ModifyData().clear();
                                                _data_map["modal"]->ModifyData().push_back(
                                                    _data_map["modal_options"]
                                                    );
                                                },
                                            [](){
                                                return std::string("<Locale open_options_dialog>");
                                            },
                                            false,
                                            "UIDataButton",
                                            BuildButtonData("Options")
                                        ),

                                        std::make_shared<UIDataButton>(
                                            [&in_application](const VectorFloat2&){
                                                in_application.Destroy(0);
                                                return;
                                                },
                                            [](){
                                                return std::string("<Locale exit_application>");
                                            },
                                            false,
                                            "UIDataButton",
                                            BuildButtonData("Exit")
                                        ),
#else
                                        locale_list_box
#endif

                                        })
                                    )
                                })
                            )
                                })
                            )
                        })

                    )
                })
            );
        _data_map["main_launch"] = data_main_launch;

        auto _data_main = std::make_shared<UIData>(
            "UIData",
            std::vector<std::shared_ptr<UIData>>({
                data_main_launch
                })
            );
        _data_map["main"] = _data_main;

        auto data_toggle_fps = std::make_shared<UIDataToggle>(
            true,
            [this](const VectorFloat2&)
            {
                auto data = std::dynamic_pointer_cast<UIDataToggle>(_data_map["data_toggle_fps"]);
                if (nullptr != data)
                {
                    data->ToggleValue();
                }
            },
            [this]()
            {
                auto data = std::dynamic_pointer_cast<UIDataToggle>(_data_map["data_toggle_fps"]);
                if (nullptr != data)
                {
                    if (data->GetValue())
                    {
                        return std::string("<Locale fps_display_is_on>");
                    }
                    else
                    {
                        return std::string("<Locale fps_display_is_off>");
                    }
                }
                return std::string();
            },
            "UIDataToggle",
            BuildCheckboxTrue(),
            BuildCheckboxFalse()
            );
        _data_map["data_toggle_fps"] = data_toggle_fps;

        auto data_toggle_tooltip = std::make_shared<UIDataToggle>(
            true,
            [this](const VectorFloat2&)
            {
                auto data = std::dynamic_pointer_cast<UIDataToggle>(_data_map["data_toggle_tooltip"]);
                if (nullptr != data)
                {
                    data->ToggleValue();
                }
                auto tooltip_layer = std::dynamic_pointer_cast<UIDataTooltipLayer>(_data_map["tooltip_layer"]);
                if (nullptr != tooltip_layer)
                {
                    if (true == data->GetValue())
                    {
                        tooltip_layer->SetTooltipType(UITooltipType::TRelativeToTouch);
                    }
                    else
                    {
                        tooltip_layer->SetTooltipType(UITooltipType::TNone);
                    }
                }
            },
            [this]()
            {
                auto data = std::dynamic_pointer_cast<UIDataToggle>(_data_map["data_toggle_tooltip"]);
                if (nullptr != data)
                {
                    if (data->GetValue())
                    {
                        return std::string("<Locale tooltip_display_is_on>");
                    }
                    else
                    {
                        return std::string("<Locale tooltip_display_is_off>");
                    }
                }
                return std::string();
            },
            "UIDataToggle",
            BuildCheckboxTrue(),
            BuildCheckboxFalse()
            );
        _data_map["data_toggle_tooltip"] = data_toggle_tooltip;

        auto data_options_body = std::make_shared<UIData>(
            "stack_top_down",
            std::vector<std::shared_ptr<UIData>>({
                std::make_shared<UIData>(
                    "grid_small_big_pair",
                    std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIDataString>(
                            "Show FPS",
                            LocaleISO_639_1::Default,
                            "string_right_em"
                            ),
                        nullptr,
                        BuildCheckbox(data_toggle_fps)
                    })
                ),

                std::make_shared<UIData>(
                    "grid_small_big_pair",
                    std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIDataString>(
                            "Show Tooltips",
                            LocaleISO_639_1::Default,
                            "string_right_em"
                            ),
                        nullptr,
                        BuildCheckbox(data_toggle_tooltip)
                    })
                ),

                std::make_shared<UIData>(
                    "grid_small_big_pair",
                    std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIDataString>(
                            "UI Scale",
                            LocaleISO_639_1::Default,
                            "string_right_em"
                            ),
                        nullptr,
                        data_map_ui_scale_slider
                    })
                ),

                std::make_shared<UIData>(
                    "grid_small_big_pair",
                    std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIDataString>(
                            "debug slider",
                            LocaleISO_639_1::Default,
                            "string_right_em"
                            ),
                        nullptr,
                        debug_slider
                    })
                ),
#if 0
                std::make_shared<UIData>(
                    "grid_small_big_pair",
                    std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIDataString>(
                            "debug scroll",
                            LocaleISO_639_1::Default,
                            "string_right_em"
                            ),
                        nullptr,
                        debug_scroll_wrapper
                    })
                ),
#endif
                std::make_shared<UIData>(
                    "grid_small_big_pair_tall",
                    std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIDataString>(
                            "List Box",
                            LocaleISO_639_1::Default,
                            "string_right_em"
                            ),
                        nullptr,
                        locale_list_box
                    })
                ),

                std::make_shared<UIData>(
                    "grid_small_big_pair",
                    std::vector<std::shared_ptr<UIData>>({
                        std::make_shared<UIDataString>(
                            "Locale",
                            LocaleISO_639_1::Default,
                            "string_right_em"
                            ),
                        nullptr,
                        //std::make_shared<UIData>("canvas_blue"),
                        data_combo_box_locale
                    })
                )

            })
            );

        // Options dialog
        auto data_modal_options = 
            std::make_shared<UIDataButton>(
                [this](const VectorFloat2&){
                    _data_map["modal"]->ModifyData().clear();
                    _data_map["main"]->ModifyData().push_back(
                        _data_map["main_launch"]
                        );
                    },
                [](){
                    return std::string("<Locale close_dialog>");
                },
                false,
                "button_background",
                std::vector<std::shared_ptr<UIData>>({

                    std::make_shared<UIData>(
                        "effect_drop_shadow",
                        std::vector<std::shared_ptr<UIData>>({

                    std::make_shared<UIData>(
                        "UIData",
                        std::vector<std::shared_ptr<UIData>>({

                    std::make_shared<UIData>(
                        "effect_corner_modal",
                        std::vector<std::shared_ptr<UIData>>({

                            std::make_shared<UIDataButton>(
                                nullptr,
                                nullptr,
                                false,
                                "button_modal_body",
                                std::vector<std::shared_ptr<UIData>>({
                                    std::make_shared<UIData>(
                                        "grid_dialog_header_body_footer",
                                        std::vector<std::shared_ptr<UIData>>({
                                            std::make_shared<UIDataString>(
                                                "Options",
                                                LocaleISO_639_1::Default,
                                                "string_middle_em"
                                                )

                                            , BuildManualScroll(
                                                true,
                                                false,
                                                _data_map,
                                                "options",
                                                data_options_body
                                                )
        #if 1
                                            , std::make_shared<UIData>(
                                                "effect_drop_shadow",
                                                std::vector<std::shared_ptr<UIData>>({

                                            std::make_shared<UIData>(
                                                "UIData",
                                                std::vector<std::shared_ptr<UIData>>({

                                            std::make_shared<UIData>(
                                                "canvas_margin",
                                                std::vector<std::shared_ptr<UIData>>({


                                            std::make_shared<UIData>(
                                                "grid_three_buttons",
                                                std::vector<std::shared_ptr<UIData>>({
                                            
                                        std::make_shared<UIDataDisable>(
                                            true,
                                            "UIDataDisable",
                                                    std::vector<std::shared_ptr<UIData>>({

                                                    std::make_shared<UIDataButton>(
                                                        nullptr,
                                                        nullptr,
                                                        false,
                                                        "UIDataButton",
                                                        BuildButtonData("Undo")
                                                    ),
                                                    })),
                                                    nullptr,

                                        std::make_shared<UIDataDisable>(
                                            true,
                                            "UIDataDisable",
                                                    std::vector<std::shared_ptr<UIData>>({

                                                    std::make_shared<UIDataButton>(
                                                        nullptr,
                                                        nullptr,
                                                        false,
                                                        "UIDataButton",
                                                        BuildButtonData("Redo")
                                                    ),
                                                    })),
                                                    nullptr,

                                        std::make_shared<UIDataDisable>(
                                            true,
                                            "UIDataDisable",
                                                    std::vector<std::shared_ptr<UIData>>({

                                                    std::make_shared<UIDataButton>(
                                                        nullptr,
                                                        nullptr,
                                                        false,
                                                        "UIDataButton",
                                                        BuildButtonData("Reset")
                                                    )
                                                    }))

                                                })
                                            )

                                            }))
                                            }))}))
        #endif
                                        })
                                    )
                                })
                            )

                        })
                    )

                    }))

                        })
                    )

                })
            );

        _data_map["modal_options"] = data_modal_options;

        auto _data_modal = std::make_shared<UIData>(
            "UIData",
            std::vector<std::shared_ptr<UIData>>({})
            );
        _data_map["modal"] = _data_modal;

        _data_array_map[""] = std::vector<std::shared_ptr<UIData>>({
#if 0
            // debug quad
            std::make_shared<UIData>(
                "canvas_debug_quad0",
                std::vector<std::shared_ptr<UIData>>({})
                ),
#endif

#if 1
            // Main page data
            _data_main
#endif

#if 1
            // Modal page data
            , _data_modal
#endif

#if 1
            // Build info
            , _data_build
#endif

#if 1
            // combo box dropdown layer
            , data_combo_box_layer
#endif

#if 1
            // tooltip layer
            , data_tooltip_layer
#endif

            });
*/
/*            
        data_debug_quad->AddChild(
#if 0
            std::make_shared<UIDataEffect>(
                UILayout::FactoryContentMiddle(),
                UIBaseColour(),
                nullptr,
                UIEffectEnum::TRoundCorners,
                UICoord(UICoord::TSource::None, 0.0f, s_default_margin),
                UICoord(UICoord::TSource::None, 0.0f, s_default_margin),
                UICoord(UICoord::TSource::None, 0.0f, s_default_margin),
                UICoord(UICoord::TSource::None, 0.0f, s_default_margin)
                )
#else
            std::make_shared<UIData>(
                UILayout::FactoryContentMiddleMargin(s_default_margin),
                UIBaseColour::FactoryGreenBackground()
                )
#endif
            )->AddChild(
            std::make_shared<UIDataString>(
                UILayout::FactoryContentMiddleMargin(s_default_margin),
                UIBaseColour::FactoryBlueBackground(),
                nullptr,
                "Hello human",
                LocaleISO_639_1::Default,
                false,
                TextEnum::HorizontalLineAlignment::Middle,
                TextEnum::VerticalBlockAlignment::Middle
                )
            );
*/
    }

    virtual ~UIModel()
    {
        // Nop
    }

    void RefreshBuildInfo(const float in_nice_fps)
    {
        auto data_show = dynamic_cast<UIDataToggle*>(GetData("data_toggle_fps"));

        bool show_fps = false;
        if (nullptr != data_show)
        {
            show_fps = data_show->GetValue();
        }

        if ((nullptr != _data_build_fps) && (true == show_fps))
        {
            std::stringstream stream;
            stream << std::fixed;
            stream << std::setprecision(1);
            stream << in_nice_fps;
            std::string stream_string = stream.str();

            //_data_build_fps->SetString(stream_string);
        }
/*
        auto& data_array = _data_build->ModifyData();
        data_array.clear();

        if (true == show_fps)
        {
            data_array.push_back(_data_build_fps);
        }

        data_array.push_back(_data_build_version);
        data_array.push_back(_data_build_info);
*/
    }

private:
    virtual UIData* const GetData(
        const std::string& in_key
        ) const override
    {
        UIData* result = nullptr;
        const auto found = _data_map.find(in_key);
        if (found != _data_map.end())
        {
            result = found->second.get();
        }
        return result;
    }

private:
    //std::map<std::string, std::vector<std::shared_ptr<UIData>>> _data_array_map;
    std::map<std::string, std::shared_ptr<UIData>> _data_map;

    std::shared_ptr<UIData> _data_build;
    std::shared_ptr<UIData> _data_build_info;
    std::shared_ptr<UIData> _data_build_version;
    std::shared_ptr<UIDataString> _data_build_fps;
};

IWindowApplication* const ApplicationBasicUI::Factory(
    const HWND in_hwnd,
    const WindowApplicationParam&in_application_param
    )
{
    return new ApplicationBasicUI(
        in_hwnd,
        in_application_param
        );
}

ApplicationBasicUI::ApplicationBasicUI(
    const HWND in_hwnd,
    const WindowApplicationParam& in_application_param
    ) 
    : IWindowApplication(
        in_hwnd,
        in_application_param
        )
    , _scroll_key_state(0)
    , _scroll_z(0)
    , _active(false)
{
    LOG_MESSAGE(
        "ApplicationBasicUI  ctor %p",
        this
        );

    // DrawSystem
    {
        RenderTargetFormatData render_target_format_data(
            DXGI_FORMAT_B8G8R8A8_UNORM,
            true,
            VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f)
            );
        _draw_system = std::make_unique<DrawSystem>(
            in_hwnd,
            2, //in_back_buffer_count
            D3D_FEATURE_LEVEL_11_0,
            0, //in_options
            render_target_format_data
            );
    }

    _draw_resource = std::make_unique<DrawResource>();

    _draw_resource->_locale_system = std::make_unique<LocaleSystem>();
    DefaultLocale::Populate(*_draw_resource->_locale_system);

    auto command_list = _draw_system->CreateCustomCommandList();

    _draw_resource->_text_manager = std::make_unique<TextManager>(
        _draw_system.get(),
        command_list->GetCommandList(),
        in_application_param._root_path
        );

    // UI
    _draw_resource->_ui_manager = std::make_unique<UIManager>(
        _draw_system.get(),
        command_list->GetCommandList(),
        in_application_param._root_path
        );
    DefaultUIComponentFactory::Populate(*_draw_resource->_ui_manager);

    _draw_resource->_ui_model = std::make_unique<UIModel>(*this);

    _draw_resource->_timer = std::make_unique<Timer>();
}

ApplicationBasicUI::~ApplicationBasicUI()
{
    _draw_resource.reset();
    _draw_system.reset();
    LOG_MESSAGE(
        "ApplicationBasicUI dtor %p",
        this
        );
}

void ApplicationBasicUI::Update()
{
    BaseType::Update();

#if defined(DRAW_SYSTEM_RESOURCE_ALIVE_COUNT)
    LOG_CONSOLE("resource alive count [%d]", s_draw_system_resource_alive_count);
#endif

    if (_draw_system)
    {
        float nice_fps = 0.0f;
        const float delta_seconds = _draw_resource->_timer->GetDeltaSeconds(&nice_fps);

        // update fps
        if ((nullptr != _draw_resource) && (nullptr != _draw_resource->_ui_model))
        {
            UIModel* const model = dynamic_cast<UIModel*>(_draw_resource->_ui_model.get());
            if (nullptr != model)
            {
                model->RefreshBuildInfo(nice_fps);
            }
        }

        float ui_scale = 1.0f;
        #if 1
        if ((nullptr != _draw_resource) && (nullptr != _draw_resource->_ui_model))
        {
            auto data = dynamic_cast<UIDataSlider*>(_draw_resource->_ui_model->GetData("ui_scale"));
            if (nullptr != data)
            {
                ui_scale = data->GetValue();
            }
        }
        #endif

        auto frame = _draw_system->CreateNewFrame();
        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());

        // Update ui layout
        {
            UIManagerUpdateParam update_param(
                _draw_system.get(),
                frame->GetCommandList(),
                DefaultUIComponentFactory::GetDefaultTextStyle(),
                _draw_resource->_locale_system.get(),
                _draw_resource->_text_manager.get(),
                ui_scale, //in_ui_scale = 1.0f,
                delta_seconds //in_time_delta = 0.0f
                );
            _draw_resource->_ui_manager->Update(
                _draw_resource->_ui_hierarchy_node,
                _draw_resource->_ui_model->GetData("main"),
                update_param
                );
        }

        // Deal input
        if ((nullptr != _draw_resource->_ui_hierarchy_node) && (true == _active))
        {
            int mouse_x = 0;
            int mouse_y = 0;
            bool mouse_left_button = false;
            bool mouse_right_button = false;
            const bool mouse_valid = GetMouseState(
                mouse_x,
                mouse_y,
                mouse_left_button,
                mouse_right_button
                );
            UIData* const tooltip_layer = _draw_resource->_ui_model->GetData("tooltip_layer");
            UIData* const combo_box_layer = _draw_resource->_ui_model->GetData("combo_box_layer");
             _draw_resource->_ui_manager->DealInput(
                *_draw_resource->_ui_hierarchy_node,
                tooltip_layer,
                combo_box_layer,
                UIManagerDealInputParam(
                    std::vector<UIManagerDealInputTouch>({
                        UIManagerDealInputTouch(
                            VectorInt2(mouse_x, mouse_y),
                            mouse_left_button,
                            0,
                            UITouchFlavour::TMouseLeft
                            )
                        }),
                    std::vector<UIManagerDealInputNavigation>(),
                    _draw_system.get(),
                    delta_seconds
                    )
                );
            _scroll_key_state = 0;
            _scroll_z = 0;
        }

        // Draw
        #if 1
        if (nullptr != _draw_resource->_ui_hierarchy_node)
        {
             _draw_resource->_ui_manager->Draw(
                *_draw_resource->_ui_hierarchy_node,
                UIManagerDrawParam(
                    _draw_system.get(),
                    frame.get(),
                    _draw_resource->_text_manager.get(),
                    _draw_resource->_ui_manager.get(),
                    ui_scale
                    )
                );
        }
        #endif

    }
}

void ApplicationBasicUI::OnWindowSizeChanged(
    const int in_width,
    const int in_height
    )
{
    BaseType::OnWindowSizeChanged(
        in_width,
        in_height
        );
    if (_draw_system)
    {
        _draw_system->OnResize();
    }

    return;
}

void ApplicationBasicUI::OnScroll(
    const int in_key_state,
    const int in_z_delta
    )
{
    _scroll_key_state |= in_key_state;
    _scroll_z += in_z_delta;
    return;
}

void ApplicationBasicUI::OnActivated()
{
    _active = true;
    return;
}

void ApplicationBasicUI::OnDeactivated()
{
    _active = false;
    return;
}

