#include "mabipackage.h"

#include "mt.h"
#include "zlib.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

typedef struct __s_pack_header
{
	char signature[8];	// 'P' 'A' 'C' 'K' 0x02 0x01 0x00 0x00
	unsigned long d1;	// 1
	unsigned long sum;	// 文件总数
	FILETIME ft1;
	FILETIME ft2;
	char path[480];		// 'd' 'a' 't' 'a' '\' 0x00 ...

} _s_pack_header;

typedef struct __s_pack_list_header 
{
	unsigned long sum;				// 文件总数
	unsigned long list_header_size;	// 文件头列表大小(包括了空白区域)
	unsigned long blank_size;		// 文件列表和数据区之间的空白区域大小 
	unsigned long data_section_size;		// 数据区大小
	char zero[16];

} _s_pack_list_header;

typedef struct __s_pack_item_name
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
} _s_pack_item_name;

typedef struct __s_pack_item_info
{
	unsigned long seed;
	unsigned long zero;
	unsigned long offset;
	unsigned long compress_size;
	unsigned long decompress_size;
	unsigned long is_compressed;
	FILETIME ft[5];
} _s_pack_item_info;

#pragma pack()
///////////////////////////////////////////////////////////
void _encrypt(char * pBuffer, size_t size, size_t seed )
{
	// 加密
	s_mersenne_twister_status mt;
	size_t i;
	unsigned long rseed = (seed << 7) ^ 0xA9C36DE1;
	init_genrand(&mt, rseed);
	for (i = 0; i < size;i++)
	{
		pBuffer[i] = (char)(pBuffer[i]  ^ genrand_int32(&mt));
	}
}

void _decrypt(char * pBuffer, size_t size, size_t seed )
{
	s_mersenne_twister_status mt;
	size_t i;
	unsigned long rseed = (seed << 7) ^ 0xA9C36DE1;
	init_genrand(&mt, rseed);
	for (i = 0; i < size;i++)
	{
		pBuffer[i] = (char)(pBuffer[i]  ^ genrand_int32(&mt));
	}
}
///////////////////////////////////////////////////////////
PPACKINPUT pack_input(LPCTSTR file_name) 
{
	// c语言规范，先定义内部变量
	size_t tmp;
	_s_pack_header header;
	_s_pack_list_header list_header;
	void *p_list_buffer;
	char *p_tmp;
	size_t i;
	PPACKENTRY p_entry;
	_s_pack_item_name * p_item_name;
	size_t size;
	_s_pack_item_info * p_info;

	// 构建返回值
	PPACKINPUT input = (PPACKINPUT) malloc(sizeof(s_pack_input_stream));
	memset(input, 0, sizeof(s_pack_input_stream));
	input->_pos = -1;

	// 打开文件
#ifdef _UNICODE
	input->_file = _wfopen(file_name, L"rb");
#else
	input->_file = fopen(file_name, "rb");
#endif
	tmp = fread(&header, sizeof(header), 1, input->_file);
	if (tmp != 1)
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


	tmp = fread(&list_header, sizeof(list_header), 1, input->_file);
	if (tmp != 1)
	{
		fprintf(stderr, "%s(%d)-%s:%s", __FILE__, __LINE__ , __FUNCTION__, "read list header error.");
		pack_input_close(input);
		return 0;
	}

	// 加载到内存
	p_list_buffer = malloc(list_header.list_header_size);
	tmp = fread(p_list_buffer, list_header.list_header_size, 1, input->_file);
	if (tmp != 1)
	{
		fprintf(stderr, "%s(%d)-%s:%s", __FILE__, __LINE__ , __FUNCTION__, "read list content error.");
		pack_input_close(input);
		return 0;
	}

	p_tmp = (char *) p_list_buffer;
	input->_entry_count = list_header.sum;
	input->_entries = (s_pack_entry *) malloc(sizeof(s_pack_entry) * list_header.sum);
	for (i = 0; i < list_header.sum; i++)
	{
		// 准备内容
		p_entry = &input->_entries[i];

		p_item_name = (_s_pack_item_name *) p_tmp;
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

		// 下面其实存在溢出可能，但是目前应该没有这么长的目录
		if ( p_item_name->len_or_type <= 0x04 )
		{
			strcpy(p_entry->name, p_item_name->sz_ansi_name);
		}
		else // 0x05
		{
			strcpy(p_entry->name, p_item_name->sz_ansi_name2);
		}

		// 指针跨越名称定义区
		p_tmp += size;

		p_info = (_s_pack_item_info *) p_tmp;
		// 偏移是从文件头开始的
		p_entry->offset = p_info->offset + sizeof(_s_pack_header) + sizeof(_s_pack_list_header) + list_header.list_header_size;
		p_entry->seed = p_info->seed;
		p_entry->compress_size = p_info->compress_size;
		p_entry->decompress_size = p_info->decompress_size;
		p_entry->is_compressed = p_info->is_compressed;
		memcpy(p_entry->ft, p_info->ft, sizeof(FILETIME) * 5);

		// 指针定位到下一项
		p_tmp += sizeof(_s_pack_item_info);
	}
	free(p_list_buffer);

	return input;
}

