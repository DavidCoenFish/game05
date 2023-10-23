#include "application_pch.h"

#include "window_application/application_basic_ui.h"
#include "build.h"
#include "default_locale.h"
#include "default_ui_component_factory.h"

#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/file_system/file_system.h"
#include "common/locale/locale_system.h"
#include "common/log/log.h"
#include "common/macro.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_int4.h"
#include "common/text/text_manager.h"
#include "common/ui/ui_component/ui_component_canvas.h"
#include "common/ui/ui_component/ui_component_texture.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"
#include "common/ui/i_ui_model.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_data/ui_data.h"
#include "common/ui/ui_data/ui_data_button.h"
#include "common/ui/ui_data/ui_data_string.h"
#include "common/ui/ui_data/ui_data_text_run.h"
#include "common/util/timer.h"
#include "common/util/vector_helper.h"
#include "common/window/window_application_param.h"

namespace
{
    std::vector<std::shared_ptr<UIData>> BuildButtonData(const std::string& in_body)
    {
        return std::vector<std::shared_ptr<UIData>>({

            std::make_shared<UIData>(
                "effect_gloss",
                std::vector<std::shared_ptr<UIData>>({

                    std::make_shared<UIData>(
                        "effect_corner",
                        std::vector<std::shared_ptr<UIData>>({
                            std::make_shared<UIData>(
                                "canvas_red"
                            )
                        })
                    )
                })
            ),
            std::make_shared<UIDataString>(
                in_body,
                LocaleISO_639_1::Default,
                "string_middle_em"
            )

        });
    }

};

