

#include "Circuit.hpp"


#include "geronimo/system/math/lerp.hpp"
#include "geronimo/system/math/angles.hpp"
#include "geronimo/system/math/constants.hpp"
#include "geronimo/system/math/2d/collisions/collisions.hpp"

#include <fstream>
#include <sstream>
#include <cmath>


Circuit::Circuit()
	:	m_valid(false)
{}

Circuit::~Circuit()
{}

bool Circuit::loadMap(const std::string& filename)
{
	// D_MYLOG("filename=" << filename);

	// open

	std::ifstream ifs;
	ifs.open(filename, std::ifstream::in);

	if (!ifs.is_open())
	{
		// D_MYLOG("fail to open");
		return false;
	}

	// loop

	std::string line;
	std::string label;

	struct rawCheckpoint
	{
		glm::vec2 pos;
		float size;
	};
	std::vector<rawCheckpoint> rawCheckpoints;

	glm::vec2 lastCenter = {0, 0};
	float lastAngle = 0.0f;

	while (ifs.good())
	{
		std::getline(ifs, line);

		if (line.empty())
		{
			// D_MYLOG("[empty line]");
			continue;
		}

		// D_MYLOG("line=" << line);

		std::stringstream sstr(line);
		sstr >> label;

		//
		// checkpoints

		// if (label == "checkpoint")
		// {
		// 	// D_MYLOG("processing checkpoint");

		// 	std::vector<float> vals;
		// 	float val;
		// 	while (sstr >> val)
		// 	{
		// 		if (std::isnan(val))
		// 		{
		// 			// D_MYLOG("invalid value -> Not a Number");
		// 			return false;
		// 		}

		// 		vals.push_back(val);
		// 	}

		// 	if (vals.size() != 4)
		// 	{
		// 		// D_MYLOG("invalid number of values");
		// 		return false;
		// 	}

		// 	// D_MYLOG("processed checkpoint");
		// 	// D_MYLOG("vals: " << vals[0] << "," << vals[1] << "  " << vals[2] << "," << vals[3]);

		// 	glm::vec2 p1(vals[0],vals[1]);
		// 	glm::vec2 p2(vals[2],vals[3]);
		// 	m_checkpoints.push_back( Line(p1, p2) );
		// }
		// else
		if (label == "checkpoint-angle")
		{
			std::vector<float> vals;
			float val;
			while (sstr >> val)
			{
				if (std::isnan(val))
				{
					// D_MYLOG("invalid value -> Not a Number");
					return false;
				}

				vals.push_back(val);
			}

			if (vals.size() != 3)
			{
				// D_MYLOG("invalid number of values");
				return false;
			}

			const glm::vec2 center = {lastCenter.x + vals[0], lastCenter.y + vals[1]};
			const float width = vals[2];
			// const float angle = lastAngle + glm::radians(vals[3]);

			lastCenter = center;

			rawCheckpoints.push_back({ center, width });
			continue;


			const glm::vec2 diff = lastCenter - center;

			float angle = 0.0f;

			if (!m_checkpoints.empty()) {
				angle = gero::math::getAngle(diff.y, -diff.x); // + gero::math::hpi;
			}

			lastCenter = center;
			// lastAngle = angle;

			// center
			const glm::vec2 p1 = gero::math::rotate2d(center + glm::vec2(-width * 0.5f,0), center, angle);
			const glm::vec2 p2 = gero::math::rotate2d(center + glm::vec2(+width * 0.5f,0), center, angle);


			// glm::vec2 p1(vals[0],vals[1]);
			// glm::vec2 p2(vals[2],vals[3]);
			// if (m_checkpoints.empty()) {
				m_checkpoints.push_back( Line(p1, p2) );
			// } else {
			// 	const Line& lastLine = m_checkpoints.back();
			// 	m_checkpoints.push_back( Line(lastLine.posA + p1, lastLine.posB + p2) );
			// }
		}

		// checkpoints
		//

		else
		{
			// D_MYLOG("unidentified line");
		}
	}

	for (std::size_t ii = 0; ii < rawCheckpoints.size(); ++ii)
	{
		const auto& tmpVal = rawCheckpoints[ii];

		float angle = 0.0f;
		float total = 0.0f;
		if (ii > 0) {
			const glm::vec2 diff = tmpVal.pos - rawCheckpoints[ii - 1].pos;
			angle += gero::math::getAngle(-diff.y, +diff.x);
			total += 1.0f;
		}

		if (ii + 1 < rawCheckpoints.size()) {
			const glm::vec2 diff = rawCheckpoints[ii + 1].pos - tmpVal.pos;
			angle += gero::math::getAngle(-diff.y, +diff.x);
			total += 1.0f;
		}

		if (total > 0.0f) {
			angle /= total;
		}

		const glm::vec2 p1 = gero::math::rotate2d(tmpVal.pos + glm::vec2(-tmpVal.size * 0.5f,0), tmpVal.pos, angle);
		const glm::vec2 p2 = gero::math::rotate2d(tmpVal.pos + glm::vec2(+tmpVal.size * 0.5f,0), tmpVal.pos, angle);

		m_checkpoints.push_back( Line(p1, p2) );

		if (m_checkpoints.size() >= 2)
		{
			const auto& pointA = m_checkpoints[m_checkpoints.size() - 2];
			auto& pointB = m_checkpoints[m_checkpoints.size() - 1];

			if (gero::math::collisionSegmentSegment(pointA.posA, pointB.posA, pointA.posB, pointB.posB)) {
				std::swap(pointB.posA, pointB.posB);
			}
		}

	}


	// close

	ifs.close();

	// final check

	// TODO : more complex check required here
	// -> need to use m_valid
	if (m_checkpoints.size() < 3)
		return false;

	// process walls

	for (unsigned int i = 1; i < m_checkpoints.size(); ++i)
	{
		const Line& prev = m_checkpoints[i - 1];
		const Line& curr = m_checkpoints[i];

		if (gero::math::collisionSegmentSegment(prev.posA, curr.posA, prev.posB, curr.posB)) {

			m_walls.push_back( Line(prev.posA, curr.posB) );
			m_walls.push_back( Line(prev.posB, curr.posA) );

		} else {

			m_walls.push_back( Line(prev.posA, curr.posA) );
			m_walls.push_back( Line(prev.posB, curr.posB) );

		}
	}
	// m_walls.push_back( Line(m_checkpoints.front().posA, m_checkpoints.back().posA) );
	// m_walls.push_back( Line(m_checkpoints.front().posB, m_checkpoints.back().posB) );

	// m_walls.push_back( Line(m_checkpoints.front().posA, m_checkpoints.front().posB) );
	// m_walls.push_back( Line(m_checkpoints.back().posA, m_checkpoints.back().posB) );

	// expand the walls (fix collisions bugs)

	for (unsigned int i = 0; i < m_walls.size(); ++i)
	{
		m_walls[i].posA = gero::math::lerp(m_walls[i].posA, m_walls[i].posB, -0.01f);
		m_walls[i].posB = gero::math::lerp(m_walls[i].posA, m_walls[i].posB, 1.01f);
	}

	// duplicate the checkpoints

	Lines	tmp_checkpoints;
	tmp_checkpoints.push_back(m_checkpoints[0]);
	for (unsigned int i = 1; i < m_checkpoints.size(); ++i)
	{
		const Line& l1 = m_checkpoints[i - 1];
		const Line& l2 = m_checkpoints[i];

		Line	tmp_line;
		for (float f = 0.25f; f < 1.0f; f += 0.25f)
		{
			tmp_line.posA = gero::math::lerp(l1.posA, l2.posA, f);
			tmp_line.posB = gero::math::lerp(l1.posB, l2.posB, f);
			tmp_checkpoints.push_back(tmp_line);
		}

		tmp_checkpoints.push_back(l2);
	}
	// {
	// 	const Line& l1 = m_checkpoints.front();
	// 	const Line& l2 = m_checkpoints.back();

	// 	Line	tmp_line;
	// 	tmp_line.posA = lerp(l1.posA, l2.posA, 0.5f);
	// 	tmp_line.posB = lerp(l1.posB, l2.posB, 0.5f);
	// 	tmp_checkpoints.push_back(tmp_line);

	// 	tmp_checkpoints.push_back(l2);
	// }
	m_checkpoints = tmp_checkpoints;

	// print checkpoints

	// D_MYLOG("print checkpoints");
	// for (unsigned int i = 0; i < m_checkpoints.size(); ++i)
	// {
	// 	const Line& l = m_checkpoints[i];
	// 	D_MYLOG("[" << i << "]: " << l.posA.x << "," << l.posA.y << "  " << l.posB.x << "," << l.posB.y);
	// }



	// print walls

	// D_MYLOG("print walls");
	// for (unsigned int i = 0; i < m_walls.size(); ++i)
	// {
	// 	const Line& l = m_walls[i];
	// 	D_MYLOG("[" << i << "]: " << l.posA.x << "," << l.posA.y << "  " << l.posB.x << "," << l.posB.y);
	// }

	// process starting position

	{
		Line& l1 = m_checkpoints.front();
		m_start_position.x = l1.posA.x + (l1.posB.x - l1.posA.x) * 0.5f;
		m_start_position.y = l1.posA.y + (l1.posB.y - l1.posA.y) * 0.5f;

		glm::vec2 pos;
		Line& l2 = m_checkpoints[1];
		pos.x = l2.posA.x + (l2.posB.x - l2.posA.x) * 0.5f;
		pos.y = l2.posA.y + (l2.posB.y - l2.posA.y) * 0.5f;

		// D_MYLOG("m_start_position.x=" << m_start_position.x);
		// D_MYLOG("m_start_position.y=" << m_start_position.y);
		// D_MYLOG("pos.x=" << pos.x);
		// D_MYLOG("pos.y=" << pos.y);

		m_start_angle = atan2f(pos.y - m_start_position.y, pos.x - m_start_position.x);

		// m_start_position.x += 10.0f * cosf(m_start_angle);
		// m_start_position.y += 10.0f * sinf(m_start_angle);

		// D_MYLOG("m_start_angle=" << m_start_angle);
	}

	{
		Line& l1 = m_checkpoints.back();
		m_stop_position.x = l1.posA.x + (l1.posB.x - l1.posA.x) * 0.5f;
		m_stop_position.y = l1.posA.y + (l1.posB.y - l1.posA.y) * 0.5f;

		glm::vec2 pos;
		Line& l2 = m_checkpoints[m_checkpoints.size() - 2];
		pos.x = l2.posA.x + (l2.posB.x - l2.posA.x) * 0.5f;
		pos.y = l2.posA.y + (l2.posB.y - l2.posA.y) * 0.5f;

		// D_MYLOG("m_stop_position.x=" << m_stop_position.x);
		// D_MYLOG("m_stop_position.y=" << m_stop_position.y);
		// D_MYLOG("pos.x=" << pos.x);
		// D_MYLOG("pos.y=" << pos.y);

		m_stop_angle = atan2f(pos.y - m_stop_position.y, pos.x - m_stop_position.x);

		// m_stop_position.x += 10.0f * cosf(m_stop_angle);
		// m_stop_position.y += 10.0f * sinf(m_stop_angle);

		// D_MYLOG("m_stop_angle=" << m_stop_angle);
	}

	return true;
}

