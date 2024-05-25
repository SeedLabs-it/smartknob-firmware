#pragma once
#include <map>
#include "logging.h"
#include "lvgl.h"
#include "semaphore_guard.h"
class BasePage
{
public:
    BasePage(lv_obj_t *parent) : parent(parent)
    {

        page = lv_obj_create(parent);
        lv_obj_remove_style_all(page);
        lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES);
        lv_obj_add_flag(page, LV_OBJ_FLAG_HIDDEN);
    }

    void show()
    {
        lv_obj_remove_flag(page, LV_OBJ_FLAG_HIDDEN);
    }

    void hide()
    {
        lv_obj_add_flag(page, LV_OBJ_FLAG_HIDDEN);
    }

protected:
    lv_obj_t *parent;
    lv_obj_t *page;
};

template <typename T>
class PageManager
{
public:
    PageManager(lv_obj_t *parent, SemaphoreHandle_t mutex) : parent_(parent), mutex_(mutex)
    {
        static_assert(std::is_enum<T>::value, "T must be an enum type");
        {
            SemaphoreGuard lock(mutex_);
            lv_obj_set_style_bg_color(parent, LV_COLOR_MAKE(0x00, 0x00, 0x00), 0);
        }
    }

    // virtual void createOverlay() = 0;

    void add(T page_enum, BasePage *page)
    {
        pages_[page_enum] = page;
    }

    virtual void show(T page_enum)
    {
        SemaphoreGuard lock(mutex_);

        pages_[current_page_]->hide();
        pages_[page_enum]->show();

        old_page_ = current_page_;
        current_page_ = page_enum;
    }

    void hide(T page_enum)
    {
        pages_[page_enum]->hide();
    }

protected:
    SemaphoreHandle_t mutex_;

    T current_page_ = static_cast<T>(0);
    T old_page_ = static_cast<T>(0);

    lv_obj_t *parent_;
    lv_obj_t *overlay_;
    std::map<T, BasePage *> pages_;
};
