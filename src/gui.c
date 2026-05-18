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
}

void gui_terminate(void) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(gui_ctx);
    gui_ctx = NULL;
}

static void draw_board_panel(board_t *board) {
    if (!igBegin("Board", NULL, ImGuiWindowFlags_None)) {
        igEnd();
        return;
    }

    const float cell_size = 40.0f;
    const float padding   = 2.0f;
    ImDrawList *dl        = igGetWindowDrawList();
    ImVec2      origin    = igGetCursorScreenPos();

    ImU32 colors[4];
    colors[0] = igColorConvertFloat4ToU32((ImVec4){200.0f / 255, 60.0f / 255, 60.0f / 255, 1});
    colors[1] = igColorConvertFloat4ToU32((ImVec4){60.0f / 255, 180.0f / 255, 60.0f / 255, 1});
    colors[2] = igColorConvertFloat4ToU32((ImVec4){60.0f / 255, 100.0f / 255, 220.0f / 255, 1});
    colors[3] = igColorConvertFloat4ToU32((ImVec4){220.0f / 255, 180.0f / 255, 40.0f / 255, 1});

    const char *labels[4] = {"2x2", "1x1", " |", "--"};

    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 4; x++) {
            float  cx = origin.x + (float)x * (cell_size + padding);
            float  cy = origin.y + (float)y * (cell_size + padding);
            ImVec2 p0 = {cx, cy};
            ImVec2 p1 = {cx + cell_size, cy + cell_size};

            piece_type_t piece = get_piece_at(board, (uint_fast16_t)x, (uint_fast16_t)y);
            ImU32        col   = igColorConvertFloat4ToU32((ImVec4){30.0f / 255, 30.0f / 255, 30.0f / 255, 1});
            if (piece >= 0)
                col = colors[(int)piece];

            ImDrawList_AddRectFilled(dl, p0, p1, col, 0, 0);
            ImDrawList_AddRect(dl, p0, p1,
                               igColorConvertFloat4ToU32((ImVec4){80.0f / 255, 80.0f / 255, 80.0f / 255, 1}), 0, 1.0f,
                               ImDrawFlags_None);

            if (piece >= 0) {
                ImVec2 tp = {cx + cell_size / 2 - 8, cy + cell_size / 2 - 6};
                igSetCursorScreenPos(tp);
                igTextColored((ImVec4){1, 1, 1, 1}, "%s", labels[(int)piece]);
            }
        }
    }

    ImVec2 mouse = igGetMousePos();
    int    gx    = (int)((mouse.x - origin.x) / (cell_size + padding));
    int    gy    = (int)((mouse.y - origin.y) / (cell_size + padding));

    static bool          sel_active;
    static uint_fast16_t sel_x, sel_y;

    if (sel_active) {
        float  cellsz = cell_size + padding;
        float  cx     = origin.x + (float)sel_x * cellsz;
        float  cy     = origin.y + (float)sel_y * cellsz;
        ImVec2 p0     = {cx, cy};
        ImVec2 p1     = {cx + cell_size, cy + cell_size};
        ImU32  gold   = igColorConvertFloat4ToU32((ImVec4){1, 0.8f, 0, 1});
        ImDrawList_AddRect(dl, p0, p1, gold, 0, 2.0f, ImDrawFlags_None);
    }

    if (gx >= 0 && gx < 4 && gy >= 0 && gy < 5 && mouse.x >= origin.x && mouse.y >= origin.y) {
        piece_type_t piece = get_piece_at(board, (uint_fast16_t)gx, (uint_fast16_t)gy);

        if (piece != PIECE_NONE) {
            bool   movable = can_piece_move(board, (uint_fast16_t)gx, (uint_fast16_t)gy);
            ImU32  border  = igColorConvertFloat4ToU32(movable ? (ImVec4){0, 1, 0, 1} : (ImVec4){1, 0, 0, 1});
            float  cellsz  = cell_size + padding;
            float  cx      = origin.x + (float)gx * cellsz;
            float  cy      = origin.y + (float)gy * cellsz;
            ImVec2 p0      = {cx, cy};
            ImVec2 p1      = {cx + cell_size, cy + cell_size};
            ImDrawList_AddRect(dl, p0, p1, border, 0, 2.0f, ImDrawFlags_None);

            if (igIsMouseClicked_Bool(ImGuiMouseButton_Left, false)) {
                if (sel_active && (uint_fast16_t)gx == sel_x && (uint_fast16_t)gy == sel_y) {
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
