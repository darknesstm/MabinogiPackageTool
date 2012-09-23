#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _s_pack_entry {
	char name[MAX_PATH];
	unsigned long seed;				// 加密种子，只有在input里的entry才有此值，output中不需要设置
	unsigned long offset;			// 数据区偏移，只有在input里的entry才有此值，output中不需要设置
	unsigned long compress_size;	// 压缩后大小，只有在input里的entry才有此值，output中不需要设置
	unsigned long decompress_size;	// 解压后大小，只有在input里的entry才有此值，output中不需要设置
	unsigned long is_compressed;	// 是否进行了压缩，只有在input里的entry才有此值，output中不需要设置
	FILETIME ft[5];
} s_pack_entry, *PPACKENTRY;

typedef struct _s_pack_input_stream {
	size_t _pos;	// 当前处理的entry index
	byte *_ptr;		// 当前读取的字节指针
	byte *_buffer;	// 当前entry解压出来的数据
	FILE *_file;
	PPACKENTRY _entries;
	size_t _entry_count;
} s_pack_input_stream, *PPACKINPUT;

typedef struct _s_pack_output_stram {
	unsigned long _seed;
	
	
	size_t _pos;
	FILE *_file;
	TCHAR _file_name[MAX_PATH];
	FILE *_tmp_file;				// 压缩内容的临时文件
	TCHAR _tmp_file_name[MAX_PATH];

	byte *_buffer;
	byte *_ptr;
	size_t _buffer_malloc_count;

	PPACKENTRY _entries;
	size_t _entry_malloc_count;	// 已经申请内存的个数
} s_pack_output_stram, *PPACKOUTPUT;


PPACKINPUT pack_input(LPCTSTR file_name);
PPACKOUTPUT pack_output(LPCTSTR file_name, unsigned long version);

void pack_input_close(PPACKINPUT input);
void pack_output_close(PPACKOUTPUT output);
/**
 * 放弃输出
 */
void pack_output_drop(PPACKOUTPUT output);

void pack_inpu_reset(PPACKINPUT input);
size_t pack_input_get_entry_count(PPACKINPUT input);
PPACKENTRY pack_input_get_entry(PPACKINPUT input, size_t index);
PPACKENTRY pack_input_read_for_entry(PPACKINPUT input, size_t index);
size_t pack_input_read(PPACKINPUT input, byte* buffer, size_t size);

void pack_output_put_next_entry(PPACKOUTPUT output, PPACKENTRY entry);
void pack_output_write(PPACKOUTPUT output, byte* buffer, size_t size);
void pack_output_close_entry(PPACKOUTPUT output);

#ifdef __cplusplus
}
#endif