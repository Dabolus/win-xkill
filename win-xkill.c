/************************************************************************
 * Copyright © 2017 Giorgio Garasto.                                    *
 *                                                                      *
 * This file is part of win-xkill                                       *
 *                                                                      *
 * win-xkill is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * win-xkill is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with win-xkill. If not, see <http://www.gnu.org/licenses/>.    *
 ************************************************************************/

#define OEMRESOURCE

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdbool.h>
#include <ctype.h>

#include "cursor.h"

#define VK_ANYBUTTON (VK_LBUTTON | VK_MBUTTON | VK_RBUTTON)

static bool CALLBACK TerminateAppEnum(HWND hwnd, LPARAM lParam);

static DWORD WINAPI TerminateApp(DWORD dwPID, DWORD dwTimeout);

static DWORD parse_id(char *s);

static int parse_button(char *s, int *button);

static void usage(void);

int _tmain(int argc, _TCHAR *argv[]) {
    int i; // iterator, temp variable
    bool done = false; // loop condition, temp variable
    DWORD id = 0; // resource to kill
    char *button_name = NULL; // name of button for window select
    int button; // button number
    HCURSOR normal =
            CopyCursor((HCURSOR) LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0,
                                           LR_SHARED)); // backup of the original cursor
    HCURSOR cursor =
            CreateCursor(GetCurrentProcess(), 16, 8, 32, 32, cursorANDMask, cursorXORMask); // custom cross cursor

    for (i = 1; i < argc; i++) {
        char *arg = argv[i];

        if (arg[0] == '/') {
            switch (arg[1]) {
                case 'i': // /id resourceid
                    if (++i >= argc) usage();
                    id = parse_id(argv[i]);
                    continue;
                case 'b': // /button number
                    if (++i >= argc) usage();
                    button_name = argv[i];
                    continue;
                    // TODO: Add unimplemented arguments
                default:
                    fprintf(stderr, "Invalid option %s.\n\n", arg);
                    usage();
                    break;
            }
        } else
            fprintf(stderr, "Invalid option %s.\n\n", arg);
        usage();
    }

    if (!id) {
        if (!button_name)
            button_name = "1";
        if (button_name && !parse_button(button_name, &button)) {
            fprintf(stderr, "xkill:  invalid button specification \"%s\"\n", button_name);
            return EXIT_FAILURE;
        }

        if (button != VK_LBUTTON && button != VK_MBUTTON && button != VK_RBUTTON && button != VK_ANYBUTTON) {
            fprintf(stderr, "xkill:  no button number %d in pointer map, can't select window\n", button);
            return EXIT_FAILURE;
        }

        switch (button) {
            case VK_LBUTTON:
                button_name = "button 1";
                break;
            case VK_MBUTTON:
                button_name = "button 2";
                break;
            case VK_RBUTTON:
                button_name = "button 3";
                break;
            case VK_ANYBUTTON:
                button_name = "any button";
                break;
            default:
                button_name = "button 1";
                break;
        }

        printf("Select the window whose client you wish to kill with %s....\n", button_name);
        SetSystemCursor(cursor, OCR_NORMAL); // set the custom cursor
        while (!done) {
            bool condition;
            if (button == VK_ANYBUTTON)
                condition = (GetAsyncKeyState(VK_LBUTTON) || GetAsyncKeyState(VK_MBUTTON) ||
                             GetAsyncKeyState(VK_RBUTTON));
            else
                condition = (bool) GetAsyncKeyState(button);
            if (condition) {
                POINT mousePos;
                if (!GetCursorPos(&mousePos)) {
                    fprintf(stderr, "xkill:  no pointer mapping, can't select window\n");
                    SetSystemCursor(normal, OCR_NORMAL);
                    return EXIT_FAILURE;
                }
                HWND hWnd = WindowFromPoint(mousePos);
                if (!hWnd) {
                    fprintf(stderr, "xkill:  Unable to locate selected window\n");
                    SetSystemCursor(normal, OCR_NORMAL);
                    return EXIT_FAILURE;
                }
                GetWindowThreadProcessId(hWnd, &id);
                done = true;
            }
        }
    }
    SetSystemCursor(normal, OCR_NORMAL); // restore the normal cursor
    printf("xkill:  killing creator of resource 0x%lx\n", id);
    if (!TerminateApp(id, 100)) {
        fprintf(stderr, "xkill:  unable to kill the process\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static bool CALLBACK TerminateAppEnum(HWND hwnd, LPARAM lParam) {
    DWORD dwID;
    GetWindowThreadProcessId(hwnd, &dwID);
    if (dwID == (DWORD) lParam)
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    return true;
}

static DWORD WINAPI TerminateApp(DWORD dwPID, DWORD dwTimeout) {
    HANDLE hProc;
    DWORD dwRet;
    hProc = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, dwPID);
    if (hProc == NULL)
        return 0;
    EnumWindows((WNDENUMPROC) TerminateAppEnum, (LPARAM) dwPID);
    if (WaitForSingleObject(hProc, dwTimeout) != WAIT_OBJECT_0)
        dwRet = (TerminateProcess(hProc, 0) ? 1 : 0);
    else
        dwRet = 1;
    CloseHandle(hProc);
    return dwRet;
}

static DWORD parse_id(char *s) {
    DWORD retval = 0;
    char *fmt = "%ld";

    if (s) {
        if (*s == '0')
            s++, fmt = "%lo";
        if (*s == 'x' || *s == 'X')
            s++, fmt = "%lx";
        sscanf(s, fmt, &retval);
    }
    return retval;
}

static int parse_button(char *s, int *button) {
    register char *cp;
    for (cp = s; *cp; cp++) {
        if (isascii(*cp) && isupper(*cp)) {
#ifdef _tolower
            *cp = (char)_tolower(*cp);
#else
            *cp = (char) tolower(*cp);
#endif
        }
    }

    if (!strcmp(s, "any")) {
        *button = VK_ANYBUTTON;
        return 1;
    }

    // check for non-numeric input
    for (cp = s; *cp; cp++) {
        if (!(isascii(*cp) && isdigit(*cp))) // bogus name
            return 0;
    }

    *button = atoi(s);
    switch (*button) {
        case 1:
            *button = VK_LBUTTON;
            break;
        case 2:
            *button = VK_MBUTTON;
            break;
        case 3:
            *button = VK_RBUTTON;
            break;
        default:
            *button = VK_LBUTTON;
            break;
    }
    return 1;
}

static void usage(void) {
    const char *options =
            "where options include:\n"
                    "    /id resource            resource whose client is to be killed\n"
                    "    /button number          specific button to be pressed to select window\n"
                    "\n";

    fprintf(stderr, "usage:  xkill [/option ...]\n%s", options);
    exit(1);
}
