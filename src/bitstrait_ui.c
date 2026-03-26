#include "../include/bitstrait_ui.h"
#include <ncurses.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void UI_Init() {
    initscr();
    start_color();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, true);

    init_pair(1, COLOR_CYAN, COLOR_BLACK);  // Framing/Headers
    init_pair(2, COLOR_GREEN, COLOR_BLACK); // Success
    init_pair(3, COLOR_RED, COLOR_BLACK);   // Errors/Danger Zeros
}

void UI_Draw(const BitStrait_Telemetry_t *data, const uint8_t *legacy_raw,
             const uint8_t *cobs_encoded, size_t enc_length, bool valid,
             bool fault) {
    int row, col;
    getmaxyx(stdscr, row, col);

    // Rule: Handle hardware/window constraints gracefully
    if (row < 22 || col < 65) {
        erase();
        mvprintw(0, 0, "Terminal too small! Resize to at least 65x22.");
        refresh();
        return;
    }

    erase();

    // --- HEADER ---
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(1, 2, "=== BITSTRAIT: ARCHITECTURAL PROGRESSION ===");
    attroff(COLOR_PAIR(1) | A_BOLD);

    // --- SECTION 1: LEGACY (V1) ---
    mvprintw(3, 2, "[ V1: LEGACY PACKING ] - High Risk (No Framing)");
    mvprintw(4, 4, "Buffer: ");
    for (size_t i = 0; i < 5; i++) {
        bool is_zero = (legacy_raw[i] == 0x00);
        if (is_zero) attron(COLOR_PAIR(3) | A_BOLD);
        printw("%02X ", legacy_raw[i]);
        if (is_zero) attroff(COLOR_PAIR(3) | A_BOLD);
    }
    mvprintw(5, 4, "Status: Raw block. Vulnerable to byte-shift desync.");

    mvaddch(7, 2, ACS_HLINE); 

    // --- SECTION 2: MODERN (V2) ---
    mvprintw(9, 2, "[ V2: COBS ENCODING ] - Industry Standard");
    mvprintw(10, 4, "Wire:   ");
    for (size_t i = 0; i < enc_length; i++) {
        // Rule: Distinct visual cues for framing bytes
        bool is_framing = (i == 0 || cobs_encoded[i] == 0x00);
        if (is_framing) attron(COLOR_PAIR(1) | A_BOLD);
        printw("%02X ", cobs_encoded[i]);
        if (is_framing) attroff(COLOR_PAIR(1) | A_BOLD);
    }
    mvprintw(11, 4, "Status: %zu-byte framed stream. 0x00 used for Sync.", enc_length);

    // --- SECTION 3: SYSTEM HEALTH ---
    mvaddch(13, 2, ACS_HLINE);
    mvprintw(15, 2, "[ SYSTEM DIAGNOSTICS ]");
    mvprintw(16, 4, "BATT: %-3d%%  ALT: %-4dm  CRC: ", data->battery, data->altitude);
    
    if (valid) {
        attron(COLOR_PAIR(2) | A_BOLD);
        printw("MATCH [VALID]");
        attroff(COLOR_PAIR(2) | A_BOLD);
    } else {
        attron(COLOR_PAIR(3) | A_BLINK | A_BOLD);
        printw("ERROR [CORRUPT]");
        attroff(COLOR_PAIR(3) | A_BLINK | A_BOLD);
    }

    mvprintw(18, 2, "FAULT INJECTION: [%s]", fault ? "ACTIVE (BIT-FLIP)" : "OFF");
    mvprintw(20, 2, "KEYS: [F] Toggle Fault | [Q] Quit");
    
    refresh();
}

void UI_Cleanup() { 
    endwin(); 
}
