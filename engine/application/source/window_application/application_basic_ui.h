#pragma once
#include "common/math/vector_int2.h"
#include "common/window/i_window_application.h"

class DrawSystem;
class LocaleSystem;
class TextManager;
class Timer;
class UIManager;
class UIHierarchyNode;
class UIModel;

class ApplicationBasicUI : public IWindowApplication
{
public:
    ApplicationBasicUI(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );
    virtual ~ApplicationBasicUI();
    static IWindowApplication* const Factory(
        const HWND in_hwnd,
        const WindowApplicationParam& in_application_param
        );

private:
    virtual void Update() override;
    virtual void OnWindowSizeChanged(
        const int in_width,
        const int in_height
        ) override;

private:
    typedef IWindowApplication BaseType;
    std::unique_ptr<DrawSystem> _draw_system;

    struct DrawResource
    {
        std::unique_ptr<TextManager> _text_manager;
        std::unique_ptr<LocaleSystem> _locale_system;
        std::unique_ptr<UIManager> _ui_manager;
        std::unique_ptr<UIModel> _ui_model;
        std::unique_ptr<Timer> _timer;
        std::shared_ptr<UIHierarchyNode> _ui_hierarchy_node;
    };
    std::unique_ptr<DrawResource> _draw_resource;

};