class UIModel : public IUIModel
{
public:
    UIModel(IWindowApplication& in_application)
    {
        auto _data_map_build_version = std::make_shared<UIDataString>(
            std::string(Build::GetBuildTime()) + " " + Build::GetBuildVersion(),
            LocaleISO_639_1::Default,
            "string_small_right"
            );

        const std::string build_info_string = std::string(
            DefaultUIComponentFactory::GetTextRunTagSmallFont() + 
            Build::GetBuildHost()) +
            " <Locale " + Build::GetBuildConfiguration() + ">" + 
            " <Locale " + Build::GetBuildPlatform() + ">";

        auto _data_map_build_info = std::make_shared<UIDataTextRun>(
            build_info_string,
            LocaleISO_639_1::Default,
            "text_run_small_right"
            );

        auto _data_map_build_fps = std::make_shared<UIDataString>(
            "0.0",
            LocaleISO_639_1::Default,
            "string_small_right_fixed"
            );
        _data_map["fps"] = _data_map_build_fps;

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
                                    "stack_vertical_middle",
                                    std::vector<std::shared_ptr<UIData>>({

                                        std::make_shared<UIDataButton>(
                                            [&in_application](const VectorFloat2&){
                                                in_application.Destroy(0);
                                                return;
                                                },
                                            true,
                                            "UIDataButton",
                                            BuildButtonData("exit")
                                        ),

                                        std::make_shared<UIDataButton>(
                                            [this](const VectorFloat2&){
                                                _data_map["main"]->ModifyData().clear();
                                                _data_map["modal"]->ModifyData().push_back(
                                                    _data_map["modal_options"]
                                                    );
                                                },
                                            true,
                                            "UIDataButton",
                                            BuildButtonData("options")
                                        ),
                                        std::make_shared<UIDataButton>(
                                            nullptr, 
                                            false,
                                            "UIDataButton",
                                            BuildButtonData("continue")
                                        ),
                                        std::make_shared<UIDataButton>(
                                            nullptr, 
                                            false,
                                            "UIDataButton",
                                            BuildButtonData("new game")
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

        // Options dialog
        auto data_modal_options = 
            std::make_shared<UIDataButton>(
                [this](const VectorFloat2&){
                    _data_map["modal"]->ModifyData().clear();
                    _data_map["main"]->ModifyData().push_back(
                        _data_map["main_launch"]
                        );
                    },
                true,
                "button_background",
                std::vector<std::shared_ptr<UIData>>({

                    std::make_shared<UIData>(
                        "effect_drop_shadow",
                        std::vector<std::shared_ptr<UIData>>({

                    std::make_shared<UIData>(
                        "UIData",
                        std::vector<std::shared_ptr<UIData>>({

                            std::make_shared<UIDataButton>(
                                [this](const VectorFloat2&){},
                                true,
                                "button_modal_body",
                                std::vector<std::shared_ptr<UIData>>({
                                    std::make_shared<UIData>(
                                        "grid_dialog_header_body_footer",
                                        std::vector<std::shared_ptr<UIData>>({
                                            std::make_shared<UIDataString>(
                                                "Options",
                                                LocaleISO_639_1::Default,
                                                "string_middle_em"
                                                ),
                                            nullptr//std::make_shared<UIData>("canvas_red")

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
                                            
                                                    std::make_shared<UIDataButton>(
                                                        [&in_application](const VectorFloat2&){},
                                                        true,
                                                        "UIDataButton",
                                                        BuildButtonData("Undo")
                                                    ),
                                                    nullptr,
                                                    std::make_shared<UIDataButton>(
                                                        [&in_application](const VectorFloat2&){},
                                                        true,
                                                        "UIDataButton",
                                                        BuildButtonData("Redo")
                                                    ),
                                                    nullptr,
                                                    std::make_shared<UIDataButton>(
                                                        [&in_application](const VectorFloat2&){},
                                                        true,
                                                        "UIDataButton",
                                                        BuildButtonData("Reset")
                                                    )

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

#if 0
            // debug string
            _data_map_build_version,
#endif


#if 1
            // Main page data
            _data_main,
#endif

#if 1
            // Modal page data
            _data_modal,
#endif

#if 1
            // Build info
            std::make_shared<UIData>(
                "stack_vertical_bottom_right",
                std::vector<std::shared_ptr<UIData>>({
                    _data_map_build_info
                    ,_data_map_build_version
                    ,_data_map_build_fps
                    })
                )
#endif

            });
    }

    virtual ~UIModel()
    {
        // Nop
    }

private:
    virtual const bool VisitDataArray(
        const std::string& in_key,
        const std::function<void(const std::vector<std::shared_ptr<UIData>>&)>& in_visitor
        ) const override
    {
        const auto found = _data_array_map.find(in_key);
        if (found != _data_array_map.end())
        {
            in_visitor(found->second);
            return true;
        }
        return false;
    }

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
    std::map<std::string, std::vector<std::shared_ptr<UIData>>> _data_array_map;
    std::map<std::string, std::shared_ptr<UIData>> _data_map;

    // Could be in _data_map? or just have as first class
    //std::shared_ptr<UIData> _data_layer_main;
    //std::shared_ptr<UIData> _data_layer_modal;
    //std::shared_ptr<UIData> _data_layer_popup;
    //std::shared_ptr<UIData> _data_layer_tooltip;

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
    if (_draw_system)
    {
        _draw_system->WaitForGpu();
    }
    _draw_resource.reset();
    _draw_system.reset();
    LOG_MESSAGE(
        "ApplicationBasicUI  dtor %p",
        this
        );
}

void ApplicationBasicUI::Update()
{
    BaseType::Update();
    if (_draw_system)
    {
        float nice_fps = 0.0f;
        const float delta_seconds = _draw_resource->_timer->GetDeltaSeconds(&nice_fps);

        #if 1
        // update fps
        {
            UIDataString* const data_string = _draw_resource->_ui_model->GetDataType<UIDataString>("fps");
            if (nullptr != data_string)
            {
                std::stringstream stream;
                stream << std::fixed;
                stream << std::setprecision(1);
                //stream << "fps: ";
                stream << nice_fps;
                std::string stream_string = stream.str();

                data_string->SetString(stream_string);
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
                _draw_resource->_ui_model.get(),
                DefaultUIComponentFactory::GetDefaultTextStyle(),
                _draw_resource->_locale_system.get(),
                _draw_resource->_text_manager.get(),
                1.0f, //in_ui_scale = 1.0f,
                delta_seconds, //in_time_delta = 0.0f,
                true, //false, //in_always_dirty = false,
                false, //in_draw_to_texture = false, // Draw to texture or backbuffer?
                VectorInt2(0,0) //in_texture_size = VectorInt2(0,0) // If in_draw_to_texture is true, size to use for texture
                );
            _draw_resource->_ui_manager->Update(
                _draw_resource->_ui_hierarchy_node,
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
             _draw_resource->_ui_manager->DealInput(
                *_draw_resource->_ui_hierarchy_node,
                UIManagerDealInputParam(
                    _draw_system.get(),
                    mouse_valid,
                    mouse_left_button,
                    mouse_right_button,
                    mouse_x,
                    mouse_y,
                    _scroll_z
                    )
                );
            _scroll_key_state = 0;
            _scroll_z = 0;
        }
        #if 1
        // Draw
        if (nullptr != _draw_resource->_ui_hierarchy_node)
        {
             _draw_resource->_ui_manager->Draw(
                *_draw_resource->_ui_hierarchy_node,
                UIManagerDrawParam(
                    _draw_system.get(),
                    frame.get(),
                    _draw_resource->_text_manager.get(),
                    _draw_resource->_ui_manager.get()
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

