/*
 * colors.h - ANSI color code definitions for terminal output
 *
 * Provides standard ANSI escape sequences for colored terminal output.
 * Colors are used throughout the parser for:
 * - IP addresses (cyan)
 * - Transaction IDs (yellow)
 * - Unit IDs (green)
 * - Function codes (magenta)
 * - Data fields (blue)
 * - Headers (white)
 * - Separators (gray)
 *
 * All colors should be reset with COLOR_RESET after use to avoid
 * bleeding into subsequent output.
 *
 * @note ANSI codes may not work in all terminals (Windows CMD, some IDEs)
 * @note Works well in: Linux terminals, macOS Terminal, Windows Terminal, MSYS2
 * 
 * Copyright (C) 2025 Marty
 * SPDX-License-Identifier: GPL-3.0-or-later
 */



#ifndef COLORS_H
#define COLORS_H

/* ANSI Color codes */
static const char COLOR_RESET[]   = "\033[0m";
static const char COLOR_CYAN[]    = "\033[36m";
static const char COLOR_YELLOW[]  = "\033[33m";
static const char COLOR_GREEN[]   = "\033[32m";
static const char COLOR_MAGENTA[] = "\033[35m";
static const char COLOR_BLUE[]    = "\033[34m";
static const char COLOR_WHITE[]   = "\033[37m";
static const char COLOR_GRAY[]    = "\033[90m";

#endif /* COLORS_H */