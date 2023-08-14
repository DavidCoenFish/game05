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
#include "common/ui/ui_content_canvas.h"
#include "common/ui/ui_content_texture.h"
#include "common/ui/ui_hierarchy_node.h"
#include "common/ui/ui_manager.h"
#include "common/ui/ui_texture.h"
#include "common/ui/ui_data/i_ui_data.h"
#include "common/ui/ui_data/i_ui_provider_data.h"
#include "common/ui/ui_data/ui_data_container.h"
#include "common/ui/ui_data/ui_data_locale.h"
#include "common/ui/ui_data/ui_data_string.h"
#include "common/ui/ui_data/ui_data_template.h"
#include "common/util/locale_system.h"
#include "common/util/vector_helper.h"
#include "common/window/window_application_param.h"

class UIProviderData : public IUIProviderData
{
public:
    UIProviderData()
    {
        _data_map["build_version"] = std::make_shared<UIDataString>(
            std::string(Build::GetBuildTime()) + " " + Build::GetBuildVersion()
            );

        // How to get this to one line?
        //_data_map["build_host"] = std::make_shared<UIDataString>(Build::GetBuildHost());
        //_data_map["build_configuration"] = std::make_shared<UIDataLocaleKey>(Build::GetBuildConfiguration());
        //_data_map["build_platform"] = std::make_shared<UIDataLocaleKey>(Build::GetBuildPlatform());
        _data_map["build_info"] = std::make_shared<UIDataLocale>(std::vector<UIDataLocale::Data>({
            UIDataLocale::Data(Build::GetBuildHost()),
            UIDataLocale::Data(Build::GetBuildConfiguration(), true),
            UIDataLocale::Data(Build::GetBuildPlatform(), true)
            }));

        _data_map["fps"] = std::make_shared<UIDataString>("0.0s");

        _data_map["root.container"] = std::make_shared<UIDataContainer>(std::vector<std::shared_ptr<IUIData>>({
            _data_map["fps"],
            _data_map["build_info"],
            _data_map["build_version"]
            }));
    }

    virtual ~UIProviderData()
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

    const TemplateData GetRootPage() const
    {
        return TemplateData("root", "root");
    }

    const std::vector<TemplateData> GetPageTemplateArray(
        const std::string& in_page,
        const std::string& in_data_provider_root
        ) const
    {
        DSC_ASSERT(in_page == "root", "missing implementation");
        return std::vector<TemplateData>({
            TemplateData("background_info")
            });
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

    _draw_resource->_ui_provider_data = std::make_unique<UIProviderData>();
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

    {
        _draw_resource->_ui_hierarchy_node = std::make_shared<UIHierarchyNode>(
            UIHierarchyNode::LayoutData::FactoryFull(),
            UIHierarchyNode::MakeContentCanvas(),
            UIHierarchyNode::MakeTextureBackBuffer(
                _draw_system.get(),
                command_list->GetCommandList(),
                false, // Allow_clear
                true // Always dirty
                )
            );

        const auto root_page = _draw_resource->_ui_provider_data->GetRootPage();
        _draw_resource->_ui_manager->BuildPage(
            _draw_resource->_ui_hierarchy_node.get(),
            root_page._template_name, 
            root_page._data_provider_root,
            _draw_system.get(),
            command_list->GetCommandList(),
            _draw_resource->_ui_provider_data.get()
            );

        //const auto root_page = _draw_resource->_ui_provider_data->GetRootPage();
        //const auto data_array = _draw_resource->_ui_provider_data->GetPageTemplateArray(root_page._template_name, root_page._data_provider_root);

        //for (const auto& item : data_array)
        //{
        //    _draw_resource->_ui_hierarchy_node->AddChild(
        //        _draw_resource->_ui_manager->MakeHierarchyNode(
        //            item._template_name,
        //            item._data_provider_root,

        //            ),
        //        _draw_system.get(),
        //        command_list->GetCommandList()
        //        );
        //}
    }
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

        _draw_resource->_ui_manager->DrawHierarchy(
            _draw_system.get(),
            frame.get(),
            _draw_resource->_ui_hierarchy_node.get(),
            UIManagerDrawData(1.0f)
            );

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

