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

  init_pair(1, COLOR_CYAN, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_RED, COLOR_BLACK);
}

void UI_Draw(const BitStrait_Telemetry_t *data, const uint8_t *legacy_raw,
             const uint8_t *cobs_encoded, size_t enc_length, bool valid,
             bool fault) {
  erase();
  attron(COLOR_PAIR(1) | A_BOLD);
  mvprintw(1, 2, "=== BITSTRAIT: ARCHITECTURAL PROGRESSION ===");
  attroff(COLOR_PAIR(1) | A_BOLD);

  // --- SECTION 1: LEGACY (V1) ---
  mvprintw(3, 2, "[ V1: LEGACY PACKING ] - High Risk (No Framing)");
  mvprintw(4, 4, "Buffer: ");
  for (int i = 0; i < FRAME_SIZE; i++) {
    // Highlight zeros in red to show why they are "dangerous" for framing
    if (legacy_raw[i] == 0)
      attron(COLOR_PAIR(3));
    printw("%02X ", legacy_raw[i]);
    if (legacy_raw[i] == 0)
      attroff(COLOR_PAIR(3));
  }
  mvprintw(5, 4, "Status: Raw 5-byte block. Vulnerable to desync.");

  mvaddch(6, 2, ACS_HLINE); // Horizontal line

  // --- SECTION 2: MODERN (V2) ---
  mvprintw(8, 2, "[ V2: COBS ENCODING ] - Industry Standard");
  mvprintw(9, 4, "Wire:   ");
  for (size_t i = 0; i < enc_length; i++) {
    // Highlight the COBS Overhead (first byte) and Delimiter (last byte)
    if (i == 0 || cobs_encoded[i] == 0x00)
      attron(COLOR_PAIR(1));
    printw("%02X ", cobs_encoded[i]);
    if (i == 0 || cobs_encoded[i] == 0x00)
      attroff(COLOR_PAIR(1));
  }
  mvprintw(10, 4, "Status: %zu-byte framed stream. 0x00 used for Sync.",
           enc_length);

  // --- SECTION 3: SYSTEM HEALTH ---
  mvprintw(12, 2, "[ SYSTEM DIAGNOSTICS ]");
  mvprintw(13, 4, "BATT: %-3d%%  ALT: %-4dm  CRC: ", data->battery,
           data->altitude);
  if (valid) {
    attron(COLOR_PAIR(2));
    printw("MATCH");
    attroff(COLOR_PAIR(2));
  } else {
    attron(COLOR_PAIR(3) | A_BLINK);
    printw("ERROR");
    attroff(COLOR_PAIR(3));
  }

  mvprintw(15, 2, "FAULT INJECTION: %s", fault ? "ACTIVE (BIT-FLIP)" : "OFF");
  mvprintw(17, 2, "KEYS: [F] Toggle Fault | [Q] Quit");
  refresh();
}

void UI_Cleanup() { endwin(); }
