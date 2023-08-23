#include "application_pch.h"

#include "window_application/application_basic_ui.h"
#include "build.h"
#include "default_locale.h"
#include "default_ui_template.h"

#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/macro.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_int4.h"
#include "common/text/text_manager.h"
#include "common/ui/ui_content_canvas.h"
#include "common/ui/ui_content_texture.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"
#include "common/ui/i_ui_model.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_data/i_ui_data.h"
#include "common/ui/ui_data/ui_data_container.h"
#include "common/ui/ui_data/ui_data_text_run.h"
#include "common/ui/ui_data/ui_data_template.h"
#include "common/util/locale_system.h"
#include "common/util/vector_helper.h"
#include "common/window/window_application_param.h"

class UIModel : public IUIModel
{
public:
    UIModel()
    {
        _data_map["build_version"] = std::make_shared<UIDataString>(
            std::string(Build::GetBuildTime()) + " " + Build::GetBuildVersion()
            );

        _data_map["build_info"] = std::make_shared<UIDataTextRun>(std::vector<UIDataTextRun::Data>({
            UIDataTextRun::Data(Build::GetBuildHost()),
            UIDataTextRun::Data(Build::GetBuildConfiguration(), true),
            UIDataTextRun::Data(Build::GetBuildPlatform(), true)
            }));

        _data_map["build_fps"] = std::make_shared<UIDataString>("0.0s");

        _data_map["build"] = std::make_shared<UIDataContainer>(std::vector<std::shared_ptr<IUIData>>({
            _data_map["build_fps"],
            _data_map["build_info"],
            _data_map["build_version"]
            }));

        _data_map[""] = std::make_shared<UIDataContainer>(std::vector<std::shared_ptr<IUIData>>({
            _data_map["build"]
            }));

    }

    virtual ~UIModel()
    {
        // Nop
    }

    virtual const IUIData* const GetData(
        const std::string& in_key
        ) const override
    {
        const auto found = _data_map.find(in_key);
        if (found != _data_map.end())
        {
            return found->second.get();
        }
        return nullptr;
    }

private:
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

    // UI
    _draw_resource->_ui_manager = std::make_unique<UIManager>(
        _draw_system.get(),
        command_list->GetCommandList(),
        in_application_param._root_path
        );
    DefaultUITemplate::Populate(*_draw_resource->_ui_manager);

    _draw_resource->_ui_model = std::make_unique<UIModel>();
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
        auto frame = _draw_system->CreateNewFrame();

        // Update ui layout
        {
            _draw_resource->_ui_manager->UpdateLayout(
                _draw_resource->_ui_hierarchy_node,
                UIManagerUpdateLayoutParam()
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

        // Draw
        if (nullptr != _draw_resource->_ui_hierarchy_node)
        {
             _draw_resource->_ui_manager->Draw(
                *_draw_resource->_ui_hierarchy_node,
                UIManagerDrawParam(
                    _draw_system.get(),
                    frame.get()
                    )
                );
        }
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

