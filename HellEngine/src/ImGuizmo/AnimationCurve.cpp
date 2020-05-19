#include "hellpch.h"
#include "AnimationCurve.h"
#include "imgui_internal.h"
#include <algorithm>

namespace ImGuizmo
{
	uint32_t AnimationCurve::s_uniqueId = 100;

	AnimationCurve::AnimationCurve(ImVec2 min, ImVec2 max, uint32_t color, ImCurveEdit::CurveType type)
		: m_min(min)
		, m_max(max)
		, m_curveColor(color)
	{
		m_curveType = type;
		if (m_curveType < ImCurveEdit::CurveLinear) m_curveType = ImCurveEdit::CurveLinear;
		if (m_curveType > ImCurveEdit::CurveSmooth) m_curveType = ImCurveEdit::CurveSmooth;

		m_controlPoints.push_back(m_min);
		m_controlPoints.push_back(m_max);
		m_id = s_uniqueId++;
	}

	size_t AnimationCurve::GetCurveCount()
	{
		return 1;
	}

	ImVec2& AnimationCurve::GetMin()
	{
		return m_min;
	}

	ImVec2& AnimationCurve::GetMax()
	{
		return m_max;
	}

	ImCurveEdit::CurveType AnimationCurve::GetCurveType(size_t curveIndex) const
	{
		return m_curveType;
	}

	size_t AnimationCurve::GetPointCount(size_t curveIndex)
	{
		return m_controlPoints.size();
	}

	uint32_t AnimationCurve::GetCurveColor(size_t curveIndex)
	{
		return m_curveColor;
	}

	ImVec2* AnimationCurve::GetPoints(size_t curveIndex)
	{
		return &m_controlPoints[0];
	}

	int AnimationCurve::EditPoint(size_t curveIndex, int pointIndex, ImVec2 value)
	{
		assert(pointIndex < m_controlPoints.size());

		// Avoid to edit first and last points
		if (pointIndex == 0 || pointIndex == m_controlPoints.size() - 1)
		{
			m_controlPoints[pointIndex] = ImVec2(m_controlPoints[pointIndex].x, ImClamp(value.y, m_min.y, m_max.y));
			return pointIndex;
		}
		ImVec2 prevPnt = m_controlPoints[pointIndex - 1], nextPnt = m_controlPoints[pointIndex + 1];

		m_controlPoints[pointIndex] = Clamped(value, prevPnt, nextPnt);

		return pointIndex;
	}

	void AnimationCurve::AddPoint(size_t curveIndex, ImVec2 value)
	{
		auto it = m_controlPoints.begin();
		while (it != m_controlPoints.end())
		{
			if ((*it).x >= value.x)
			{
				m_controlPoints.insert(it, Clamped(value, m_min, m_max));
				return;
			}
			++it;
		}

		if (it == m_controlPoints.end())
		{
			m_controlPoints.push_back(Clamped(value, m_min, m_max));
		}
	}

	uint32_t AnimationCurve::GetUniqueId() const
	{
		return m_id;
	}

	void AnimationCurve::Show(ImVec2 position, ImVec2 size)
	{
		ImCurveEdit::Edit( *this, position, size, GetUniqueId() );
	}

	float AnimationCurve::Evaluate(float x) const
	{
		float result = 0;

		for (size_t p = 0; p < m_controlPoints.size() - 1; p++)
		{
			const ImVec2 p1 = m_controlPoints[p];
			const ImVec2 p2 = m_controlPoints[p + 1];
			if( x >= p1.x && x <= p2.x )
			{
				float t = (x - p1.x) / (p2.x - p1.x);
				const ImVec2 sp = ImLerp(p1, p2, t);
				if (m_curveType == ImCurveEdit::CurveLinear)
				{
					result = 1.0f - sp.y;
				}
				else
				{
					const float rt = ImCurveEdit::smoothstep(p1.x, p2.x, sp.x);
					result = 1.0f - ImLerp(p1.y, p2.y, rt);
				}
				break;
			}
		}

		return result;
	}

	ImVec2 AnimationCurve::Clamped(ImVec2 value, ImVec2 min, ImVec2 max)
	{
		// Use eps to avoid floating point errors
		float const EPS = 1e-2f;
		float x = value.x;
		if (x < min.x + EPS) x = min.x + EPS;
		if (x > max.x - EPS) x = max.x - EPS;
		if (value.y < m_min.y) value.y = m_min.y;
		if (value.y > m_max.y) value.y = m_max.y;

		return ImVec2(x, value.y);
	}

}