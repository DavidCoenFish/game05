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
#include "common/ui/ui_content/ui_content_canvas.h"
#include "common/ui/ui_content/ui_content_texture.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"
#include "common/ui/i_ui_model.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_data/i_ui_data.h"
#include "common/ui/ui_data/ui_data_button.h"
#include "common/ui/ui_data/ui_data_container.h"
#include "common/ui/ui_data/ui_data_string.h"
#include "common/ui/ui_data/ui_data_template.h"
#include "common/ui/ui_data/ui_data_text_run.h"
#include "common/util/timer.h"
#include "common/util/vector_helper.h"
#include "common/window/window_application_param.h"

class UIModel : public IUIModel
{
public:
    UIModel()
    {
        auto _data_map_build_version = std::make_shared<UIDataString>(
            std::string(Build::GetBuildTime()) + " " + Build::GetBuildVersion(),
            LocaleISO_639_1::Default,
            "UIDataStringRight"
            );

        const std::string build_info_string = std::string(
            Build::GetBuildHost()) +
            " <Locale " + Build::GetBuildConfiguration() + ">" + 
            " <Locale " + Build::GetBuildPlatform() + ">";

        auto _data_map_build_info = std::make_shared<UIDataTextRun>(
            build_info_string,
            LocaleISO_639_1::Default,
            "UIDataTextRunRight"
            );

        auto _data_map_build_fps = std::make_shared<UIDataString>(
            "0.0",
            LocaleISO_639_1::Default,
            "UIDataStringRight"
            );
        _data_map["fps"] = _data_map_build_fps;

        _data_array_map[""] = std::vector<std::shared_ptr<IUIData>>({

#if 1
            // Build info
            std::make_shared<UIDataContainer>(
                std::vector<std::shared_ptr<IUIData>>({}),
                "canvas_debug_quad0"
                ),
#endif


#if 0
            // Build info
            std::make_shared<UIDataContainer>(std::vector<std::shared_ptr<IUIData>>({
                _data_map_build_info,
                _data_map_build_version,
                _data_map_build_fps
                }),
                "stack_vertical_bottom_right"
                ),
#endif

#if 0
            // Left banner
            std::make_shared<UIDataContainer>(
                std::vector<std::shared_ptr<IUIData>>({
                std::make_shared<UIDataContainer>(
                    std::vector<std::shared_ptr<IUIData>>({
                        std::make_shared<UIDataButton>(),
                        std::make_shared<UIDataButton>(),
                        std::make_shared<UIDataButton>(),
                        std::make_shared<UIDataButton>()
                    }),
                    "stack_vertical_middle"
                    )
                }),
                "canvas_banner_left"
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
        const std::function<void(const std::vector<std::shared_ptr<IUIData>>&)>& in_visitor
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

    virtual IUIData* const GetData(
        const std::string& in_key
        ) const override
    {
        IUIData* result = nullptr;
        const auto found = _data_map.find(in_key);
        if (found != _data_map.end())
        {
            result = found->second.get();
        }
        return result;
    }

private:
    std::map<std::string, std::vector<std::shared_ptr<IUIData>>> _data_array_map;
    std::map<std::string, std::shared_ptr<IUIData>> _data_map;

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

    _draw_resource->_ui_model = std::make_unique<UIModel>();

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
        if (nullptr != _draw_resource->_ui_hierarchy_node)
        {
             _draw_resource->_ui_manager->DealInput(
                *_draw_resource->_ui_hierarchy_node,
                UIManagerDealInputParam()
                );
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
                    _draw_resource->_text_manager.get()
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

