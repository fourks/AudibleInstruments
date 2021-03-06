#include "AudibleInstruments.hpp"
#include <string.h>


struct Veils : Module {
	enum ParamIds {
		GAIN1_PARAM,
		GAIN2_PARAM,
		GAIN3_PARAM,
		GAIN4_PARAM,
		RESPONSE1_PARAM,
		RESPONSE2_PARAM,
		RESPONSE3_PARAM,
		RESPONSE4_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		CV1_INPUT,
		CV2_INPUT,
		CV3_INPUT,
		CV4_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		NUM_OUTPUTS
	};

	float lights[4] = {};

	Veils() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
	void step() override;
};


static float processChannel(Input &in, Param &gain, Input &cv, Param &response) {
	float out = in.value * gain.value;
	if (out == 0.0)
		return 0.0;

	if (cv.active) {
		float linear = fmaxf(cv.value / 5.0, 0.0);
		if (linear == 0.0)
			return 0.0;
		const float ex = 200.0;
		float exponential = rescalef(powf(ex, linear), 1.0, ex, 0.0, 1.0);
		out *= crossf(exponential, linear, response.value);
	}
	return out;
}

void Veils::step() {
	float out = 0.0;
	out += processChannel(inputs[IN1_INPUT], params[GAIN1_PARAM], inputs[CV1_INPUT], params[RESPONSE1_PARAM]);
	lights[0] = out;
	if (outputs[OUT1_OUTPUT].active) {
		outputs[OUT1_OUTPUT].value = out;
		out = 0.0;
	}

	out += processChannel(inputs[IN2_INPUT], params[GAIN2_PARAM], inputs[CV2_INPUT], params[RESPONSE2_PARAM]);
	lights[1] = out;
	if (outputs[OUT2_OUTPUT].active) {
		outputs[OUT2_OUTPUT].value = out;
		out = 0.0;
	}

	out += processChannel(inputs[IN3_INPUT], params[GAIN3_PARAM], inputs[CV3_INPUT], params[RESPONSE3_PARAM]);
	lights[2] = out;
	if (outputs[OUT3_OUTPUT].active) {
		outputs[OUT3_OUTPUT].value = out;
		out = 0.0;
	}

	out += processChannel(inputs[IN4_INPUT], params[GAIN4_PARAM], inputs[CV4_INPUT], params[RESPONSE4_PARAM]);
	lights[3] = out;
	if (outputs[OUT4_OUTPUT].active) {
		outputs[OUT4_OUTPUT].value = out;
	}
}


VeilsWidget::VeilsWidget() {
	Veils *module = new Veils();
	setModule(module);
	box.size = Vec(15*12, 380);

	{
		Panel *panel = new LightPanel();
		panel->backgroundImage = Image::load(assetPlugin(plugin, "res/Veils.png"));
		panel->box.size = box.size;
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(150, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));
	addChild(createScrew<ScrewSilver>(Vec(150, 365)));

	addParam(createParam<Rogan1PSWhite>(Vec(8, 52), module, Veils::GAIN1_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<Rogan1PSWhite>(Vec(8, 131), module, Veils::GAIN2_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<Rogan1PSWhite>(Vec(8, 210), module, Veils::GAIN3_PARAM, 0.0, 1.0, 0.0));
	addParam(createParam<Rogan1PSWhite>(Vec(8, 288), module, Veils::GAIN4_PARAM, 0.0, 1.0, 0.0));

	addParam(createParam<Trimpot>(Vec(72, 56), module, Veils::RESPONSE1_PARAM, 0.0, 1.0, 1.0));
	addParam(createParam<Trimpot>(Vec(72, 135), module, Veils::RESPONSE2_PARAM, 0.0, 1.0, 1.0));
	addParam(createParam<Trimpot>(Vec(72, 214), module, Veils::RESPONSE3_PARAM, 0.0, 1.0, 1.0));
	addParam(createParam<Trimpot>(Vec(72, 292), module, Veils::RESPONSE4_PARAM, 0.0, 1.0, 1.0));

	addInput(createInput<PJ301MPort>(Vec(110, 41), module, Veils::IN1_INPUT));
	addInput(createInput<PJ301MPort>(Vec(110, 120), module, Veils::IN2_INPUT));
	addInput(createInput<PJ301MPort>(Vec(110, 198), module, Veils::IN3_INPUT));
	addInput(createInput<PJ301MPort>(Vec(110, 277), module, Veils::IN4_INPUT));

	addInput(createInput<PJ301MPort>(Vec(110, 80), module, Veils::CV1_INPUT));
	addInput(createInput<PJ301MPort>(Vec(110, 159), module, Veils::CV2_INPUT));
	addInput(createInput<PJ301MPort>(Vec(110, 238), module, Veils::CV3_INPUT));
	addInput(createInput<PJ301MPort>(Vec(110, 316), module, Veils::CV4_INPUT));

	addOutput(createOutput<PJ301MPort>(Vec(144, 41), module, Veils::OUT1_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(144, 120), module, Veils::OUT2_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(144, 198), module, Veils::OUT3_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(144, 277), module, Veils::OUT4_OUTPUT));

	addChild(createValueLight<MediumLight<GreenRedPolarityLight>>(Vec(150, 87), &module->lights[0]));
	addChild(createValueLight<MediumLight<GreenRedPolarityLight>>(Vec(150, 166), &module->lights[1]));
	addChild(createValueLight<MediumLight<GreenRedPolarityLight>>(Vec(150, 245), &module->lights[2]));
	addChild(createValueLight<MediumLight<GreenRedPolarityLight>>(Vec(150, 324), &module->lights[3]));
}
