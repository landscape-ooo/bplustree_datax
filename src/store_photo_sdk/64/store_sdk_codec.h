#ifndef __STORE_SDK_CODEC_H
#define __STORE_SDK_CODEC_H

#include<stdio.h>
#include<string>
#include<map>
#include<vector>
#include "store_define.h"


using namespace std;

namespace store_photo_sdk
{
	class StoreSdkMsg;
	class SdkCodec
	{
		public:
			SdkCodec();
			~SdkCodec();

		    void set_business_id(const string &business_id);
			
			int store_is_complete(const char *buffer, unsigned len);

			//rsp_decode
			int decode_head(const string &buffer, unsigned &req_id, int &ret_code);

			//��������encode�ӿ�,������reqid Ϊ0������sdk�ڲ�����seq�Ų�����������sdk���ô����reqid��Ϊseq��

#ifdef __CLOUDPIC_INTERFACE__
			//�����ļ�by ordinal
			int encode_file_download_ordinal_req(const string &fileid,  const unsigned ordinal, 
					string &buffer, unsigned &reqid);
			int decode_file_download_ordinal_rsp(const string &buffer, string &data);
			// ��UGC�ļ�����
			// ɾ���ļ�
			int encode_raw_delete_req(const string &fileid, string &buffer, unsigned &reqid);
            int decode_raw_delete_rsp(const string &buffer, unsigned &del_file_size);

			//��ѯ�ļ���Ϣ 
			int encode_raw_query_req(const string &fileid, string &buffer, unsigned &reqid);
			int decode_raw_query_rsp(const string &buffer, string &inter_info, string &file_info, pic_info_list &info_list);
			int decode_raw_query_rsp(const string &buffer, string &inter_info, string &file_info, pic_info_list &info_list, map<string, string> &pic_profile);

			//�޸��ļ�˵����Ϣ
			int encode_raw_modify_req(const string &fileid, const string &file_info, string &buffer, unsigned &reqid);
			
            int encode_raw_upload_v2_req(const string &uid, const string &fileid, const string &file_info, 
					const string &data, const string &data_sha1, const upload_param &param, string &buffer, unsigned &reqid);
			int decode_raw_upload_v2_rsp(const string &buffer, string &fileid, unsigned &chid, unsigned &fid, 
					pic_info_list &list, map<string, string> &pic_profile);
			//�ļ�����v2
			int encode_raw_overwrite_v2_req(const string &uid, const string &fileid, const string &file_info, 
					const string &data, const string &data_sha1, const upload_param &param, string &buffer, unsigned &reqid);
			int decode_raw_overwrite_v2_rsp(const string &buffer, string &fileid, unsigned &chid, unsigned &fid, 
					pic_info_list &list, map<string, string> &pic_profile);

			//�ƶ�or�����ļ�
			int encode_raw_move_req(const string &src_fileid, const string &dst_fileid,
					const int delete_old, const unsigned savedays, string &buffer, unsigned &reqid);
			int encode_raw_move_req(const string &src_fileid, const string &dst_fileid,
					const int delete_old, const int overwrite, const unsigned savedays, string &buffer, unsigned &reqid);
#endif

        public:
			int encode_msg(StoreSdkMsg *msg, string &buffer);
			int decode_msg(const string &buffer, StoreSdkMsg &msg);
       
			void init_protobuf_head(StoreSdkMsg *msg, unsigned cmd_type, unsigned &reqid);
			unsigned generate_flowno();
			string _business_id;
			unsigned _flow_no_seq;
	};
};

#endif