// 每次扩容一倍
void _grow_entry_array(PPACKOUTPUT output)
{
	output->_entry_malloc_size *= 2;
	output->_entries = (PPACKENTRY) realloc(output->_entries, output->_entry_malloc_size);
}

PPACKOUTPUT pack_output(LPCTSTR file_name) 
{
	PPACKOUTPUT output = (PPACKOUTPUT) malloc(sizeof(s_pack_output_stram));
	memset(output, 0, sizeof(s_pack_output_stram));
	output->_pos = -1;

	// 初始化一个空间，防止经常申请内存
	output->_entry_malloc_size = sizeof(s_pack_entry) * 100;
	output->_entries = (PPACKENTRY) malloc(output->_entry_malloc_size);
#ifdef _UNICODE
	output->_file = _wfopen(file_name, L"wb");
#else
	output->_file = fopen(file_name, "wb");
#endif
	return 0;
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

		// 释放entry数组
		if (input->_entries)
		{
			free(input->_entries);
			input->_entries = 0;
		}

		if (input->_buffer)
		{
			free(input->_buffer);
			input->_buffer = 0;
		}

		free(input);
	}
}
void pack_output_close(PPACKOUTPUT output)
{
}

void pack_inpu_reset(PPACKINPUT input)
{
	input->_ptr = input->_buffer;
}
size_t pack_input_get_entry_count(PPACKINPUT input)
{
	return input->_entry_count;
}
PPACKENTRY pack_input_get_entry(PPACKINPUT input, size_t index)
{
	return &input->_entries[index];
}
PPACKENTRY pack_input_read_for_entry(PPACKINPUT input, size_t index)
{
	s_pack_entry * p_entry;
	char *p_buffer;
	unsigned long dest_len;

	input->_pos = index;

	p_entry = &input->_entries[input->_pos];
	// 将当前内容进行解密 解压
	// 先读内容到内存
	p_buffer = (char *) malloc(p_entry->compress_size);
	// 从文件读取
	fseek(input->_file, p_entry->offset, SEEK_SET);
	_decrypt(p_buffer, p_entry->compress_size, p_entry->seed);
	
	// 准备好缓冲区
	if (input->_buffer)
	{
		free(input->_buffer);
		input->_buffer = 0;
	}
	input->_buffer = (byte *)malloc(p_entry->decompress_size);
	dest_len = p_entry->decompress_size;
	uncompress(input->_buffer, &dest_len, (byte *)p_buffer, p_entry->compress_size);
	input->_ptr = input->_buffer;

	free(p_buffer);
	return p_entry;
}
size_t pack_input_read(PPACKINPUT input, byte* buffer, size_t size)
{
	s_pack_entry * p_entry;
	size_t remain_size;
	size_t result;
	if (input->_pos < 0 || input->_pos >= input->_entry_count)
	{
		return EOF;
	}
	p_entry = &input->_entries[input->_pos];
	remain_size = p_entry->decompress_size + input->_buffer - input->_ptr;
	result = remain_size >= size ? size : remain_size;
	memcpy(buffer, input->_ptr, result);
	input->_ptr += result;
	return result;
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