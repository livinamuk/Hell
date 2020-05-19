#pragma once
#include <stdint.h>
#include "imgui.h"
#include <set>

struct ImRect;

namespace ImCurveEdit
{
	float smoothstep(float edge0, float edge1, float x);

    enum CurveType
    {
        CurveNone,
        CurveDiscrete,
        CurveLinear,
        CurveSmooth,
        CurveBezier,
    };

    struct EditPoint
    {
        int curveIndex;
        int pointIndex;
        bool operator <(const EditPoint& other) const
        {
            if (curveIndex < other.curveIndex)
                return true;
            if (curveIndex > other.curveIndex)
                return false;

            if (pointIndex < other.pointIndex)
                return true;
            return false;
        }
    };

    struct Delegate
    {
        bool focused = false;
        virtual size_t GetCurveCount() = 0;
        virtual bool IsVisible(size_t curveIndex) { return true; }
        virtual CurveType GetCurveType(size_t curveIndex) const { return CurveLinear; }
        virtual ImVec2& GetMin() = 0;
        virtual ImVec2& GetMax() = 0;
		virtual float Evaluate(float x) const = 0;
        virtual size_t GetPointCount(size_t curveIndex) = 0;
        virtual uint32_t GetCurveColor(size_t curveIndex) = 0;
        virtual ImVec2* GetPoints(size_t curveIndex) = 0;
        virtual int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value) = 0;
        virtual void AddPoint(size_t curveIndex, ImVec2 value) = 0;
        virtual unsigned int GetBackgroundColor() { return 0xFF202020; }
        // handle undo/redo thru this functions
        virtual void BeginEdit(int /*index*/) {}
        virtual void EndEdit() {}

		bool& GetSelectionQuad() { return m_selectingQuad; }
		ImVec2& GetQuadSelection() { return m_quadSelection; }
		int& GetOverCurve() { return m_overCurve; }
		int& GetMovingCurve() { return m_movingCurve; }
		bool& GetScrollingV() { return m_scrollingV; }
		std::set<ImCurveEdit::EditPoint>& GetSelection() { return m_selection; }
		bool& GetOverSelectedPoint() { return m_overSelectedPoint; }
	private:

		bool m_selectingQuad = false;
		ImVec2 m_quadSelection;
		int m_overCurve = -1;
		int m_movingCurve = -1;
		bool m_scrollingV = false;
		std::set<ImCurveEdit::EditPoint> m_selection;
		bool m_overSelectedPoint = false;
    };

    int Edit(Delegate &delegate, const ImVec2& pos, const ImVec2& size, unsigned int id, const ImRect *clippingRect = NULL, ImVector<EditPoint> *selectedPoints = NULL);
}
