

#include "CarAgent.hpp"

#include "Circuit.hpp"

// #include "../utils/utils.hpp"
// #include "../utils/collision.hpp"


#include "geronimo/system/math/2d/collisions/collisions.hpp"
#include "geronimo/system/math/rotateVec2.hpp"
#include "geronimo/system/math/constants.hpp"
#include "geronimo/system/math/clamp.hpp"

#include <cmath>


namespace {
	constexpr float k_maxLife = 25.0f;
}

CarAgent::CarAgent()
	:	_angle(0.0f),
		_fitness(0.0f),
		_alive(true),
		_total_updates(0),
		_current_checkpoint(0)
{
	updateSensors();
}

void	CarAgent::update(float step, const Circuit& circuit, const NeuralNetwork& in_NN)
{
	if (std::isnan(_position.x) ||
		std::isnan(_position.y) ||
		std::isnan(_angle))
		_alive = false;

	if (!_alive)
		return;

	//
	// min update

	if (_life > 0.0f) {
		_life -= step;
	}
	if (_life <= 0.0f)
	{
		_life = 0.0f;
		_alive = false;
	}

	// min update
	//
	//

	// _fitness += 0.01f;

	this->updateSensors();
	this->collideSensors( circuit.getWalls() );
	this->collideCheckpoints( circuit.getCheckpoints() );
	this->collideWalls( circuit.getWalls() );

	// return;

	//


	std::vector<float>	input, output;

	input.reserve(in_NN.getTopology().getInputLayerSize());
	output.reserve(in_NN.getTopology().getOutputLayerSize());

	for (t_sensor& sensor : _sensors) {
		input.push_back( sensor.value );
	}

	// if (input.size() != 5)
	//     throw std::invalid_argument( "about to use an invalid number of inputs" );

	in_NN.compute(input, output);

	// if (output.size() != 2)
	//     throw std::invalid_argument( "received invalid number of outputs" );

	float acceleration = output[0] * 2.0f - 1.0f; // [-1..1]
	float direction	= output[1] * 2.0f - 1.0f; // [-1..1]

	if (std::isnan(acceleration)){
		acceleration = 0.0f;
	}
	if (std::isnan(direction)) {
		direction = 0.0f;
	}


	acceleration = gero::math::clamp(acceleration * 2.0f, -0.5f, 1.0f);
	_speed += acceleration;
	_speed = gero::math::clamp(_speed, -3.0f, 5.0f);

	const float steer_range = gero::math::pi/32.0f;
	direction = gero::math::clamp(direction, -steer_range, steer_range);
	_angle += direction * step;


	// speed *= 3.0f;





	const glm::vec2	prev_pos = _position;

	_position.x += (_speed * std::cos(_angle)) * step;
	_position.y += (_speed * std::sin(_angle)) * step;


	_trail.push_back( Line(prev_pos, _position) );

	++_total_updates;
}

void	CarAgent::updateSensors()
{
	// initialise

	// static const float angles[5] = {
	// 	-M_PI/2.0f,
	// 	-M_PI/4.0f,
	// 	0.0f,
	// 	M_PI/4.0f,
	// 	M_PI/2.0f
	// };

	static const std::array<float, 5> angles = {{
		-gero::math::pi/4.0f,
		-gero::math::pi/8.0f,
		0.0f,
		gero::math::pi/8.0f,
		gero::math::pi/4.0f
	}};

	for (std::size_t ii = 0; ii < _sensors.size(); ++ii)
	{
		_sensors[ii].line.posA = glm::vec2();
		_sensors[ii].line.posB.x = 200.0f * cosf(angles[ii]);
		_sensors[ii].line.posB.y = 200.0f * sinf(angles[ii]);

		_sensors[ii].value = 1.0f;
	}

	// translate/rotate

	for (t_sensor& sensor : _sensors)
	{
		sensor.line.posA.x += _position.x;
		sensor.line.posB.x += _position.x;
		sensor.line.posA.y += _position.y;
		sensor.line.posB.y += _position.y;

		sensor.line.posA = gero::math::rotateVec2(sensor.line.posA, _position, _angle);
		sensor.line.posB = gero::math::rotateVec2(sensor.line.posB, _position, _angle);
	}
}

void	CarAgent::collideSensors(const Lines& walls)
{
	for (t_sensor& sensor : _sensors)
	{
		sensor.value = 1.0f;

		for (const Line& wall : walls)
		{
			float v = gero::math::rawCollisionSegmentSegment(sensor.line.posA, sensor.line.posB, wall.posA, wall.posB);

			if (v >= 0.0f && v < 1.0f && sensor.value > v)
				sensor.value = v;
		}
	}
}

void	CarAgent::collideCheckpoints(const Lines& checkpoints)
{
	if (_current_checkpoint < checkpoints.size())
	{
		auto& checkpoint = checkpoints[_current_checkpoint];
		if (gero::math::collisionSegmentCircle(checkpoint.posA, checkpoint.posB, _position, 5.0f))
		{
			_life = k_maxLife;
			++_fitness;
			++_current_checkpoint;
		}
	}

	if (_current_checkpoint >= checkpoints.size())
	{
		// this line reward a faster car once the circuit is completed
		_fitness += (1000.0f / float(_total_updates));

		_alive = false;
	}
}

void	CarAgent::collideWalls(const Lines& walls)
{
	for (const Line& l : walls)
		if (gero::math::collisionSegmentCircle(l.posA, l.posB, _position, 10.0f))
		{
			_alive = false;
			break;
		}
}

void	CarAgent::reset(const Circuit& circuit)
{
	_position = circuit.getStartingPosition();
	_angle = circuit.getStartingAngle();

	_alive = true;
	_fitness = 0;
	_total_updates = 0;
	_trail.clear();
	_life = k_maxLife;
	_current_checkpoint = 0;
}

float	CarAgent::getLife() const
{
	return _life;
}
