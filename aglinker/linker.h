/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _LINKER_H_
#define _LINKER_H_

#include <unistd.h>
#include <sys/types.h>
#include <elf.h>
#include <stdio.h>
#include <link.h>
#include <sys/user.h>

#include <aglinker.h>

#define DL_ERR(fmt, x...) \
    do { \
      snprintf(linker_get_error_buffer(), linker_get_error_buffer_size(), fmt, ##x); \
      /* If LD_DEBUG is set high enough, log every dlerror(3) message. */ \
      DEBUG("%s\n", linker_get_error_buffer()); \
    } while (false)

#define DL_WARN(fmt, x...) \
    do { \
      fprintf(stderr, "WARNING: linker: "); \
      fprintf(stderr, fmt, ##x); \
      fprintf(stderr, "\n"); \
    } while (false)


// Returns the address of the page containing address 'x'.
#define PAGE_START(x)  ((x) & PAGE_MASK)

// Returns the offset of address 'x' in its page.
#define PAGE_OFFSET(x) ((x) & ~PAGE_MASK)

// Returns the address of the next page after address 'x', unless 'x' is
// itself at the start of a page.
#define PAGE_END(x)    PAGE_START((x) + (PAGE_SIZE-1))

// Magic shared structures that GDB knows about.

struct link_map_glibc {
    /* These first few members are part of the protocol with the debugger.
       This is the same format used in SVR4.  */

    ElfW(Addr) l_addr;		/* Difference between the address in the ELF
				   file and the addresses in memory.  */
    char *l_name;		/* Absolute file name object was found in.  */
    ElfW(Dyn) *l_ld;		/* Dynamic section of the shared object.  */
    struct link_map_glibc *l_next, *l_prev; /* Chain of loaded objects.  */

    /* All following members are internal to the dynamic linker.
       They may change without notice.  */

    /* This is an element which is only ever different from a pointer to
       the very same copy of this type for ld.so when it is used in more
       than one namespace.  */
    struct link_map_glibc *l_real;
};

typedef struct link_map_glibc link_map_t;

#define FLAG_LINKED     0x00000001
#define FLAG_EXE        0x00000004 // The main executable
#define FLAG_LINKER     0x00000010 // The linker itself

#define SOINFO_NAME_LEN 128

typedef void (*linker_function_t)();
typedef void* (*dlsym_func_t)(void* handle, const char* symbol);

struct soinfo {
 public:
  char name[SOINFO_NAME_LEN];
  const Elf32_Phdr* phdr;
  size_t phnum;
  Elf32_Addr entry;
  Elf32_Addr base;
  unsigned size;

  uint32_t unused1;  // DO NOT USE, maintained for compatibility.

  Elf32_Dyn* dynamic;

  uint32_t unused2; // DO NOT USE, maintained for compatibility
  uint32_t unused3; // DO NOT USE, maintained for compatibility

  soinfo* next;
  unsigned flags;

  const char* strtab;
  Elf32_Sym* symtab;

  size_t nbucket;
  size_t nchain;
  unsigned* bucket;
  unsigned* chain;

  unsigned* plt_got;

  Elf32_Rel* plt_rel;
  size_t plt_rel_count;

  Elf32_Rel* rel;
  size_t rel_count;

  linker_function_t* preinit_array;
  size_t preinit_array_count;

  linker_function_t* init_array;
  size_t init_array_count;
  linker_function_t* fini_array;
  size_t fini_array_count;

  linker_function_t init_func;
  linker_function_t fini_func;

#if defined(ANDROID_ARM_LINKER)
  // ARM EABI section used for stack unwinding.
  unsigned* ARM_exidx;
  size_t ARM_exidx_count;
#elif defined(ANDROID_MIPS_LINKER)
  unsigned mips_symtabno;
  unsigned mips_local_gotno;
  unsigned mips_gotsym;
#endif

  size_t ref_count;
  struct link_map_glibc link_map;
  char link_map_filler[0x260];

  bool constructors_called;

  // When you read a virtual address from the ELF file, add this
  // value to get the corresponding address in the process' address space.
  Elf32_Addr load_bias;

  bool has_text_relocations;
  bool has_DT_SYMBOLIC;

  dlsym_func_t ext_dlsym;

  void CallConstructors();
  void CallDestructors();
  void CallPreInitConstructors();

 private:
  void CallArray(const char* array_name, linker_function_t* functions, size_t count, bool reverse);
  void CallFunction(const char* function_name, linker_function_t function);
};

extern soinfo libdl_info;

// These aren't defined in <sys/exec_elf.h>.
#ifndef DT_PREINIT_ARRAY
#define DT_PREINIT_ARRAY   32
#endif
#ifndef DT_PREINIT_ARRAYSZ
#define DT_PREINIT_ARRAYSZ 33
#endif

void do_android_update_LD_LIBRARY_PATH(const char* ld_library_path);
soinfo* do_dlopen(const char* name, int flags);
soinfo* do_dlextlib(const char* name, dlsym_func_t ext_dlsym);
int do_dlclose(soinfo* si);

Elf32_Sym* dlsym_linear_lookup(const char* name, soinfo** found, soinfo* start);
soinfo* find_containing_library(const void* addr);

Elf32_Sym* dladdr_find_symbol(soinfo* si, const void* addr);
Elf32_Sym* dlsym_handle_lookup(soinfo* si, const char* name);

extern "C" void notify_gdb_of_libraries();

char* linker_get_error_buffer();
size_t linker_get_error_buffer_size();

extern "C" size_t strlcpy(char *dst, const char *src, size_t siz);
extern "C" size_t strlcat(char *dst, const char *src, size_t siz);

#endif
