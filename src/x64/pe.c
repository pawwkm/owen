#include "x64.h"
#include <time.h>

PACK(typedef struct
{
    uint8_t magic[2];
    uint8_t padding[0x3A];
    uint32_t offset;
    uint8_t message[104];
} Dos_Stub);

// https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#coff-file-header-object-and-image
PACK(typedef struct
{
    uint16_t machine;
    uint16_t number_of_sections;

    uint32_t time_stamp;

    // Deprecated. Should be 0.
    uint32_t pointer_to_symbol_table;

    // Deprecated. Should be 0.
    uint32_t symbols_length;
    
    uint16_t size_of_optional_header;
    uint16_t characteristics;
} Pe_Header);

// https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#optional-header-data-directories-image-only
PACK(typedef struct
{
    uint32_t virtual_address;
    uint32_t size;
} Data_Directory);

// https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#optional-header-image-only
PACK(typedef struct
{
    // https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#optional-header-standard-fields-image-only
    struct
    {
        uint16_t magic;

        uint8_t major_linker_version;
        uint8_t minor_linker_version;

        uint32_t size_of_code;
        uint32_t size_of_initialized_data;
        uint32_t size_of_uninitialized_data;

        uint32_t address_of_entry_point;
        uint32_t base_of_code;
    } std;

    // https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#optional-header-windows-specific-fields-image-only
    struct
    {
        uint64_t image_base;
        uint32_t section_alignment;
        uint32_t file_alignment;

        uint16_t major_os_version;
        uint16_t minor_os_version;

        uint16_t major_image_version;
        uint16_t minor_image_version;

        uint16_t major_sub_system_version;
        uint16_t minor_sub_system_version;

        uint32_t win32_version_value;
        uint32_t size_of_image;
        uint32_t size_of_headers;
        uint32_t check_sum;
        uint16_t sub_system;

        uint16_t dll_characteristics;
        uint64_t size_of_stack_reserve;
        uint64_t size_of_stack_commit;
        uint64_t size_of_heap_reserve;
        uint64_t size_of_heap_commit;

        uint32_t loader_flags;
        uint32_t number_of_rva_and_sizes;
    } windows;

    // https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#optional-header-data-directories-image-only
    struct
    {
        Data_Directory export_table;
        Data_Directory import_table;
        Data_Directory resource_table;
        Data_Directory exception_table;
        Data_Directory certificate_table;
        Data_Directory base_relocation_table;
        Data_Directory debug_table;
        Data_Directory architecture_table;
        Data_Directory global_pointer_table;
        Data_Directory tls_table;
        Data_Directory load_configuration_table;
        Data_Directory bound_import_table;
        Data_Directory import_address_table;
        Data_Directory delay_import_table;
        Data_Directory clr_runtime_header;
        Data_Directory reserved;
    } directories;
} Optional_Header);

// https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#section-table-section-headers
PACK(typedef struct
{
    uint8_t name[8];
    uint32_t virtual_size;
    uint32_t virtual_address;
    uint32_t size_of_raw_data;
    uint32_t pointer_to_raw_data;
    uint32_t pointer_to_relocations;
    uint32_t pointer_to_line_numbers;
    uint16_t number_of_relocations;
    uint16_t number_of_line_numbers;
    uint32_t characteristics;
} Section_Header);

// https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#coff-file-header-object-and-image
PACK(typedef struct
{
    Dos_Stub dos;
    uint8_t signature[4];
    Pe_Header header;
    Optional_Header optional_header;
} Pe);

#define IMAGE_FILE_MACHINE_AMD64 0x8664

#define IMAGE_FILE_RELOCS_STRIPPED     0x0001
#define IMAGE_FILE_EXECUTABLE_IMAGE    0x0002
#define IMAGE_FILE_LARGE_ADDRESS_AWARE 0x0020

#define PE32_PLUS 0x20b

