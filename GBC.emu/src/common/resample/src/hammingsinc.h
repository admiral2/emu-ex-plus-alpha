/***************************************************************************
 *   Copyright (C) 2008 by Sindre Aamås                                    *
 *   aamas@stud.ntnu.no                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 as     *
 *   published by the Free Software Foundation.                            *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License version 2 for more details.                *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   version 2 along with this program; if not, write to the               *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef HAMMINGSINC_H
#define HAMMINGSINC_H

#include "convoluter.h"
#include "subresampler.h"
#include "makesinckernel.h"
#include "cic3.h"
#include "array.h"
#include <cmath>
#include <cstdlib>

template<unsigned channels, unsigned phases>
class HammingSinc : public SubResampler {
	PolyPhaseConvoluter<channels, phases> convoluters[channels];
	Array<short> kernel;
	
	static SysDDec hammingWin(const long i, const long M) {
		static const SysDDec PI = 3.14159265358979323846;
		return 0.53836 - 0.46164 * std::cos(2 * PI * i / M);
	}
	
	void init(unsigned div, unsigned phaseLen, SysDDec fc);
	
public:
	enum { MUL = phases };
	
	typedef Cic3<channels> Cic;
	static float cicLimit() { return 4.2f; }

	class RollOff {
		static unsigned toTaps(const float rollOffWidth) {
			static const float widthTimesTaps = 3.0f;
			return static_cast<unsigned>(std::ceil(widthTimesTaps / rollOffWidth));
		}
		
		static float toFc(const float rollOffStart, const int taps) {
			static const float startToFcDeltaTimesTaps = 1.27f;
			return startToFcDeltaTimesTaps / taps + rollOffStart;
		}
		
	public:
		const unsigned taps;
		const float fc;
		
		RollOff(float rollOffStart, float rollOffWidth) : taps(toTaps(rollOffWidth)), fc(toFc(rollOffStart, taps)) {}
	};

	HammingSinc(unsigned div, unsigned phaseLen, SysDDec fc) { init(div, phaseLen, fc); }
	HammingSinc(unsigned div, RollOff ro) { init(div, ro.taps, ro.fc); }
	std::size_t resample(short *out, const short *in, std::size_t inlen);
	void adjustDiv(unsigned div);
	unsigned mul() const { return MUL; }
	unsigned div() const { return convoluters[0].div(); }
};

template<unsigned channels, unsigned phases>
void HammingSinc<channels, phases>::init(const unsigned div, const unsigned phaseLen, const SysDDec fc) {
	kernel.reset(phaseLen * phases);
	
	makeSincKernel(kernel, phases, phaseLen, fc, hammingWin);
	
	for (unsigned i = 0; i < channels; ++i)
		convoluters[i].reset(kernel, phaseLen, div);
}

template<unsigned channels, unsigned phases>
std::size_t HammingSinc<channels, phases>::resample(short *const out, const short *const in, const std::size_t inlen) {
	std::size_t samplesOut;
	
	for (unsigned i = 0; i < channels; ++i)
		samplesOut = convoluters[i].filter(out + i, in + i, inlen);
	
	return samplesOut;
}

template<unsigned channels, unsigned phases>
void HammingSinc<channels, phases>::adjustDiv(const unsigned div) {
	for (unsigned i = 0; i < channels; ++i)
		convoluters[i].adjustDiv(div);
}

#endif
