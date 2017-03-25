#include "ColorModulation.h"
#include <math.h>

namespace jet
{
	namespace util
	{
		const ColorModulation& ColorModulation::get()
		{
			static ColorModulation instance;
			return instance;
		}

		static void colorModulationRedShift(float* color, float r, float g, float b, int num)
		{
			color[4 * num] = color[4 * num] * r;
			color[4 * num + 1] = color[4 * num] * g;
			color[4 * num + 2] = color[4 * num] * b;
			//color[4*num+3] = color[4*num]*a;
		}

		static void colorModulation(float* color, float r, float g, float b, int num)
		{
			color[4 * num] = color[4 * num + 0] * r;
			color[4 * num + 1] = color[4 * num + 1] * g;
			color[4 * num + 2] = color[4 * num + 2] * b;
			//color[4*num+3] = color[4*num]*a;
		}

		void ColorModulation::fillData()
		{
			//color modulation coefficients for star streak & ghost image
			for (int i = 0; i<16; i++) 
			{
				star_modulation1st[i] = 0.25f;
				star_modulation2nd[i] = 0.25f;
				star_modulation3rd[i] = 0.25f;
				hori_modulation1st[i] = 0.5f;
				hori_modulation2nd[i] = 0.5f;
				hori_modulation3rd[i] = 0.5f;
				filmic_ghost_modulation1st[i] = 1.0f;
				filmic_ghost_modulation2nd[i] = 1.0f;
				camera_ghost_modulation1st[i] = 1.0f;
				camera_ghost_modulation2nd[i] = 1.0f;
			}
			//star
			colorModulationRedShift(star_modulation1st, 1.0f, 0.95f, 0.9f, 0);
			colorModulationRedShift(star_modulation1st, 0.8f, 1.0f, 0.9f, 1);
			colorModulationRedShift(star_modulation1st, 0.9f, 0.9f, 1.0f, 2);
			colorModulationRedShift(star_modulation1st, 0.9f, 1.0f, 0.9f, 3);

			colorModulationRedShift(star_modulation2nd, 1.0f, 0.9f, 0.8f, 0);
			colorModulationRedShift(star_modulation2nd, 1.0f, 0.6f, 0.5f, 1);
			colorModulationRedShift(star_modulation2nd, 0.5f, 1.0f, 0.6f, 2);
			colorModulationRedShift(star_modulation2nd, 0.6f, 0.4f, 1.0f, 3);

			colorModulationRedShift(star_modulation3rd, 1.0f, 0.6f, 0.6f, 1);
			colorModulationRedShift(star_modulation3rd, 0.6f, 1.0f, 0.6f, 2);
			colorModulationRedShift(star_modulation3rd, 0.6f, 0.6f, 1.0f, 3);

			colorModulation(hori_modulation1st, 0.2f, 0.3f, 0.95f, 0);
			colorModulation(hori_modulation1st, 0.2f, 0.3f, 0.95f, 1);
			colorModulation(hori_modulation1st, 0.1f, 0.2f, 0.9f, 2);
			colorModulation(hori_modulation1st, 0.2f, 0.3f, 0.95f, 3);

			colorModulation(hori_modulation2nd, 0.2f, 0.3f, 0.95f, 0);
			colorModulation(hori_modulation2nd, 0.1f, 0.2f, 0.9f, 1);
			colorModulation(hori_modulation2nd, 0.02f, 0.1f, 0.99f, 2);
			colorModulation(hori_modulation2nd, 0.02f, 0.1f, 0.99f, 3);

			colorModulation(hori_modulation3rd, 1.0f, 1.0f, 1.0f, 0);
			colorModulation(hori_modulation3rd, 1.0f, 1.0f, 1.0f, 1);
			colorModulation(hori_modulation3rd, 1.0f, 1.0f, 1.0f, 2);
			colorModulation(hori_modulation3rd, 1.0f, 1.0f, 1.0f, 3);

			//ghost camera
			colorModulationRedShift(camera_ghost_modulation1st, 1.0f, 0.9f, 0.8f, 0);
			colorModulationRedShift(camera_ghost_modulation1st, 1.0f, 0.6f, 0.5f, 1);
			colorModulationRedShift(camera_ghost_modulation1st, 0.5f, 1.0f, 0.6f, 2);
			colorModulationRedShift(camera_ghost_modulation1st, 1.0f, 0.7f, 0.3f, 3);

			colorModulationRedShift(camera_ghost_modulation2nd, 0.2f, 0.3f, 0.7f, 0);
			colorModulationRedShift(camera_ghost_modulation2nd, 0.5f, 0.3f, 0.2f, 1);
			colorModulationRedShift(camera_ghost_modulation2nd, 0.1f, 0.5f, 0.2f, 2);
			colorModulationRedShift(camera_ghost_modulation2nd, 0.1f, 0.1f, 1.0f, 3);

			//ghost filmic
			colorModulation(filmic_ghost_modulation1st, 0.1f, 0.1f, 1.0f, 0);
			colorModulation(filmic_ghost_modulation1st, 0.2f, 0.3f, 1.0f, 1);
			colorModulation(filmic_ghost_modulation1st, 0.1f, 0.2f, 0.6f, 2);
			colorModulation(filmic_ghost_modulation1st, 0.6f, 0.3f, 1.0f, 3);

			colorModulation(filmic_ghost_modulation2nd, 0.6f, 0.2f, 0.2f, 0);
			colorModulation(filmic_ghost_modulation2nd, 0.2f, 0.06f, 0.6f, 1);
			colorModulation(filmic_ghost_modulation2nd, 0.15f, 0.00f, 0.1f, 2);
			colorModulation(filmic_ghost_modulation2nd, 0.06f, 0.00f, 0.55f, 3);

		}
		
