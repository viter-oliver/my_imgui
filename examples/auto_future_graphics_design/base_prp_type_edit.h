#pragma once
#include "af_bind.h"
class base_prp_type_edit
{
     ps_bs_prp _sl_bs_prp = { nullptr };
     string _key_name;
     string _popup_cap;
     bool _be_draging { false }, _hit_sm_window { false };
     int _pgidx { 0 }, _fdidx { 0 };
public:
     base_prp_type_edit()
     {}
     ~base_prp_type_edit() {}
     void set_dragging( bool be_dragging, base_ui_component* pobj = nullptr, uint16_t page_idx = 0, uint16_t off_idx = 0 );
     void view_base_prp_list();
     void base_prp_item();
     void clear_sel()
     {
          _sl_bs_prp = nullptr;
          _be_draging = _hit_sm_window = false;
     }
};

