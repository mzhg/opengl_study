#pragma once

namespace jet
{
	namespace util
	{
		class ColorModulation
		{
		private:
			ColorModulation& operator=(ColorModulation&) = delete;
			ColorModulation(ColorModulation&) = delete;

			void makeColorMoluations();
			void fillData();

		protected:
			ColorModulation();
			~ColorModulation(){}

		public:
			static const ColorModulation& get();

			float filmic_ghost_modulation1st[16];
			float filmic_ghost_modulation2nd[16];

			float camera_ghost_modulation1st[16];
			float camera_ghost_modulation2nd[16];

			float hori_passes[7][16];

			enum 
			{
				HORI0,
				HORI1,
				HORI2,
				HORI3,
				STAR0,
				STAR1,
				STAR2
			};

		private:
			// temp variables
			float star_modulation1st[16];
			float star_modulation2nd[16];
			float star_modulation3rd[16];

			float hori_modulation1st[16];
			float hori_modulation2nd[16];
			float hori_modulation3rd[16];
		};
	}
}

