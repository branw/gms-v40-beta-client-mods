#ifndef MAPLE_B8DE3FCBEC5A49BD979B3384D1CEB0C5_HPP
#define MAPLE_B8DE3FCBEC5A49BD979B3384D1CEB0C5_HPP

#include <array>

const auto cursor_width = 24;
const auto cursor_height = 23;

const std::array<uint8_t, cursor_width * cursor_height * 4> cursor_bitmap {
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x11, 0x99, 0xbb, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xdd, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x11, 0xbb, 0xcc, 0xff, 0x11, 0x99, 0xbb, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xee, 0xff, 0xff, 0xff,
        0xee, 0xff, 0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff, 0x22, 0x22, 0x22, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x11, 0xbb, 0xdd, 0xff, 0x11, 0xbb, 0xdd, 0xff, 0x33, 0xbb, 0xcc, 0xff, 0xcc, 0xee, 0xff, 0xff, 0x99, 0xee, 0xee, 0xff,
        0x66, 0xee, 0xee, 0xff, 0x55, 0xee, 0xff, 0xff, 0x33, 0xdd, 0xff, 0xff, 0x55, 0xee, 0xff, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xaa, 0xff, 0xff, 0xff,
        0x55, 0xee, 0xff, 0xff, 0x33, 0xee, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x00, 0xcc, 0xee, 0xff, 0x11, 0xcc, 0xdd, 0xff, 0x11, 0xdd, 0xee, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xee, 0xff, 0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xdd, 0xff, 0xff, 0xff,
        0xbb, 0xff, 0xff, 0xff, 0x77, 0xee, 0xff, 0xff, 0x33, 0xee, 0xff, 0xff, 0x11, 0xdd, 0xff, 0xff, 0x11, 0xdd, 0xee, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xcc, 0xee, 0xff, 0xbb, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xff,
        0x88, 0xff, 0xff, 0xff, 0x22, 0xaa, 0xcc, 0xff, 0x11, 0x99, 0xaa, 0xff, 0x11, 0xaa, 0xcc, 0xff, 0x11, 0xbb, 0xdd, 0xff, 0x00, 0xbb, 0xdd, 0xff,
        0x00, 0xaa, 0xdd, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xdd, 0xee, 0xff, 0xcc, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xee, 0xee, 0xee, 0xff, 0xcc, 0xbb, 0x99, 0xff, 0xbb, 0x99, 0x55, 0xff,
        0x88, 0xff, 0xff, 0xff, 0x22, 0x99, 0xbb, 0xff, 0x99, 0x55, 0x11, 0xff, 0x77, 0x77, 0x44, 0xff, 0x55, 0x88, 0x77, 0xff, 0x22, 0xaa, 0xbb, 0xff,
        0x11, 0xaa, 0xcc, 0xff, 0x11, 0x99, 0xbb, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xdd, 0xee, 0xff, 0xdd, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
        0xdd, 0xff, 0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xdd, 0xcc, 0xaa, 0xff, 0xdd, 0x99, 0x44, 0xff, 0xdd, 0x99, 0x22, 0xff, 0xdd, 0x99, 0x33, 0xff,
        0x77, 0xee, 0xff, 0xff, 0x22, 0xaa, 0xbb, 0xff, 0xaa, 0x44, 0x00, 0xff, 0xbb, 0x44, 0x00, 0xff, 0x99, 0x33, 0x00, 0xff, 0x77, 0x44, 0x22, 0xff,
        0x44, 0x77, 0x77, 0xff, 0x11, 0x88, 0xbb, 0xff, 0x11, 0x88, 0xaa, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xcc, 0xee, 0xff, 0xaa, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0x99, 0xff, 0xff, 0xff,
        0xaa, 0xff, 0xff, 0xff, 0xbb, 0xcc, 0xaa, 0xff, 0xdd, 0x99, 0x44, 0xff, 0xee, 0xbb, 0x55, 0xff, 0xff, 0xcc, 0x77, 0xff, 0xff, 0xdd, 0x99, 0xff,
        0xbb, 0x88, 0x55, 0xff, 0x99, 0x44, 0x22, 0xff, 0xbb, 0x55, 0x00, 0xff, 0xaa, 0x44, 0x00, 0xff, 0x88, 0x22, 0x00, 0xff, 0x77, 0x22, 0x00, 0xff,
        0x77, 0x22, 0x11, 0xff, 0x44, 0x66, 0x77, 0xff, 0x11, 0x88, 0xaa, 0xff, 0x11, 0x77, 0x99, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xaa, 0xff, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xff, 0x55, 0xee, 0xff, 0xff,
        0x77, 0xdd, 0xdd, 0xff, 0xcc, 0x99, 0x44, 0xff, 0xee, 0xbb, 0x66, 0xff, 0xff, 0xee, 0xbb, 0xff, 0xff, 0xee, 0xcc, 0xff, 0xff, 0xee, 0xcc, 0xff,
        0xff, 0xdd, 0x99, 0xff, 0xdd, 0xaa, 0x44, 0xff, 0xcc, 0x66, 0x00, 0xff, 0xbb, 0x55, 0x00, 0xff, 0x88, 0x33, 0x00, 0xff, 0x77, 0x22, 0x00, 0xff,
        0x77, 0x22, 0x00, 0xff, 0x77, 0x33, 0x22, 0xff, 0x33, 0x77, 0x88, 0xff, 0x11, 0x66, 0x99, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x22, 0xdd, 0xff, 0xff, 0x66, 0xee, 0xff, 0xff, 0x22, 0xee, 0xff, 0xff,
        0x77, 0xaa, 0x88, 0xff, 0xee, 0x99, 0x33, 0xff, 0xff, 0xdd, 0x99, 0xff, 0xff, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xcc, 0xff,
        0xff, 0xdd, 0x88, 0xff, 0xee, 0xaa, 0x33, 0xff, 0xcc, 0x77, 0x00, 0xff, 0xaa, 0x55, 0x00, 0xff, 0x99, 0x44, 0x00, 0xff, 0x88, 0x33, 0x11, 0xff,
        0x88, 0x33, 0x11, 0xff, 0x88, 0x22, 0x11, 0xff, 0x55, 0x55, 0x55, 0xff, 0x11, 0x66, 0x99, 0xff, 0x11, 0x66, 0x88, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xcc, 0xee, 0xff, 0x00, 0xdd, 0xff, 0xff, 0x00, 0xdd, 0xff, 0xff,
        0xaa, 0x88, 0x44, 0xff, 0xee, 0x99, 0x33, 0xff, 0xff, 0xcc, 0x88, 0xff, 0xff, 0xee, 0xcc, 0xff, 0xff, 0xee, 0xcc, 0xff, 0xff, 0xee, 0xaa, 0xff,
        0xee, 0xcc, 0x66, 0xff, 0xdd, 0x99, 0x22, 0xff, 0xcc, 0x66, 0x00, 0xff, 0xaa, 0x55, 0x00, 0xff, 0xaa, 0x44, 0x00, 0xff, 0xaa, 0x55, 0x22, 0xff,
        0x99, 0x55, 0x44, 0xff, 0x99, 0x44, 0x33, 0xff, 0x77, 0x44, 0x33, 0xff, 0x11, 0x77, 0xaa, 0xff, 0x11, 0x66, 0x88, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xcc, 0xee, 0xff, 0x00, 0xcc, 0xee, 0xff, 0x77, 0xee, 0xff, 0xff,
        0xaa, 0xff, 0xff, 0xff, 0x66, 0xee, 0xff, 0xff, 0xdd, 0x99, 0x44, 0xff, 0xff, 0xcc, 0x99, 0xff, 0xff, 0xdd, 0x88, 0xff, 0xee, 0xbb, 0x55, 0xff,
        0xdd, 0x99, 0x22, 0xff, 0xcc, 0x77, 0x00, 0xff, 0xbb, 0x55, 0x00, 0xff, 0xaa, 0x55, 0x00, 0xff, 0xaa, 0x55, 0x11, 0xff, 0xaa, 0x66, 0x33, 0xff,
        0xaa, 0x66, 0x55, 0xff, 0x11, 0xbb, 0xdd, 0xff, 0x00, 0xbb, 0xdd, 0xff, 0x00, 0xaa, 0xcc, 0xff, 0x11, 0x66, 0x99, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xbb, 0xdd, 0xff, 0x11, 0x77, 0x99, 0xff,
        0x11, 0x66, 0x88, 0xff, 0x11, 0x88, 0xaa, 0xff, 0xcc, 0x55, 0x11, 0xff, 0xee, 0xaa, 0x55, 0xff, 0xdd, 0x99, 0x33, 0xff, 0xdd, 0x88, 0x00, 0xff,
        0xcc, 0x77, 0x00, 0xff, 0xbb, 0x66, 0x00, 0xff, 0xaa, 0x55, 0x00, 0xff, 0x99, 0x44, 0x00, 0xff, 0xaa, 0x55, 0x11, 0xff, 0xaa, 0x66, 0x44, 0xff,
        0xbb, 0x77, 0x66, 0xff, 0x22, 0x66, 0x88, 0xff, 0x11, 0x66, 0x88, 0xff, 0x11, 0x77, 0x99, 0xff, 0x11, 0x77, 0x99, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xbb, 0xdd, 0xff, 0x11, 0x88, 0xaa, 0xff,
        0x99, 0x66, 0x33, 0xff, 0xbb, 0x44, 0x00, 0xff, 0xbb, 0x44, 0x11, 0xff, 0xcc, 0x77, 0x22, 0xff, 0xcc, 0x66, 0x00, 0xff, 0xcc, 0x55, 0x00, 0xff,
        0xbb, 0x55, 0x00, 0xff, 0x99, 0x44, 0x00, 0xff, 0x88, 0x33, 0x00, 0xff, 0x88, 0x33, 0x00, 0xff, 0x99, 0x55, 0x22, 0xff, 0xbb, 0x77, 0x55, 0xff,
        0xbb, 0x88, 0x77, 0xff, 0x99, 0x66, 0x44, 0xff, 0x77, 0x44, 0x33, 0xff, 0x00, 0x66, 0x88, 0xff, 0x11, 0x77, 0x99, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0xaa, 0xcc, 0xff, 0x11, 0x99, 0xcc, 0xff,
        0x66, 0x77, 0x66, 0xff, 0xcc, 0x55, 0x00, 0xff, 0xcc, 0x66, 0x11, 0xff, 0xcc, 0x66, 0x33, 0xff, 0xcc, 0x66, 0x22, 0xff, 0xbb, 0x55, 0x00, 0xff,
        0x99, 0x33, 0x00, 0xff, 0x88, 0x22, 0x00, 0xff, 0x88, 0x33, 0x00, 0xff, 0x99, 0x44, 0x11, 0xff, 0xaa, 0x77, 0x55, 0xff, 0xcc, 0xaa, 0x88, 0xff,
        0xdd, 0xbb, 0xaa, 0xff, 0xcc, 0xaa, 0x88, 0xff, 0x88, 0x88, 0x77, 0xff, 0x11, 0x99, 0xbb, 0xff, 0x11, 0x99, 0xbb, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x11, 0x99, 0xbb, 0xff,
        0x33, 0x88, 0x99, 0xff, 0x99, 0x55, 0x22, 0xff, 0xcc, 0x55, 0x22, 0xff, 0xcc, 0x77, 0x44, 0xff, 0xcc, 0x77, 0x44, 0xff, 0xaa, 0x44, 0x11, 0xff,
        0x99, 0x33, 0x00, 0xff, 0x99, 0x33, 0x11, 0xff, 0x99, 0x44, 0x22, 0xff, 0xaa, 0x66, 0x44, 0xff, 0xcc, 0xaa, 0x88, 0xff, 0xdd, 0xcc, 0xbb, 0xff,
        0xdd, 0xbb, 0xaa, 0xff, 0xbb, 0x99, 0x88, 0xff, 0x55, 0x99, 0xaa, 0xff, 0x22, 0xaa, 0xcc, 0xff, 0x11, 0x99, 0xbb, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x11, 0x88, 0xbb, 0xff,
        0x11, 0x88, 0xaa, 0xff, 0x55, 0x77, 0x77, 0xff, 0xaa, 0x44, 0x11, 0xff, 0xbb, 0x44, 0x22, 0xff, 0xbb, 0x55, 0x22, 0xff, 0xaa, 0x33, 0x11, 0xff,
        0x99, 0x44, 0x22, 0xff, 0xaa, 0x44, 0x22, 0xff, 0xaa, 0x66, 0x44, 0xff, 0xbb, 0x88, 0x66, 0xff, 0xdd, 0xbb, 0xaa, 0xff, 0xdd, 0xbb, 0xaa, 0xff,
        0xcc, 0xaa, 0x88, 0xff, 0x88, 0x99, 0x99, 0xff, 0x44, 0xbb, 0xdd, 0xff, 0x44, 0xaa, 0xcc, 0xff, 0x22, 0xaa, 0xbb, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
        0x11, 0x77, 0x99, 0xff, 0x11, 0x77, 0x99, 0xff, 0x44, 0x66, 0x66, 0xff, 0x88, 0x44, 0x22, 0xff, 0xaa, 0x33, 0x00, 0xff, 0x99, 0x22, 0x00, 0xff,
        0x44, 0x99, 0xbb, 0xff, 0x22, 0x66, 0x88, 0xff, 0x99, 0x44, 0x33, 0xff, 0xbb, 0x77, 0x66, 0xff, 0xcc, 0x99, 0x77, 0xff, 0xbb, 0x99, 0x88, 0xff,
        0x88, 0xaa, 0x99, 0xff, 0x55, 0xcc, 0xdd, 0xff, 0x77, 0xcc, 0xee, 0xff, 0x11, 0x66, 0x88, 0xff, 0x44, 0xbb, 0xcc, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0x11, 0x66, 0x88, 0xff, 0x11, 0x66, 0x88, 0xff, 0x22, 0x66, 0x77, 0xff, 0x55, 0x55, 0x55, 0xff, 0x77, 0x44, 0x33, 0xff,
        0x44, 0x99, 0xaa, 0xff, 0x11, 0x66, 0x88, 0xff, 0x66, 0x22, 0x11, 0xff, 0x88, 0x66, 0x55, 0xff, 0x88, 0x88, 0x88, 0xff, 0x66, 0xaa, 0xbb, 0xff,
        0x77, 0xcc, 0xee, 0xff, 0x99, 0xdd, 0xee, 0xff, 0x00, 0x11, 0x22, 0xff, 0x11, 0x55, 0x77, 0xff, 0x55, 0xbb, 0xdd, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x11, 0x55, 0x77, 0xff, 0x11, 0x55, 0x77, 0xff, 0x11, 0x55, 0x77, 0xff, 0x11, 0x55, 0x77, 0xff,
        0x33, 0x88, 0x99, 0xff, 0x11, 0x77, 0x99, 0xff, 0x11, 0x55, 0x77, 0xff, 0x33, 0x99, 0xbb, 0xff, 0x55, 0xbb, 0xcc, 0xff, 0x77, 0xcc, 0xee, 0xff,
        0xaa, 0xdd, 0xee, 0xff, 0x33, 0x99, 0xaa, 0xff, 0x22, 0x88, 0xaa, 0xff, 0x22, 0x77, 0x99, 0xff, 0x88, 0xdd, 0xee, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x11, 0x55, 0x77, 0xff, 0x11, 0x55, 0x77, 0xff,
        0x22, 0x66, 0x88, 0xff, 0x22, 0x77, 0x99, 0xff, 0x22, 0x77, 0x99, 0xff, 0x55, 0xaa, 0xcc, 0xff, 0x88, 0xcc, 0xdd, 0xff, 0x88, 0xcc, 0xdd, 0xff,
        0x99, 0xdd, 0xee, 0xff, 0x99, 0xdd, 0xee, 0xff, 0x99, 0xdd, 0xee, 0xff, 0x99, 0xdd, 0xee, 0xff, 0x99, 0xdd, 0xee, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
        0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff
};

#endif//MAPLE_B8DE3FCBEC5A49BD979B3384D1CEB0C5_HPP
