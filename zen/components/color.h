#ifndef COLOR_H
#define COLOR_H

#include "components.h"

/*
 * Color structure
 * Represents a 32-bit color value (0x00RRGGBB)
 */
struct Color {
    uint32_t color; // 32-bit integer for color (0x00RRGGBB)
};

/*
 * Basic colors macros (CSS\X11 colors names)
 */
#define COLOR_ALICE_BLUE              ((Color){0x00F0F8FF})
#define COLOR_ANTIQUE_WHITE           ((Color){0x00FAEBD7})
#define COLOR_AQUA                    ((Color){0x0000FFFF})
#define COLOR_AQUAMARINE              ((Color){0x007FFFD4})
#define COLOR_AZURE                   ((Color){0x00F0FFFF})
#define COLOR_BEIGE                   ((Color){0x00F5F5DC})
#define COLOR_BISQUE                  ((Color){0x00FFE4C4})
#define COLOR_BLACK                   ((Color){0x00000000})
#define COLOR_BLANCHED_ALMOND         ((Color){0x00FFEBCD})
#define COLOR_BLUE                    ((Color){0x000000FF})
#define COLOR_BLUE_VIOLET             ((Color){0x008A2BE2})
#define COLOR_BROWN                   ((Color){0x00A52A2A})
#define COLOR_BURLY_WOOD              ((Color){0x00DEB887})
#define COLOR_CADET_BLUE              ((Color){0x005F9EA0})
#define COLOR_CHARTREUSE              ((Color){0x007FFF00})
#define COLOR_CHOCOLATE               ((Color){0x00D2691E})
#define COLOR_CORAL                   ((Color){0x00FF7F50})
#define COLOR_CORNFLOWER_BLUE         ((Color){0x006495ED})
#define COLOR_CORNSILK                ((Color){0x00FFF8DC})
#define COLOR_CRIMSON                 ((Color){0x00DC143C})
#define COLOR_CYAN                    ((Color){0x0000FFFF})
#define COLOR_DARK_BLUE               ((Color){0x0000008B})
#define COLOR_DARK_CYAN               ((Color){0x00008B8B})
#define COLOR_DARK_GOLDEN_ROD         ((Color){0x00B8860B})
#define COLOR_DARK_GRAY               ((Color){0x00A9A9A9})
#define COLOR_DARK_GREY               ((Color){0x00A9A9A9})
#define COLOR_DARK_GREEN              ((Color){0x00006400})
#define COLOR_DARK_KHAKI              ((Color){0x00BDB76B})
#define COLOR_DARK_MAGENTA            ((Color){0x008B008B})
#define COLOR_DARK_OLIVE_GREEN        ((Color){0x00556B2F})
#define COLOR_DARK_ORANGE             ((Color){0x00FF8C00})
#define COLOR_DARK_ORCHID             ((Color){0x009932CC})
#define COLOR_DARK_RED                ((Color){0x008B0000})
#define COLOR_DARK_SALMON             ((Color){0x00E9967A})
#define COLOR_DARK_SEA_GREEN          ((Color){0x008FBC8F})
#define COLOR_DARK_SLATE_BLUE         ((Color){0x00483D8B})
#define COLOR_DARK_SLATE_GRAY         ((Color){0x002F4F4F})
#define COLOR_DARK_SLATE_GREY         ((Color){0x002F4F4F})
#define COLOR_DARK_TURQUOISE          ((Color){0x0000CED1})
#define COLOR_DARK_VIOLET             ((Color){0x009400D3})
#define COLOR_DEEP_PINK               ((Color){0x00FF1493})
#define COLOR_DEEP_SKY_BLUE           ((Color){0x0000BFFF})
#define COLOR_DIM_GRAY                ((Color){0x00696969})
#define COLOR_DIM_GREY                ((Color){0x00696969})
#define COLOR_DODGER_BLUE             ((Color){0x001E90FF})
#define COLOR_FIRE_BRICK              ((Color){0x00B22222})
#define COLOR_FLORAL_WHITE            ((Color){0x00FFFAF0})
#define COLOR_FOREST_GREEN            ((Color){0x00228B22})
#define COLOR_FUCHSIA                 ((Color){0x00FF00FF})
#define COLOR_GAINSBORO               ((Color){0x00DCDCDC})
#define COLOR_GHOST_WHITE             ((Color){0x00F8F8FF})
#define COLOR_GOLD                    ((Color){0x00FFD700})
#define COLOR_GOLDEN_ROD              ((Color){0x00DAA520})
#define COLOR_GRAY                    ((Color){0x00808080})
#define COLOR_GREY                    ((Color){0x00808080})
#define COLOR_GREEN                   ((Color){0x00008000})
#define COLOR_GREEN_YELLOW            ((Color){0x00ADFF2F})
#define COLOR_HONEY_DEW               ((Color){0x00F0FFF0})
#define COLOR_HOT_PINK                ((Color){0x00FF69B4})
#define COLOR_INDIAN_RED              ((Color){0x00CD5C5C})
#define COLOR_INDIGO                  ((Color){0x004B0082})
#define COLOR_IVORY                   ((Color){0x00FFFFF0})
#define COLOR_KHAKI                   ((Color){0x00F0E68C})
#define COLOR_LAVENDER                ((Color){0x00E6E6FA})
#define COLOR_LAVENDER_BLUSH          ((Color){0x00FFF0F5})
#define COLOR_LAWN_GREEN              ((Color){0x007CFC00})
#define COLOR_LEMON_CHIFFON           ((Color){0x00FFFACD})
#define COLOR_LIGHT_BLUE              ((Color){0x00ADD8E6})
#define COLOR_LIGHT_CORAL             ((Color){0x00F08080})
#define COLOR_LIGHT_CYAN              ((Color){0x00E0FFFF})
#define COLOR_LIGHT_GOLDEN_ROD_YELLOW ((Color){0x00FAFAD2})
#define COLOR_LIGHT_GRAY              ((Color){0x00D3D3D3})
#define COLOR_LIGHT_GREY              ((Color){0x00D3D3D3})
#define COLOR_LIGHT_GREEN             ((Color){0x0090EE90})
#define COLOR_LIGHT_PINK              ((Color){0x00FFB6C1})
#define COLOR_LIGHT_SALMON            ((Color){0x00FFA07A})
#define COLOR_LIGHT_SEA_GREEN         ((Color){0x0020B2AA})
#define COLOR_LIGHT_SKY_BLUE          ((Color){0x0087CEFA})
#define COLOR_LIGHT_SLATE_GRAY        ((Color){0x00778899})
#define COLOR_LIGHT_SLATE_GREY        ((Color){0x00778899})
#define COLOR_LIGHT_STEEL_BLUE        ((Color){0x00B0C4DE})
#define COLOR_LIGHT_YELLOW            ((Color){0x00FFFFE0})
#define COLOR_LIME                    ((Color){0x0000FF00})
#define COLOR_LIME_GREEN              ((Color){0x0032CD32})
#define COLOR_LINEN                   ((Color){0x00FAF0E6})
#define COLOR_MAGENTA                 ((Color){0x00FF00FF})
#define COLOR_MAROON                  ((Color){0x00800000})
#define COLOR_MEDIUM_AQUA_MARINE      ((Color){0x0066CDAA})
#define COLOR_MEDIUM_BLUE             ((Color){0x000000CD})
#define COLOR_MEDIUM_ORCHID           ((Color){0x00BA55D3})
#define COLOR_MEDIUM_PURPLE           ((Color){0x009370DB})
#define COLOR_MEDIUM_SEA_GREEN        ((Color){0x003CB371})
#define COLOR_MEDIUM_SLATE_BLUE       ((Color){0x007B68EE})
#define COLOR_MEDIUM_SPRING_GREEN     ((Color){0x0000FA9A})
#define COLOR_MEDIUM_TURQUOISE        ((Color){0x0048D1CC})
#define COLOR_MEDIUM_VIOLET_RED       ((Color){0x00C71585})
#define COLOR_MIDNIGHT_BLUE           ((Color){0x00191970})
#define COLOR_MINT_CREAM              ((Color){0x00F5FFFA})
#define COLOR_MISTY_ROSE              ((Color){0x00FFE4E1})
#define COLOR_MOCCASIN                ((Color){0x00FFE4B5})
#define COLOR_NAVAJO_WHITE            ((Color){0x00FFDEAD})
#define COLOR_NAVY                    ((Color){0x00000080})
#define COLOR_OLD_LACE                ((Color){0x00FDF5E6})
#define COLOR_OLIVE                   ((Color){0x00808000})
#define COLOR_OLIVE_DRAB              ((Color){0x006B8E23})
#define COLOR_ORANGE                  ((Color){0x00FFA500})
#define COLOR_ORANGE_RED              ((Color){0x00FF4500})
#define COLOR_ORCHID                  ((Color){0x00DA70D6})
#define COLOR_PALE_GOLDEN_ROD         ((Color){0x00EEE8AA})
#define COLOR_PALE_GREEN              ((Color){0x0098FB98})
#define COLOR_PALE_TURQUOISE          ((Color){0x00AFEEEE})
#define COLOR_PALE_VIOLET_RED         ((Color){0x00DB7093})
#define COLOR_PAPAYA_WHIP             ((Color){0x00FFEFD5})
#define COLOR_PEACH_PUFF              ((Color){0x00FFDAB9})
#define COLOR_PERU                    ((Color){0x00CD853F})
#define COLOR_PINK                    ((Color){0x00FFC0CB})
#define COLOR_PLUM                    ((Color){0x00DDA0DD})
#define COLOR_POWDER_BLUE             ((Color){0x00B0E0E6})
#define COLOR_PURPLE                  ((Color){0x00800080})
#define COLOR_REBECCA_PURPLE          ((Color){0x00663399})
#define COLOR_RED                     ((Color){0x00FF0000})
#define COLOR_ROSY_BROWN              ((Color){0x00BC8F8F})
#define COLOR_ROYAL_BLUE              ((Color){0x004169E1})
#define COLOR_SADDLE_BROWN            ((Color){0x008B4513})
#define COLOR_SALMON                  ((Color){0x00FA8072})
#define COLOR_SANDY_BROWN             ((Color){0x00F4A460})
#define COLOR_SEA_GREEN               ((Color){0x002E8B57})
#define COLOR_SEA_SHELL               ((Color){0x00FFF5EE})
#define COLOR_SIENNA                  ((Color){0x00A0522D})
#define COLOR_SILVER                  ((Color){0x00C0C0C0})
#define COLOR_SKY_BLUE                ((Color){0x0087CEEB})
#define COLOR_SLATE_BLUE              ((Color){0x006A5ACD})
#define COLOR_SLATE_GRAY              ((Color){0x00708090})
#define COLOR_SLATE_GREY              ((Color){0x00708090})
#define COLOR_SNOW                    ((Color){0x00FFFAFA})
#define COLOR_SPRING_GREEN            ((Color){0x0000FF7F})
#define COLOR_STEEL_BLUE              ((Color){0x004682B4})
#define COLOR_TAN                     ((Color){0x00D2B48C})
#define COLOR_TEAL                    ((Color){0x00008080})
#define COLOR_THISTLE                 ((Color){0x00D8BFD8})
#define COLOR_TOMATO                  ((Color){0x00FF6347})
#define COLOR_TURQUOISE               ((Color){0x0040E0D0})
#define COLOR_VIOLET                  ((Color){0x00EE82EE})
#define COLOR_WHEAT                   ((Color){0x00F5DEB3})
#define COLOR_WHITE                   ((Color){0x00FFFFFF})
#define COLOR_WHITE_SMOKE             ((Color){0x00F5F5F5})
#define COLOR_YELLOW                  ((Color){0x00FFFF00})
#define COLOR_YELLOW_GREEN            ((Color){0x009ACD32})


#define COLOR_NONE                    ((Color){0xF0FFFFFF}) // Use a value unlikely to be a valid color



// -----------------------------------------------------------------------------
//  Color Utilities
// -----------------------------------------------------------------------------
/*
 * Extracts the red component from a 32-bit color.
 */
inline unsigned char get_red(Color color) { return (color.color >> 16) & 0xFF; }

/*
 * Extracts the green component from a 32-bit color.
 */
inline unsigned char get_green(Color color) { return (color.color >> 8) & 0xFF; }

/*
 * Extracts the blue component from a 32-bit color.
 */
inline unsigned char get_blue(Color color) { return color.color & 0xFF; }

/*
 * Checks if a color is set to the 'NONE' value.
 */
inline bool is_none(Color color) { return color.color == COLOR_NONE.color; }

/*
 * Creates a 32-bit RGB color from individual red, green, and blue components.
 */
inline Color create_color_rgb(int r, int g, int b) {
    return (Color){0x00000000 | (uint32_t)((r & 0xFF) << 16) | (uint32_t)((g & 0xFF) << 8) | (uint32_t)(b & 0xFF << 0)};
}

#endif // COLOR_H
