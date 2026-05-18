#include "gui.h"

#include <assert.h>

GLFWwindow          *gui_window = NULL;
struct ImGuiContext *gui_ctx    = NULL;
struct ImGuiIO      *gui_io     = NULL;

void gui_init(GLFWwindow *win) {
    gui_window = win;

    gui_ctx = igCreateContext(NULL);
    igSetCurrentContext(gui_ctx);

    const char *glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    igStyleColorsDark(NULL);

    ImGuiStyle *style = igGetStyle();
    ImGuiStyle_ScaleAllSizes(style, 2.0f);
    style->FontScaleDpi = 2.0f;
}

void gui_terminate(void) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(gui_ctx);
    gui_ctx = NULL;
}

static void piece_screen_rect(const ImVec2 origin, float cs, float pad, uint_fast16_t px, uint_fast16_t py, int pw,
                              int ph, ImVec2 *out_p0, ImVec2 *out_p1) {
    float cellsz = cs + pad;
    out_p0->x    = origin.x + (float)px * cellsz;
    out_p0->y    = origin.y + (float)py * cellsz;
    out_p1->x    = out_p0->x + (float)pw * cs + (float)(pw - 1) * pad;
    out_p1->y    = out_p0->y + (float)ph * cs + (float)(ph - 1) * pad;
}

static void get_piece_cell_size(piece_type_t type, int *out_w, int *out_h) {
    switch (type) {
        case PIECE_BIG_SQUARE:
            *out_w = 2;
            *out_h = 2;
            break;
        case PIECE_SMALL_BLOCK:
            *out_w = 1;
            *out_h = 1;
            break;
        case PIECE_VERTICAL_I:
            *out_w = 1;
            *out_h = 2;
            break;
        case PIECE_HORIZONTAL_I:
            *out_w = 2;
            *out_h = 1;
            break;
        default:
            *out_w = 1;
            *out_h = 1;
            break;
    }
}

static bool is_same_piece(const board_t *board, uint_fast16_t x1, uint_fast16_t y1, uint_fast16_t x2,
                          uint_fast16_t y2) {
    piece_type_t t1 = get_piece_at(board, x1, y1);
    piece_type_t t2 = get_piece_at(board, x2, y2);
    if (t1 != t2 || t1 == PIECE_NONE)
        return false;
    uint_fast16_t px1, py1, px2, py2;
    board_get_piece_position(board, t1, x1, y1, &px1, &py1);
    board_get_piece_position(board, t2, x2, y2, &px2, &py2);
    return px1 == px2 && py1 == py2;
}

static void draw_piece(const ImVec2 origin, float cell_size, float padding, ImDrawList *dl, piece_type_t type,
                       uint_fast16_t px, uint_fast16_t py, ImU32 fill_col, ImU32 border_col) {
    int pw, ph;
    get_piece_cell_size(type, &pw, &ph);

    ImVec2 p0, p1;
    piece_screen_rect(origin, cell_size, padding, px, py, pw, ph, &p0, &p1);

    ImDrawList_AddRectFilled(dl, p0, p1, fill_col, 0, 0);
    ImDrawList_AddRect(dl, p0, p1, border_col, 0, 1.0f, ImDrawFlags_None);
}

