#pragma once
#include <ImGui/imgui.h>
#include <Maths/hVector.h>

namespace ESP
{
    inline void DrawBox(const Vector2& topLeft, const Vector2& bottomRight, ImColor color = ImColor(255, 255, 255, 255), float thickness = 2.0f) {
        ImGui::GetBackgroundDrawList()->AddRect(ImVec2(topLeft.x, topLeft.y), ImVec2(bottomRight.x, bottomRight.y), color, 0.0f, ImDrawFlags_None, thickness);
    }

    inline void DrawLine(const Vector2& from, const Vector2& to, ImColor color = ImColor(255, 255, 255, 255), float thickness = 1.0f) {
        ImGui::GetBackgroundDrawList()->AddLine(ImVec2(from.x, from.y), ImVec2(to.x, to.y), color, thickness);
    }

    inline void DrawHealthBar(const Vector2& topLeft, const Vector2& bottomRight, int health, ImColor color = ImColor(0, 255, 0, 255), float thickness = 2.0f, float barWidth = 7.0f, float offset = 5.0f) {
        float boxHeight = bottomRight.y - topLeft.y;
        float healthHeight = (health / 100.0f) * boxHeight;

        ImVec2 healthTopLeft(topLeft.x - barWidth - offset, bottomRight.y - healthHeight);
        ImVec2 healthBottomRight(topLeft.x - offset, bottomRight.y);

        ImGui::GetBackgroundDrawList()->AddRectFilled(healthTopLeft, healthBottomRight, color, 0.0f, ImDrawFlags_None);
    }

    inline void DrawText(const char* text, const Vector2& position, ImU32 color, bool centered = false)
    {
        ImGuiIO& io = ImGui::GetIO();
        ImFont* font = ImGui::GetFont(); // Get the current font
        ImVec2 textSize = font->CalcTextSizeA(io.FontGlobalScale, FLT_MAX, 0.0f, text); // Get the size of the text

        if (centered)
        {
            ImVec2 textPos(position.x - textSize.x / 2.0f, position.y - textSize.y / 2.0f);
            ImGui::GetBackgroundDrawList()->AddText(textPos, color, text);
        }
        else
        {
            ImGui::GetBackgroundDrawList()->AddText(ImVec2(position.x, position.y), color, text);
        }
    }

}