#define IMAGE_SUBSYSTEM_WINDOWS_GUI 2
#define IMAGE_SUBSYSTEM_WINDOWS_CUI 3

#define IMAGE_SCN_CNT_CODE    0x00000020
#define IMAGE_SCN_MEM_EXECUTE 0x20000000
#define IMAGE_SCN_MEM_READ    0x40000000

#define ALIGN(VALUE, ALIGNMENT) ((VALUE) % (ALIGNMENT) ? ((VALUE) + (ALIGNMENT) - ((VALUE) % (ALIGNMENT))) : VALUE)

static Section_Header assemble_ir(uint32_t alignment)
{
    assemble_function(lookup_function(main_function)->ir);
    return (Section_Header) 
    {
        .name = ".text",
        .size_of_raw_data = ALIGN(text_section_length, alignment),
        .virtual_size = text_section_length,
        .virtual_address = 0x1000,
        .pointer_to_raw_data = 0x200,
        .characteristics = IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ
    };
}

static void write_section(const Section_Header* header, const uint8_t* data, uint32_t data_length, FILE* file)
{
    fseek(file, header->pointer_to_raw_data, SEEK_SET);
    fwrite(data, 1, data_length, file);
    
    if ((uint32_t)ftell(file) != header->pointer_to_raw_data + header->size_of_raw_data)
    {
        fseek(file, header->pointer_to_raw_data + header->size_of_raw_data - 1, SEEK_SET);
        fputc(0, file);
    }
}

void generate_pe(void)
{
    Pe pe =
    {
        .dos =
        {
            .magic = { 'M', 'Z' },
            .offset = 0xA8,
            .message = "This program cannot be run in DOS mode.",
        },
        .signature = "PE",
        .header =
        {
            .machine = IMAGE_FILE_MACHINE_AMD64,
            .number_of_sections = 1,
            .time_stamp = (uint32_t)time(NULL),
            .characteristics = IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_LARGE_ADDRESS_AWARE | IMAGE_FILE_RELOCS_STRIPPED,
            .size_of_optional_header = sizeof(pe.optional_header)
        },
        .optional_header =
        {
            .std =
            {
                .magic = PE32_PLUS,
                .base_of_code = 0x1000,
                .address_of_entry_point = 0x1000,
            },
            .windows =
            {
                .image_base = 0x400000,
                .section_alignment = 0x1000,
                .file_alignment = 0x200,
                .major_os_version = 0x6,
                .major_sub_system_version = 0x6,
                .sub_system = IMAGE_SUBSYSTEM_WINDOWS_CUI,
                .size_of_stack_reserve = 0x1000000,
                .size_of_stack_commit = 0x1000,
                .size_of_heap_reserve = 0x1000000,
                .size_of_heap_commit = 0x1000,
                .number_of_rva_and_sizes = 0x10
            }
        }
    };

    start_timer(Timer_code_generation);

    Section_Header text_header = assemble_ir(pe.optional_header.windows.file_alignment);
    pe.optional_header.std.size_of_code = text_header.size_of_raw_data;
    pe.optional_header.windows.size_of_headers = ALIGN(sizeof(Dos_Stub) + sizeof(Pe_Header) + pe.header.number_of_sections * sizeof(Section_Header), pe.optional_header.windows.file_alignment);
    pe.optional_header.windows.size_of_image = ALIGN(sizeof(pe) + pe.optional_header.std.size_of_code + sizeof(text_header), pe.optional_header.windows.section_alignment) +
                                               ALIGN(text_section_length, pe.optional_header.windows.section_alignment);

    start_timer(Timer_file_writing);
    FILE* file = fopen(options.destination, "wb");
    fwrite(&pe, sizeof(pe), 1, file);

    // Write section_headers.
    fwrite(&text_header, sizeof(text_header), 1, file);

    // Write sections.
    write_section(&text_header, text_section, text_section_length, file);

    fclose(file);
}