static void draw_board_panel(board_t *board) {
    if (!igBegin("Board", NULL, ImGuiWindowFlags_None)) {
        igEnd();
        return;
    }

    const float cell_size = 160.0f;
    const float padding   = 8.0f;
    ImDrawList *dl        = igGetWindowDrawList();
    ImVec2      origin    = igGetCursorScreenPos();

    ImU32 colors[4];
    colors[0] = igColorConvertFloat4ToU32((ImVec4){200.0f / 255, 60.0f / 255, 60.0f / 255, 1});
    colors[1] = igColorConvertFloat4ToU32((ImVec4){60.0f / 255, 180.0f / 255, 60.0f / 255, 1});
    colors[2] = igColorConvertFloat4ToU32((ImVec4){60.0f / 255, 100.0f / 255, 220.0f / 255, 1});
    colors[3] = igColorConvertFloat4ToU32((ImVec4){220.0f / 255, 180.0f / 255, 40.0f / 255, 1});

    ImU32 dim    = igColorConvertFloat4ToU32((ImVec4){30.0f / 255, 30.0f / 255, 30.0f / 255, 1});
    ImU32 border = igColorConvertFloat4ToU32((ImVec4){80.0f / 255, 80.0f / 255, 80.0f / 255, 1});

    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 4; x++) {
            float  cx = origin.x + (float)x * (cell_size + padding);
            float  cy = origin.y + (float)y * (cell_size + padding);
            ImVec2 p0 = {cx, cy};
            ImVec2 p1 = {cx + cell_size, cy + cell_size};
            ImDrawList_AddRectFilled(dl, p0, p1, dim, 0, 0);
            ImDrawList_AddRect(dl, p0, p1, border, 0, 1.0f, ImDrawFlags_None);
        }
    }

    {
        uint_fast16_t bx = get_x_position_from_big_square(board->big_piece);
        uint_fast16_t by = get_y_position_from_big_square(board->big_piece);
        draw_piece(origin, cell_size, padding, dl, PIECE_BIG_SQUARE, bx, by, colors[0], border);
    }

    for (uint_fast8_t i = 0; i < 4; i++) {
        uint_fast16_t sx = get_x_position_from_small_block(board->small_blocks, i);
        uint_fast16_t sy = get_y_position_from_small_block(board->small_blocks, i);
        draw_piece(origin, cell_size, padding, dl, PIECE_SMALL_BLOCK, sx, sy, colors[1], border);
    }

    for (uint_fast8_t i = 0; i < board->num_vertical; i++) {
        uint_fast16_t vx = get_x_position_from_vertical_i(board->vertical_blocks, i);
        uint_fast16_t vy = get_y_position_from_vertical_i(board->vertical_blocks, i);
        draw_piece(origin, cell_size, padding, dl, PIECE_VERTICAL_I, vx, vy, colors[2], border);
    }

    for (uint_fast8_t i = 0; i < board->num_horizontal; i++) {
        uint_fast16_t hx = get_x_position_from_horizontal_i(board->horizontal_blocks, i);
        uint_fast16_t hy = get_y_position_from_horizontal_i(board->horizontal_blocks, i);
        draw_piece(origin, cell_size, padding, dl, PIECE_HORIZONTAL_I, hx, hy, colors[3], border);
    }

    ImVec2 mouse = igGetMousePos();
    int    gx    = (int)((mouse.x - origin.x) / (cell_size + padding));
    int    gy    = (int)((mouse.y - origin.y) / (cell_size + padding));

    static bool          sel_active;
    static uint_fast16_t sel_x, sel_y;

    if (gx >= 0 && gx < 4 && gy >= 0 && gy < 5 && mouse.x >= origin.x && mouse.y >= origin.y) {
        piece_type_t piece = get_piece_at(board, (uint_fast16_t)gx, (uint_fast16_t)gy);

        if (piece != PIECE_NONE) {
            uint_fast16_t hx, hy;
            board_get_piece_position(board, piece, (uint_fast16_t)gx, (uint_fast16_t)gy, &hx, &hy);

            int hw, hh;
            get_piece_cell_size(piece, &hw, &hh);

            ImVec2 hp0, hp1;
            piece_screen_rect(origin, cell_size, padding, hx, hy, hw, hh, &hp0, &hp1);

            bool  movable = can_piece_move(board, (uint_fast16_t)gx, (uint_fast16_t)gy);
            ImU32 hl_col  = igColorConvertFloat4ToU32(movable ? (ImVec4){0, 1, 0, 1} : (ImVec4){1, 0, 0, 1});
            ImDrawList_AddRect(dl, hp0, hp1, hl_col, 0, 2.0f, ImDrawFlags_None);

            if (igIsMouseClicked_Bool(ImGuiMouseButton_Left, false)) {
                if (sel_active && is_same_piece(board, sel_x, sel_y, (uint_fast16_t)gx, (uint_fast16_t)gy)) {
                    sel_active = false;
                } else if (movable) {
                    int count = board_count_legal_moves(board, (uint_fast16_t)gx, (uint_fast16_t)gy);
                    if (count == 1) {
                        board_move_piece_to_empty_cell(board, (uint_fast16_t)gx, (uint_fast16_t)gy);
                        sel_active = false;
                    } else {
                        sel_x      = (uint_fast16_t)gx;
                        sel_y      = (uint_fast16_t)gy;
                        sel_active = true;
                    }
                }
            }
        } else if (sel_active && igIsMouseClicked_Bool(ImGuiMouseButton_Left, false)) {
            piece_type_t      ptype = get_piece_at(board, sel_x, sel_y);
            does_piece_fit_fn fn    = get_does_piece_fit_fn(ptype);
            assert(fn != NULL);

            uint_fast16_t px, py;
            board_get_piece_position(board, ptype, sel_x, sel_y, &px, &py);

            int max_x, max_y;
            switch (ptype) {
                case PIECE_BIG_SQUARE:
                    max_x = 2;
                    max_y = 3;
                    break;
                case PIECE_SMALL_BLOCK:
                    max_x = 3;
                    max_y = 4;
                    break;
                case PIECE_VERTICAL_I:
                    max_x = 3;
                    max_y = 3;
                    break;
                case PIECE_HORIZONTAL_I:
                    max_x = 2;
                    max_y = 4;
                    break;
                default:
                    max_x = 3;
                    max_y = 4;
                    break;
            }

            board_t copy = board_clone(board);
            board_remove_piece(&copy, ptype, sel_x, sel_y);

            int           offsets[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
            float         best_dist     = 1e9f;
            uint_fast16_t best_nx = 0, best_ny = 0;

            for (int d = 0; d < 4; d++) {
                int nx = (int)px + offsets[d][0];
                int ny = (int)py + offsets[d][1];
                if (nx < 0 || ny < 0 || nx > max_x || ny > max_y)
                    continue;
                if (!fn(&copy, (uint_fast16_t)nx, (uint_fast16_t)ny))
                    continue;

                float cx = 0, cy = 0;
                switch (ptype) {
                    case PIECE_SMALL_BLOCK:
                        cx = (float)nx + 0.5f;
                        cy = (float)ny + 0.5f;
                        break;
                    case PIECE_VERTICAL_I:
                        cx = (float)nx + 0.5f;
                        cy = (float)ny + 1.0f;
                        break;
                    case PIECE_HORIZONTAL_I:
                        cx = (float)nx + 1.0f;
                        cy = (float)ny + 0.5f;
                        break;
                    case PIECE_BIG_SQUARE:
                        cx = (float)nx + 1.0f;
                        cy = (float)ny + 1.0f;
                        break;
                    default: break;
                }
                float fx   = origin.x + cx * (cell_size + padding);
                float fy   = origin.y + cy * (cell_size + padding);
                float dx   = fx - mouse.x;
                float dy   = fy - mouse.y;
                float dist = dx * dx + dy * dy;
                if (dist < best_dist) {
                    best_dist = dist;
                    best_nx   = (uint_fast16_t)nx;
                    best_ny   = (uint_fast16_t)ny;
                }
            }

            board_move_piece_to(board, ptype, sel_x, sel_y, best_nx, best_ny);
            sel_active = false;
        }
    }

    if (sel_active) {
        piece_type_t stype = get_piece_at(board, sel_x, sel_y);
        if (stype != PIECE_NONE) {
            uint_fast16_t spx, spy;
            board_get_piece_position(board, stype, sel_x, sel_y, &spx, &spy);
            int sw, sh;
            get_piece_cell_size(stype, &sw, &sh);

            ImVec2 sp0, sp1;
            piece_screen_rect(origin, cell_size, padding, spx, spy, sw, sh, &sp0, &sp1);
            ImU32 gold = igColorConvertFloat4ToU32((ImVec4){1, 0.8f, 0, 1});
            ImDrawList_AddRect(dl, sp0, sp1, gold, 0, 2.0f, ImDrawFlags_None);
        }
    }

    {
        int    goal_x = 1, goal_y = 3;
        ImVec2 gp0, gp1;
        piece_screen_rect(origin, cell_size, padding, goal_x, goal_y, 2, 2, &gp0, &gp1);

        float  gap    = 12.0f;
        float  thick  = 10.0f;
        ImU32  yellow = igColorConvertFloat4ToU32((ImVec4){1, 0.8f, 0, 1});
        ImVec2 ip0    = {gp0.x, gp1.y + gap};
        ImVec2 ip1    = {gp1.x, gp1.y + gap + thick};
        ImDrawList_AddRectFilled(dl, ip0, ip1, yellow, 0, 0);
    }

    igEnd();
}

