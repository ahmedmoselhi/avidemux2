// automatically generated by admSerialization.py, do not edit!
#include "ADM_default.h"
#include "ADM_paramList.h"
#include "ADM_coreJson.h"
#include "x265_settings.h"
bool  x265_settings_jserialize(const char *file, const x265_settings *key){
admJson json;
json.addBool("useAdvancedConfiguration",key->useAdvancedConfiguration);
json.addNode("general");
json.addCompressParam("params",key->general.params);
json.addUint32("poolThreads",key->general.poolThreads);
json.addUint32("frameThreads",key->general.frameThreads);
json.addString("preset",key->general.preset);
json.addString("tuning",key->general.tuning);
json.addString("profile",key->general.profile);
json.endNode();
json.addInt32("level",key->level);
json.addNode("vui");
json.addUint32("sar_height",key->vui.sar_height);
json.addUint32("sar_width",key->vui.sar_width);
json.endNode();
json.addUint32("MaxRefFrames",key->MaxRefFrames);
json.addUint32("MinIdr",key->MinIdr);
json.addUint32("MaxIdr",key->MaxIdr);
json.addUint32("i_scenecut_threshold",key->i_scenecut_threshold);
json.addUint32("MaxBFrame",key->MaxBFrame);
json.addUint32("i_bframe_adaptive",key->i_bframe_adaptive);
json.addUint32("i_bframe_bias",key->i_bframe_bias);
json.addUint32("i_bframe_pyramid",key->i_bframe_pyramid);
json.addBool("b_deblocking_filter",key->b_deblocking_filter);
json.addBool("b_open_gop",key->b_open_gop);
json.addUint32("interlaced_mode",key->interlaced_mode);
json.addBool("constrained_intra",key->constrained_intra);
json.addBool("b_intra",key->b_intra);
json.addUint32("lookahead",key->lookahead);
json.addUint32("weighted_pred",key->weighted_pred);
json.addBool("weighted_bipred",key->weighted_bipred);
json.addBool("rect_inter",key->rect_inter);
json.addUint32("cb_chroma_offset",key->cb_chroma_offset);
json.addUint32("cr_chroma_offset",key->cr_chroma_offset);
json.addUint32("me_method",key->me_method);
json.addUint32("me_range",key->me_range);
json.addUint32("subpel_refine",key->subpel_refine);
json.addUint32("trellis",key->trellis);
json.addDouble("psy_rd",key->psy_rd);
json.addBool("fast_pskip",key->fast_pskip);
json.addBool("dct_decimate",key->dct_decimate);
json.addUint32("noise_reduction",key->noise_reduction);
json.addUint32("noise_reduction_intra",key->noise_reduction_intra);
json.addUint32("noise_reduction_inter",key->noise_reduction_inter);
json.addBool("strong_intra_smoothing",key->strong_intra_smoothing);
json.addNode("ratecontrol");
json.addUint32("rc_method",key->ratecontrol.rc_method);
json.addUint32("qp_constant",key->ratecontrol.qp_constant);
json.addUint32("qp_step",key->ratecontrol.qp_step);
json.addUint32("bitrate",key->ratecontrol.bitrate);
json.addDouble("rate_tolerance",key->ratecontrol.rate_tolerance);
json.addUint32("vbv_max_bitrate",key->ratecontrol.vbv_max_bitrate);
json.addUint32("vbv_buffer_size",key->ratecontrol.vbv_buffer_size);
json.addUint32("vbv_buffer_init",key->ratecontrol.vbv_buffer_init);
json.addDouble("ip_factor",key->ratecontrol.ip_factor);
json.addDouble("pb_factor",key->ratecontrol.pb_factor);
json.addUint32("aq_mode",key->ratecontrol.aq_mode);
json.addDouble("aq_strength",key->ratecontrol.aq_strength);
json.addBool("cu_tree",key->ratecontrol.cu_tree);
json.addBool("strict_cbr",key->ratecontrol.strict_cbr);
json.endNode();
return json.dumpToFile(file);
};
bool  x265_settings_jdeserialize(const char *file, const ADM_paramList *tmpl,x265_settings *key){
admJsonToCouple json;
CONFcouple *c=json.readFromFile(file);
if(!c) {ADM_error("Cannot read json file");return false;}
bool r= ADM_paramLoadPartial(c,tmpl,key);
delete c;
return r;
};
