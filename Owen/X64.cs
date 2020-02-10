using System;
using System.Collections.Generic;
using static System.IO.File;

namespace Owen
{
    internal static class X64
    {
        public static void Generate(Program program, bool includePropositions, string output)
        {
            var text = Generate(program, includePropositions);

            // https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#optional-header-image-only
            var executable = new List<byte>(new byte[]
            {
                // MS-DOS Stub
                0x4D, 0x5A, 0x90, 0x00, 0x03, 0x00, 0x00, 0x00,
                0x04, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
                0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
                0x0E, 0x1F, 0xBA, 0x0E, 0x00, 0xB4, 0x09, 0xCD,
                0x21, 0xB8, 0x01, 0x4C, 0xCD, 0x21, 0x54, 0x68,
                0x69, 0x73, 0x20, 0x70, 0x72, 0x6F, 0x67, 0x72,
                0x61, 0x6D, 0x20, 0x63, 0x61, 0x6E, 0x6E, 0x6F,
                0x74, 0x20, 0x62, 0x65, 0x20, 0x72, 0x75, 0x6E,
                0x20, 0x69, 0x6E, 0x20, 0x44, 0x4F, 0x53, 0x20,
                0x6D, 0x6F, 0x64, 0x65, 0x2E, 0x0D, 0x0D, 0x0A,
                0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            });

            // Magic number PE\0\0
            executable.Add(0x50, 0x45, 0x00, 0x00);

            // Coff File Header - https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#coff-file-header-object-and-image
            executable.Add(0x64, 0x86);             // Machine = x64
            executable.Add(0x01, 0x00);             // Number of sections
            executable.Add(0x00, 0x00, 0x00, 0x00); // Creation time stamp (since 01/01/1970)
            executable.Add(0x00, 0x00, 0x00, 0x00); // Pointer to Symbol Table (deprecated)
            executable.Add(0x00, 0x00, 0x00, 0x00); // Number of symbols (deprecated)
            executable.Add(0xF0, 0x00);             // Size of Optional Header
            executable.Add(0x23, 0x00);             // Characteristics (IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_LARGE_ADDRESS_AWARE | IMAGE_FILE_RELOCS_STRIPPED)

            // Optional Header - https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#optional-header-image-only
            executable.Add(0x0B, 0x02);                        // PE32+
            executable.Add(0x01);                              // Major linker version
            executable.Add(0x00);                              // Minor linker version

            // Pad size of code to file allignment.
            var sizeOfCode = (uint)0;
            while (sizeOfCode < text.Count)
                sizeOfCode += 0x200;
            
            executable.Add(BitConverter.GetBytes(sizeOfCode)); // Size of the code segments
            executable.Add(0x00, 0x00, 0x00, 0x00);            // Size of initialized data
            executable.Add(0x00, 0x00, 0x00, 0x00);            // Size of uninitialized data
            executable.Add(0x00, 0x10, 0x00, 0x00);            // Address of entry point
            executable.Add(0x00, 0x10, 0x00, 0x00);            // Base of code

            // Windows Specific Optional Header fields - https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#optional-header-windows-specific-fields-image-only
            executable.Add(0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00); // ImageBase
            executable.Add(0x00, 0x10, 0x00, 0x00);                         // Section allignment
            executable.Add(0x00, 0x02, 0x00, 0x00);                         // File allignment
            executable.Add(0x06, 0x00);                                     // Major OS version
            executable.Add(0x00, 0x00);                                     // Minor OS version
            executable.Add(0x00, 0x00);                                     // Major image version
            executable.Add(0x00, 0x00);                                     // Minor image version
            executable.Add(0x06, 0x00);                                     // Major subsystem version
            executable.Add(0x00, 0x00);                                     // Minor subsystem version
            executable.Add(0x00, 0x00, 0x00, 0x00);                         // Win32 Version Value (must be zero)
            executable.Add(0x00, 0x20, 0x00, 0x00);                         // Size of image
            executable.Add(0x00, 0x02, 0x00, 0x00);                         // Size of headers
            executable.Add(0x00, 0x00, 0x00, 0x00);                         // Checksum
            executable.Add(0x03, 0x00);                                     // Subsystem (console)
            executable.Add(0x00, 0x00);                                     // DLL characteristics
            executable.Add(0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00); // Size of stack reserve
            executable.Add(0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Size of stack commit
            executable.Add(0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00); // Size of heap reserve
            executable.Add(0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Size of heap commit
            executable.Add(0x00, 0x00, 0x00, 0x00);                         // Loader flags (must be zero)
            executable.Add(0x10, 0x00, 0x00, 0x00);                         // Number of RVA and sizes

            // Optional Header Data Directories (Image Only) - https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#optional-header-data-directories-image-only
            executable.Add(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Export Directory
            executable.Add(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Import Directory
            executable.Add(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Resource Directory
            executable.Add(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Exception Directory
            executable.Add(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Certificates Directory
            executable.Add(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Base Relocation Directory
            executable.Add(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Debug Directory
            executable.Add(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Architecture Directory
            executable.Add(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Global Pointer Directory
            executable.Add(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Thread Storage Directory
            executable.Add(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Load Configuration Directory
            executable.Add(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Bound Import Directory
            executable.Add(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Import Address Table Directory
            executable.Add(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Delay Import Directory
            executable.Add(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // COM Descriptor Directory
            executable.Add(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00); // Reserved Directory (must be zero)

            // Section Table (Section Headers) - https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#section-table-section-headers
            executable.Add(0x2E, 0x74, 0x65, 0x78, 0x74, 0x00, 0x00, 0x00); // Name (.text)
            executable.Add(BitConverter.GetBytes((uint)text.Count));        // Virtual size
            executable.Add(0x00, 0x10, 0x00, 0x00);                         // Virtual address

            for (var i = text.Count; i < sizeOfCode; i++)
                text.Add(0);

            executable.Add(BitConverter.GetBytes((uint)text.Count));        // Size of raw data
            executable.Add(0x00, 0x02, 0x00, 0x00);                         // Pointer to raw data
            executable.Add(0x00, 0x00, 0x00, 0x00);                         // Pointer to relocations
            executable.Add(0x00, 0x00, 0x00, 0x00);                         // Pointer to line numbers
            executable.Add(0x00, 0x00);                                     // Number of relocations
            executable.Add(0x00, 0x00);                                     // Number of line numbers
            executable.Add(0x20, 0x00, 0x00, 0x60);                         // Characteristics (IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ)

            // .text section
            // Pad .text to Pointer to raw data.
            while (executable.Count != 0x200)
                executable.Add(0);

            executable.AddRange(text);

            WriteAllBytes(output, executable.ToArray());
        }

        private static List<byte> Generate(Program program, bool includePropositions)
        {
            return new List<byte>
            {
                // xor eax, eax
                0x31, 0xC0,

                // ret
                0xC3
            };
        }

        private static void Add(this List<byte> list, params byte[] data) => list.AddRange(data);
    }
}