static void draw_info_panel(const board_t *board) {
    if (!igBegin("Board Info", NULL, ImGuiWindowFlags_None)) {
        igEnd();
        return;
    }

    igText("Valid: %s", is_board_valid(board) ? "YES" : "NO");
    igText("Solved: %s", is_board_solved(board) ? "YES" : "NO");
    igSeparator();

    igText("Big square (2x2):");
    {
        uint_fast16_t bx = get_x_position_from_big_square(board->big_piece);
        uint_fast16_t by = get_y_position_from_big_square(board->big_piece);
        igText("  position: (%d, %d)", bx, by);
    }

    igSeparator();
    igText("Small blocks (1x1): 4");
    for (uint_fast8_t i = 0; i < 4; i++) {
        uint_fast16_t sx = get_x_position_from_small_block(board->small_blocks, i);
        uint_fast16_t sy = get_y_position_from_small_block(board->small_blocks, i);
        igText("  [%d] (%d, %d)", i, sx, sy);
    }

    igSeparator();
    igText("Vertical I (2x1): %d", board->num_vertical);
    for (uint_fast8_t i = 0; i < board->num_vertical; i++) {
        uint_fast16_t vx = get_x_position_from_vertical_i(board->vertical_blocks, i);
        uint_fast16_t vy = get_y_position_from_vertical_i(board->vertical_blocks, i);
        igText("  [%d] (%d, %d)", i, vx, vy);
    }

    igSeparator();
    igText("Horizontal I (1x2): %d", board->num_horizontal);
    for (uint_fast8_t i = 0; i < board->num_horizontal; i++) {
        uint_fast16_t hx = get_x_position_from_horizontal_i(board->horizontal_blocks, i);
        uint_fast16_t hy = get_y_position_from_horizontal_i(board->horizontal_blocks, i);
        igText("  [%d] (%d, %d)", i, hx, hy);
    }

    igSeparator();
    {
        igText("Empty cells:");
        int found = 0;
        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 5; y++) {
                if (is_position_free(board, (uint_fast16_t)x, (uint_fast16_t)y)) {
                    igText("  (%d, %d)", x, y);
                    found++;
                }
            }
        }
        igText("  total: %d", found);
    }

    igEnd();
}

static void draw_raw_panel(const board_t *board) {
    if (!igBegin("Raw Data", NULL, ImGuiWindowFlags_None)) {
        igEnd();
        return;
    }

    igText("big_piece: %hhu", board->big_piece);
    igSeparator();
    igText("small_blocks:  %llu", (unsigned long long)board->small_blocks);
    igSeparator();
    igText("vertical_blocks:  %llu", (unsigned long long)board->vertical_blocks);
    igText("num_vertical: %hhu", board->num_vertical);
    igSeparator();
    igText("horizontal_blocks:  %llu", (unsigned long long)board->horizontal_blocks);
    igText("num_horizontal: %hhu", board->num_horizontal);

    igEnd();
}

void gui_render(board_t *board) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    draw_board_panel(board);
    draw_info_panel(board);
    draw_raw_panel(board);

    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}
