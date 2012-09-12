#include "mabipackage.h"

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

struct _s_pack_header
{
	char signature[8];	// 'P' 'A' 'C' 'K' 0x02 0x01 0x00 0x00
	unsigned long d1;	// 1
	unsigned long sum;	// 文件总数
	FILETIME ft1;
	FILETIME ft2;
	char path[480];		// 'd' 'a' 't' 'a' '\' 0x00 ...

};

struct _s_pack_list_header 
{
	unsigned long sum;				// 文件总数
	unsigned long list_header_size;	// 文件头列表大小(包括了空白区域)
	unsigned long blank_size;		// 文件列表和数据区之间的空白区域大小 
	unsigned long data_section_size;		// 数据区大小
	char zero[16];

};

struct _s_pack_item_name
{
	char len_or_type;
	union
	{
		// len_or_type == 0x05的时候
		struct  
		{
			unsigned long len;
			char  sz_ansi_name2[1];
		};

		// 普通情况下的文件名
		char sz_ansi_name[1];
	};
};

struct _s_pack_item_info
{
	unsigned long seed;
	unsigned long zero;
	unsigned long offset;
	unsigned long compress_size;
	unsigned long decompress_size;
	unsigned long is_compressed;
	FILETIME ft[5];
};

#pragma pack()

PPACKINPUT pack_input(char *file_name) 
{
	// 构建返回值
	PPACKINPUT input = (PPACKINPUT) malloc(sizeof(s_pack_input_stream));
	memset(input, 0, sizeof(s_pack_input_stream));

	// 打开文件
	input->_file = fopen(file_name, "rb");

	size_t tmp;
	_s_pack_header header;
	tmp = fread(&header, sizeof(header), 1, input->_file);
	if (tmp != sizeof(header))
	{
		fprintf(stderr, "%s(%d)-%s:%s", __FILE__, __LINE__ , __FUNCTION__, "read header error.");
		pack_input_close(input);
		return 0;
	}

	// 检查文件头
	if (memcmp(header.signature, "PACK", 4) != 0)
	{
		fprintf(stderr, "%s(%d)-%s:%s", __FILE__, __LINE__ , __FUNCTION__, "header signature error.");
		pack_input_close(input);
		return 0;
	}

	_s_pack_list_header list_header;
	tmp = fread(&list_header, sizeof(list_header), 1, input->_file);
	if (tmp != sizeof(list_header))
	{
		fprintf(stderr, "%s(%d)-%s:%s", __FILE__, __LINE__ , __FUNCTION__, "read list header error.");
		pack_input_close(input);
		return 0;
	}

	// 加载到内存
	void *p_list_buffer = malloc(list_header.list_header_size);
	tmp = fread(p_list_buffer, list_header.list_header_size, 1, input->_file);
	if (tmp != sizeof(list_header.list_header_size))
	{
		fprintf(stderr, "%s(%d)-%s:%s", __FILE__, __LINE__ , __FUNCTION__, "read list content error.");
		pack_input_close(input);
		return 0;
	}

	char *p_tmp = (char *) p_list_buffer;
	for (size_t i = 0; i < list_header.sum; i++)
	{
		_s_pack_item_name * p_item_name = (_s_pack_item_name *) p_tmp;
		size_t size;
		if (p_item_name->len_or_type < 4)
		{
			// 第一字节小于4
			size = (0x10 * (p_item_name->len_or_type + 1));
		}
		else if (p_item_name->len_or_type == 4)
		{
			// 可恶的恶魔猫，这里怎么要搞特殊的
			size = 0x60 ;
		}
		else
		{
			// 基本参考mabiunpack代码
			size = p_item_name->len + 5;
		}

		string name;
		if ( p_item_name->len_or_type <= 0x04 )
		{
			name = p_item_name->sz_ansi_name;
		}
		else // 0x05
		{
			name = p_item_name->sz_ansi_name2;
		}

		// 指针跨越名称定义区
		p_tmp += size;

		_s_pack_item_info * p_info = (_s_pack_item_info *) p_tmp;

		/*
		// 累计文件偏移，其实这里修改了内存
		pInfo->offset += sizeof(PACKAGE_HEADER) + sizeof(PACKAGE_LIST_HEADER) + listHeader.list_header_size;

		// 指针定位到下一项
		pTemp += sizeof(ITEM_INFO);

		shared_ptr<IResource> spResource(new CPackResource(name, spFile, pInfo));
		pResourceSet->m_Resources.push_back( spResource );
		*/

	}

}
PPACKOUTPUT pack_output(char *file_name) 
{

}

void pack_input_close(PPACKINPUT input)
{
	if (input)
	{
		// 根据需要关闭文件
		if (input->_file)
		{
			fclose(input->_file);
			input->_file = 0;
		}

		free(input);
	}
}
void pack_output_close(PPACKOUTPUT output)
{
}

void pack_inpu_reset(PPACKINPUT input)
{
}
PPACKENTRY pack_input_get_next_entry(PPACKINPUT input)
{
}
size_t pack_input_read(PPACKINPUT input, byte* buffer, size_t size);
{
}

void pack_output_put_next_entry(PPACKOUTPUT output, PPACKENTRY entry)
{
}
void pack_output_write(PPACKOUTPUT output, byte* buffer, size_t size)
{
}
void pack_output_close_entry(PPACKOUTPUT output)
{
}



#ifdef __cplusplus
}
#endif