#include "runner.h"

#pragma warning(push, 0)
#include <windows.h>
#include <shlwapi.h>
#pragma warning(pop)

#include <stdio.h>

bool file_exists(const char (* file_name)[MAX_PATH_LENGTH])
{
    return PathFileExists(*file_name);
}

void delete_directory(const char(* directory)[MAX_PATH_LENGTH])
{
    if (!PathFileExists(*directory))
        return;

    char path[MAX_PATH_LENGTH];
    strncpy(path, *directory, MAX_PATH_LENGTH);

    size_t directory_length = strlen(*directory);
    path[directory_length] = '*';
    path[directory_length + 1] = '\0';

    WIN32_FIND_DATA fd;
    HANDLE handle = FindFirstFile(path, &fd);

    if (handle == INVALID_HANDLE_VALUE)
        return;

    do
    {
        if (!strcmp(".", fd.cFileName) || !strcmp("..", fd.cFileName))
            continue;

        strncpy(&path[directory_length], fd.cFileName, MAX_PATH_LENGTH - directory_length);
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        {
            size_t file_name_length = strlen(fd.cFileName);
            path[directory_length + file_name_length] = '/';
            path[directory_length + file_name_length + 1] = '\0';

            delete_directory(&path);
        }
        else if (!DeleteFile(path))
        {
            fprintf(stderr, "Could not delete %s.\n", &path);
            exit(EXIT_FAILURE);
        }
    } while (FindNextFile(handle, &fd));

    FindClose(handle);
    RemoveDirectory(*directory);
}

static void enable_ansi_codes(void)
{
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode;

    GetConsoleMode(output, &dwMode);
    dwMode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    if (!SetConsoleMode(output, dwMode))
    {
        fprintf(stderr, "Could not enable ANSI codes.\n");
        exit(EXIT_FAILURE);
    }
}

static char compiler_path[MAX_PATH_LENGTH];
static void find_compiler_path(void)
{
    uint64_t compiler_path_length = GetCurrentDirectory(MAX_PATH_LENGTH, compiler_path);

    compiler_path[compiler_path_length] = '\\';
    strcpy(&compiler_path[compiler_path_length + 1], "owen.exe");
    compiler_path[compiler_path_length + strlen("owen.exe") + 1] = '\0';
}

void platform_specific_initialization(void)
{
    enable_ansi_codes();
    find_compiler_path();
}

void ensure_directory_exists(const char (* innermost_directory)[MAX_PATH_LENGTH])
{
    char outer_directory[MAX_PATH_LENGTH];

    char* end = strchr(*innermost_directory, '/');
    while (end)
    {
        strncpy(outer_directory, *innermost_directory, end - *innermost_directory + 1);
        outer_directory[end - *innermost_directory + 1] = 0;

        if (!CreateDirectory(outer_directory, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
        {
            fprintf(stderr, "Could not create %s\n", outer_directory);
            exit(EXIT_FAILURE);
        }

        end = strchr(++end, '/');
    }
}

void run_test(void)
{
    SECURITY_ATTRIBUTES attributes =
    {
        .nLength = sizeof(attributes),
        .bInheritHandle = true,
        .lpSecurityDescriptor = NULL
    };

    HANDLE owen_out_read = NULL;
    HANDLE owen_out_write = NULL;

    if (!CreatePipe(&owen_out_read, &owen_out_write, &attributes, 0))
    {
        fprintf(stderr, "Could not out pipe for: %s\n", test.command_line);
        exit(EXIT_FAILURE);
    }

    PROCESS_INFORMATION pi = { 0 };
    STARTUPINFO si = { 0 };
    si.cb = sizeof(si);
    si.hStdError = owen_out_write;
    si.hStdOutput = owen_out_write;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.dwFlags |= STARTF_USESTDHANDLES;

    if (!CreateProcess(compiler_path, test.command_line, NULL, NULL, true, 0, NULL, NULL, &si, &pi))
    {
        fprintf(stderr, "Could not create process: %s\n", test.command_line);
        exit(EXIT_FAILURE);
    }

    CloseHandle(owen_out_write);
    
    char buffer[sizeof(test.expected_error) + sizeof(test.expected_semantics) + sizeof(test.expected_ir) + sizeof(test.expected_pe)];
    DWORD buffer_length = 0;
    DWORD exit_code;
    DWORD bytes_read;

    while (true)
    {
        GetExitCodeProcess(pi.hProcess, &exit_code);
        if (exit_code != STILL_ACTIVE)
            break;

        bool success = ReadFile(owen_out_read, buffer + buffer_length, sizeof(buffer) - buffer_length, &bytes_read, NULL);
        if (!success || !bytes_read)
            break;

        buffer_length += bytes_read;
    }

    ReadFile(owen_out_read, buffer + buffer_length, sizeof(buffer) - buffer_length, &bytes_read, NULL);
    buffer_length += bytes_read;
    buffer[buffer_length] = '\0';

    GetExitCodeProcess(pi.hProcess, &exit_code);
    if (exit_code)
        strcpy(test_result.actual_error, buffer);
    else
    {
        // TODO: Read until the first double NEW_LINE or end of the buffer.
        // Each part is printed in a fixed order and only errors have 
        // NEW_LINE NEW_LINE. So I can use NEW_LINE NEW_LINE for splitting 
        // the printed output.
        // 
        // This also means that I should have a platform independant 
        // function for parsing the output.
        if (test.expected_semantics[0])
            strcpy(test_result.actual_semantics, buffer);

        if (test.expected_ir[0])
            strcpy(test_result.actual_ir, buffer);

        if (test.expected_pe[0])
            strcpy(test_result.actual_pe, buffer);
    }

    CloseHandle(owen_out_read);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}
