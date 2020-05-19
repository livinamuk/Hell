#pragma once

#include "ImCurveEdit.h"
#include <vector>

namespace ImGuizmo
{

	struct AnimationCurve : public ImCurveEdit::Delegate
	{
	public:
		AnimationCurve(ImVec2 min = ImVec2(0, 0), ImVec2 max = ImVec2(1, 1), uint32_t color = uint32_t(0xFF0000ff), ImCurveEdit::CurveType type = ImCurveEdit::CurveLinear);

		size_t GetCurveCount() override;

		ImVec2& GetMin() override;
		ImVec2& GetMax() override;

		ImCurveEdit::CurveType GetCurveType(size_t curveIndex) const override;
		size_t GetPointCount(size_t curveIndex) override;
		uint32_t GetCurveColor(size_t curveIndex) override;

		ImVec2* GetPoints(size_t curveIndex) override;
		int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value) override;
		void AddPoint(size_t curveIndex, ImVec2 value) override;

		uint32_t GetUniqueId() const;

		void Show( ImVec2 position, ImVec2 size );
		float Evaluate(float x) const override;

	private:

		ImVec2 Clamped(ImVec2 value, ImVec2 min, ImVec2 max);

	private:
		ImVec2 m_min{ 0, 0 };
		ImVec2 m_max{ 1, 1 };
		uint32_t m_curveColor{0xFF0000FF};
		ImCurveEdit::CurveType m_curveType{ImCurveEdit::CurveLinear};

		std::vector<ImVec2> m_controlPoints;
		uint32_t m_id;

		static uint32_t s_uniqueId;
	};

}