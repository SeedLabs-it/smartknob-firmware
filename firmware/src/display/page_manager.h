#pragma once
#include <map>
#include "logging.h"
#include "lvgl.h"
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
    }

    // virtual void createOverlay() = 0;

    void add(T page_enum, BasePage *page)
    {
        pages_[page_enum] = page;
    }

    virtual void show(T page_enum)
    {
        for (auto &it : pages_)
        {
            if (it.first == page_enum)
            {
                old_page_ = current_page_;
                current_page_ = page_enum;
                it.second->show();
            }
            else
            {
                it.second->hide();
            }
        }
    }

    void hide(T page_enum)
    {
        for (auto &it : pages_)
        {
            if (it.first == page_enum)
            {
                it.second->hide();
            }
        }
    }

protected:
    SemaphoreHandle_t mutex_;

    T current_page_;
    T old_page_;

    lv_obj_t *parent_;
    lv_obj_t *overlay_;
    std::map<T, BasePage *> pages_;
};
