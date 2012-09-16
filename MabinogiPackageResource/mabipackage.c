#include "mabipackage.h"

#include "mt.h"
#include "zlib.h"
#include <io.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FILE_READ_BUFFER_COUNT 10240

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
	PPACKINPUT input = (PPACKINPUT) calloc(sizeof(s_pack_input_stream), 1);
	input->_pos = -1;

	// 打开文件
#ifdef _UNICODE
	input->_file = _wfopen(file_name, L"rb");
#else
	input->_file = fopen(file_name, "rb");
#endif
	if (input->_file == 0)
	{
		fprintf(stderr, "%s(%d)-%s:%s", __FILE__, __LINE__ , __FUNCTION__, "open file error.");
		pack_input_close(input);
		return 0;
	}

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
	output->_entry_malloc_count *= 2;
	// 这里申请的内存应该在关闭输出的时候被释放
	output->_entries = (PPACKENTRY) realloc(output->_entries,  sizeof(s_pack_entry) * output->_entry_malloc_count);
}

PPACKOUTPUT pack_output(LPCTSTR file_name, unsigned long version) 
{
	size_t i = 0;
	// 这里申请的内存应该在关闭输出的时候被释放
	PPACKOUTPUT output = (PPACKOUTPUT) calloc(sizeof(s_pack_output_stram), 1);
	output->_pos = -1;
	output->_seed = version;

	// 初始化一个空间，防止经常申请内存
	output->_entry_malloc_count = 100;
	// 这里申请的内存应该在关闭输出的时候被释放
	output->_entries = (PPACKENTRY) calloc(sizeof(s_pack_entry) , output->_entry_malloc_count);

#ifdef _UNICODE
	output->_file = _wfopen(file_name, L"wb");
#else
	output->_file = fopen(file_name, "wb");
#endif

	// TODO 文件打开错误检测

#ifdef _UNICODE
	do
	{
		swprintf(output->_tmp_file_name, MAX_PATH, L"%s.tmp%d", file_name, i);
		i++;
	} 
	while (_waccess(output->_tmp_file_name, 0) == 0);
	output->_tmp_file = _wfopen(output->_tmp_file_name, L"w+b");
#else
	do
	{
		printf(output->_tmp_file_name, "%s.tmp%d", file_name, i);
		i++;
	} 
	while (access(output->_tmp_file_name, 0) == 0);
	output->_tmp_file = fopen(output->_tmp_file_name, "w+b");
#endif
	// TODO 文件打开错误检测


	return output;
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

size_t _put_name_chars(const char * name, char * buffer)
{
	size_t result;
	int len = strlen(name) + 1; // 结束符
	char type;
	DWORD put_len;

	if (len <= 0x60 - 1 )
	{
		if (len < 0x40 - 1)
		{
			type = len / 0x10;
			result = (type + 1) * 0x10;
		}
		else
		{
			type = 4;
			result = 0x60;
		}

		buffer[0] = type;
		strcpy(buffer + 1, name);

		return result;
	}
	else
	{
		put_len = (len + 5) % 0x10;
		if (put_len == 0)
		{
			put_len = len;
		}
		else
		{
			put_len = (len - put_len + 0x10);
		}
		buffer[0] = 5;
		memcpy(buffer + 1, &put_len, 2);
		strcpy(buffer + 5, name);

		return put_len + 5;
	}
}

void pack_output_close(PPACKOUTPUT output)
{
	// 写入文件，如果预留的空间足够大，则直接使用临时文件
	_s_pack_header header = {0};
	_s_pack_list_header list_header = {0};
	_s_pack_item_info info = {0};
	s_pack_entry *p_entry;
	char *buffer;
	size_t name_chars_len;
	size_t i;

	FILETIME ft;
	SYSTEMTIME st;

	memcpy(header.signature, "PACK\002\001\0\0", 8);
	header.d1 = 1;
	header.sum = output->_pos + 1;

	GetSystemTime(&st);              // gets current time
	SystemTimeToFileTime(&st, &ft);  // converts to file time format
	header.ft1 = ft;
	header.ft2 = ft;
	strcpy(header.path, "data\\");

	// 写入文件头
	fwrite(&header, sizeof(_s_pack_header), 1, output->_file);

	// 先要跳过list header，直接写入文件列表
	fseek(output->_file, sizeof(_s_pack_header) + sizeof(_s_pack_list_header), SEEK_SET);
	for (i = 0; i < header.sum; i++)
	{
		p_entry = &output->_entries[i];
		// 这里申请的内存在下面被释放
		buffer = (char *) malloc(strlen(p_entry->name) + 0x16);
		name_chars_len = _put_name_chars(p_entry->name, buffer);
		fwrite(buffer, name_chars_len, 1, output->_file);
		
		info.compress_size = p_entry->compress_size;
		info.decompress_size = p_entry->decompress_size;
		memcpy(info.ft, p_entry->ft, sizeof(FILETIME) * 5);
		info.is_compressed = p_entry->is_compressed;
		info.offset = p_entry->offset;
		info.seed = p_entry->seed;

		fwrite(&info, sizeof(_s_pack_item_info), 1, output->_file);
		free(buffer);

		list_header.list_header_size += name_chars_len;
		list_header.data_section_size += p_entry->compress_size;
	}

	// 写入 list header
	list_header.sum = header.sum;

	fseek(output->_file, sizeof(_s_pack_header), SEEK_SET);
	fwrite(&list_header, sizeof(_s_pack_list_header), 1, output->_file);

	// 将临时文件的内容追加进去
	fseek(output->_file, sizeof(_s_pack_header) + sizeof(_s_pack_list_header) + list_header.list_header_size, SEEK_SET);
	fseek(output->_tmp_file, 0, SEEK_SET);

	// 准备缓存区
	buffer = (char *) malloc(FILE_READ_BUFFER_COUNT);
	while ((name_chars_len = fread(buffer, 1, FILE_READ_BUFFER_COUNT, output->_tmp_file)) > 0 )
	{
		fwrite(buffer, name_chars_len, 1, output->_file);
	}
	free(buffer);

	fclose(output->_file);
	fclose(output->_tmp_file);

	if (output->_buffer)
	{
		free(output->_buffer);
		output->_buffer = 0;
	}

	if (output->_entries)
	{
		free(output->_entries);
		output->_entries = 0;
	}
	// 删除临时文件
#ifdef _UNICODE
	_wremove(output->_tmp_file_name);
#else
	remove(output->_tmp_file_name);
#endif
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
	// 这里申请的内存在下面被释放
	p_buffer = (char *) malloc(p_entry->compress_size);
	// 从文件读取
	fseek(input->_file, p_entry->offset, SEEK_SET);
	fread(p_buffer, p_entry->compress_size, 1, input->_file);
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
	if (output->_buffer)
	{
		pack_output_close_entry(output);
	}
	output->_pos++;
	if (output->_pos >= output->_entry_malloc_count)
	{
		// 需要增长缓冲区
		_grow_entry_array(output);
	}

	memcpy(&output->_entries[output->_pos], entry, sizeof(s_pack_entry));
	// 文件统一seed
	output->_entries[output->_pos].seed = output->_seed;

	// 准备好数据缓冲区
	output->_buffer_malloc_count = 1024 * 50; // 默认50k 缓冲区
	output->_buffer = (byte *) malloc(output->_buffer_malloc_count);
	output->_ptr = output->_buffer;
}

void _grow_buffer(PPACKOUTPUT output) 
{
	size_t offset = output->_ptr - output->_buffer;
	output->_buffer_malloc_count *= 2;
	output->_buffer = (byte *) realloc(output->_buffer, output->_buffer_malloc_count);
	output->_ptr = output->_buffer + offset;
}

void pack_output_write(PPACKOUTPUT output, byte* buffer, size_t size)
{
	while (output->_ptr + size > output->_buffer + output->_buffer_malloc_count)
	{
		_grow_buffer(output);
	}

	memcpy(output->_ptr, buffer, size);
	output->_ptr += size;
}
void pack_output_close_entry(PPACKOUTPUT output)
{
	// 缓冲区写入临时文件
	// 先压缩后加密
	PPACKENTRY p_entry = &output->_entries[output->_pos];
	unsigned long src_len = output->_ptr - output->_buffer;
	unsigned long dst_len = compressBound(src_len);
	byte * buffer = (byte *) malloc(dst_len);
	compress(buffer, &dst_len, output->_buffer, src_len);
	_encrypt((char *)buffer, dst_len, p_entry->seed);
	fwrite(buffer, dst_len, 1, output->_tmp_file);

	// 将解压前后大小写入
	p_entry->decompress_size = src_len;
	p_entry->compress_size = dst_len;
	p_entry->is_compressed = 1;
	if (output->_pos == 0)
	{
		p_entry->offset = 0;
	}
	else
	{
		p_entry->offset = output->_entries[output->_pos - 1].offset + dst_len;
	}

	free(output->_buffer);
	output->_buffer = 0;
	free(buffer);
}



#ifdef __cplusplus
}
#endif