		void ColorModulation::makeColorMoluations()
		{
			int s;
			int n;
			float dec = 0.96f;
			float* colorCoeff = hori_passes[0];
			//4 passes to generate 256 pixel blur in each direction
			//1st pass
			n = 1;
			for (s = 0; s<4; s += 1) {
				colorCoeff[s * 4 + 0] = (float)(hori_modulation1st[s * 4 + 0] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 1] = (float)(hori_modulation1st[s * 4 + 1] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 2] = (float)(hori_modulation1st[s * 4 + 2] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 3] = (float)(hori_modulation1st[s * 4 + 3] * pow((dec), pow(4.0f, n - 1)*s));
			}
			//			hori_passes[0] = colorCoeff;

			// 2nd pass
			colorCoeff = hori_passes[1];
			n = 2;
			for (s = 0; s<4; s += 1) {
				colorCoeff[s * 4 + 0] = (float)(hori_modulation1st[s * 4 + 0] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 1] = (float)(hori_modulation1st[s * 4 + 1] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 2] = (float)(hori_modulation1st[s * 4 + 2] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 3] = (float)(hori_modulation1st[s * 4 + 3] * pow((dec), pow(4.0f, n - 1)*s));
			}
			//			hori_passes[1] = colorCoeff;

			// 3rd pass
			n = 3;
			colorCoeff = hori_passes[2];
			for (s = 0; s<4; s += 1) {
				colorCoeff[s * 4 + 0] = (float)(hori_modulation2nd[s * 4 + 0] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 1] = (float)(hori_modulation2nd[s * 4 + 1] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 2] = (float)(hori_modulation2nd[s * 4 + 2] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 3] = (float)(hori_modulation2nd[s * 4 + 3] * pow((dec), pow(4.0f, n - 1)*s));
			}
			//			hori_passes[2] = colorCoeff;

			// 4th pass
			n = 4;
			colorCoeff = hori_passes[3];
			for (s = 0; s<4; s += 1) {
				colorCoeff[s * 4 + 0] = (float)(hori_modulation3rd[s * 4 + 0] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 1] = (float)(hori_modulation3rd[s * 4 + 1] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 2] = (float)(hori_modulation3rd[s * 4 + 2] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 3] = (float)(hori_modulation3rd[s * 4 + 3] * pow((dec), pow(4.0f, n - 1)*s));
			}
			//			hori_passes[3] = colorCoeff;

			//3 passes to generate 64 pixel blur in each direction
			dec = 0.9f;
			//1st pass
			colorCoeff = hori_passes[4];
			n = 1;
			for (s = 0; s<4; s += 1) {
				colorCoeff[s * 4 + 0] = (float)(star_modulation1st[s * 4 + 0] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 1] = (float)(star_modulation1st[s * 4 + 1] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 2] = (float)(star_modulation1st[s * 4 + 2] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 3] = (float)(star_modulation1st[s * 4 + 3] * pow((dec), pow(4.0f, n - 1)*s));
			}
			//			hori_passes[4] = colorCoeff;

			// 2nd pass
			colorCoeff = hori_passes[5];
			n = 2;
			for (s = 0; s<4; s += 1) {
				colorCoeff[s * 4 + 0] = (float)(star_modulation2nd[s * 4 + 0] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 1] = (float)(star_modulation2nd[s * 4 + 1] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 2] = (float)(star_modulation2nd[s * 4 + 2] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 3] = (float)(star_modulation2nd[s * 4 + 3] * pow((dec), pow(4.0f, n - 1)*s));
			}
			//			hori_passes[5] = colorCoeff;

			// 3rd pass
			colorCoeff = hori_passes[6];
			n = 3;
			for (s = 0; s<4; s += 1) {
				colorCoeff[s * 4 + 0] = (float)(star_modulation3rd[s * 4 + 0] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 1] = (float)(star_modulation3rd[s * 4 + 1] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 2] = (float)(star_modulation3rd[s * 4 + 2] * pow((dec), pow(4.0f, n - 1)*s));
				colorCoeff[s * 4 + 3] = (float)(star_modulation3rd[s * 4 + 3] * pow((dec), pow(4.0f, n - 1)*s));
			}
			//			hori_passes[6] = colorCoeff;
		}

		ColorModulation::ColorModulation()
		{
			fillData();
			makeColorMoluations();
		}
	}
}
