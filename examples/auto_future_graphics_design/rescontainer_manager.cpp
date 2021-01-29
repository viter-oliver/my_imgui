#include "rescontainer_manager.h"
#include "res_output.h"
#include "af_state_manager.h"


int removl_base_ui_component_ref( base_ui_component* pobj )
{
     int ref_count = 0;
     for( auto ibind_rec = g_bind_ref_dic.begin(); ibind_rec != g_bind_ref_dic.end(); )
     {
          if( ibind_rec->first._pobj == pobj )
          {
               ref_count++;
               auto& ref_list = *ibind_rec->second;
               for(auto& ref_pos:ref_list )
               {
                    auto ibind_unit = g_bind_dic.find( ref_pos );
                    if (ibind_unit!=g_bind_dic.end())
                    {
                         g_bind_dic.erase( ibind_unit );
                    }
               }
               ibind_rec = g_bind_ref_dic.erase( ibind_rec );
          }
          else
          {
               ++ibind_rec;
          }
     }    
     for( auto ibind = g_bind_dic.begin(); ibind != g_bind_dic.end(); )
     {
          if( ibind->first._pobj == pobj )
          {
               ref_count++;
               ibind = g_bind_dic.erase( ibind );
          }
          else
          {
               ++ibind;
          }
     }

     for( auto& icmn : g_base_prp_dic )
     {
          auto& base_prp = *icmn.second;
          auto& pmlist = base_prp._param_list;
          for( auto ipm = pmlist.begin(); ipm != pmlist.end(); )
          {
               if( ipm->_pobj == pobj )
               {
                    ref_count++;
                    ipm = pmlist.erase( ipm );
                    break;
               }
               else
                    ++ipm;
          }
     }
     for (auto& istm:g_mstate_manager)
     {
          auto& stm = *istm.second;
          auto& prp_list = stm._prop_list;
          auto& prm_value_list = stm._prop_value_list;
          int idx = 0;
          for( auto iprp = prp_list.begin(); iprp != prp_list.end(); )
          {
               if (iprp->_pobj==pobj)
               {
                    ref_count++;
                    for (auto& prm_sub:prm_value_list)
                    {
                         prm_sub.erase( prm_sub.begin() + idx );
                    }
                    iprp = prp_list.erase( iprp );
                    break;
               }
               else
               {
                    ++iprp;
                    ++idx;
               }
          }
     }
     for( auto ialias = g_aliase_dic.begin(); ialias != g_aliase_dic.end(); )
     {
          auto& ele_pos = *ialias->second;
          if( ele_pos._pobj == pobj )
          {
               ref_count++;
               ialias = g_aliase_dic.erase( ialias );
          }
          else
          {
               ++ialias;
          }
     }
     return ref_count;
}
