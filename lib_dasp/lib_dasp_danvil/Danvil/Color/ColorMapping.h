/*
 * ColorMapping.h
 *
 *  Created on: 14.02.2009
 *     Changed: 03.08.2010
 *      Author: david
 */

#ifndef DANVIL_COLOR_COLORMAPPING_H_
#define DANVIL_COLOR_COLORMAPPING_H_
#ifndef DANVIL_COLOR_H_
	#error "Do not include this file directly. Use Danvil/Color.h instead!"
#endif
//---------------------------------------------------------------------------
#include "Color.h"
#include <map>
#include <vector>
#include <cmath>
#include <stdexcept>
//---------------------------------------------------------------------------
namespace Danvil {
//---------------------------------------------------------------------------

/** Interface for mapping a value to a color */
template<typename ColorChannelType, typename ValueType>
class IColorMapping
{
public:
	typedef TColor<ColorChannelType> ColorType;

public:
	virtual ColorType operator()(const ValueType& v) const = 0;

};

//---------------------------------------------------------------------------

/** Maps a discrete list of valued to a color */
template<typename ColorChannelType, typename ValueType>
class DiscreetColorMapping
	: public IColorMapping<ColorChannelType, ValueType>
{
public:
	typedef typename IColorMapping<ColorChannelType, ValueType>::ColorType ColorType;

	typedef std::map<ValueType, ColorType> Mapping;

public:
	/** Default constructor maps everything to white */
	DiscreetColorMapping()
		: _defaultColor(Color::White)
	{}

	/** Explicit constructor */
	DiscreetColorMapping(const Mapping& colors, const ColorType& default_color)
		: _colors(colors),
		  _defaultColor(default_color)
	{}

	/** Tests if a value type has an explicit mapping */
	bool contains(const ValueType& i) const {
		return (_colors.find(i) != _colors.end());
	}

	/** Establishes a mapping from value to color */
	void set(const ValueType& value, const ColorType& color) {
		_colors[value] = color;
	}

	/** Sets the default color */
	void setDefault(const ColorType& default_color) {
		_defaultColor = default_color;
	}

	ColorType operator()(const ValueType& v) const {
		typename Mapping::const_iterator it = _colors.find(v);
		if(it == _colors.end()) {
			return _defaultColor;
		} else {
			return it->second;
		}
	}

	ColorType map(const ValueType& v) const {
		return (*this)(v);
	}

public:
	/** Factors a simple color mapping
	 * It maps the integers 0-6 to the primary colors
	 * (Black, Red, Green, Blue, Yellow, Cyan, Magenta)
	 * with default color White.
	 */
	static DiscreetColorMapping FactorPrimary(){
		DiscreetColorMapping a;
		a.set(0, Color::Black);
		a.set(1, Color::Red);
		a.set(2, Color::Green);
		a.set(3, Color::Blue);
		a.set(4, Color::Yellow);
		a.set(5, Color::Cyan);
		a.set(6, Color::Magenta);
		a.setDefault(Color::White);
		return a;
	}

private:
	Mapping _colors;

	ColorType _defaultColor;

};

//---------------------------------------------------------------------------

namespace Palettes
{
	enum Palette {
		Black_White,
		White_Black,
		Blue_White_Red,
		DarkBlue_Blue_White_Red_DarkRed,
		Blue_Red_Yellow,
		Blue_Red_Yellow_White,
		Yellow_Red_Blue,
		Black_Blue_Red_Yellow_White,
		HueWhiteToRed,
		Red_Green_Blue
	};
}

typedef Palettes::Palette Palette;

/** Maps a closed interval of a value type to a color.
 * Values outside the range interval will be mapped to first resp. last
 * color in the palette. Values inside the value range will be mapped
 * to a color interpolated from the given palette colors.
 * The value type should be convertible to float.
 */
template<typename ColorChannelType, typename ValueType>
class ContinuousIntervalColorMapping
	: public IColorMapping<ColorChannelType, ValueType>
{
public:
	typedef typename IColorMapping<ColorChannelType, ValueType>::ColorType ColorType;

	typedef typename std::vector<ColorType> PaletteType;

public:
	/** Default constructor uses the range [0,1] */
	ContinuousIntervalColorMapping() {
		setRange(0.0f, 1.0f);
		use_custom_border_colors_ = false;
	}

	virtual ~ContinuousIntervalColorMapping() {}

	/** Sets the value range to [0, max] */
	void setRange(const ValueType& max) {
		_min = 0.0f;
		_max = max;
		_interval_length_inv = 1.0f / (float)max;
	}

	/** Sets the value range to [min, max] */
	void setRange(const ValueType& min, const ValueType& max) {
		_min = min;
		_max = max;
		_interval_length_inv = 1.0f / (float)(_max - _min);
	}

	/** Lower bound of the value range */
	const ValueType& min() const { return _min; }

	/** Upper bound of the value range */
	const ValueType& max() const { return _max; }

	/** Adds a color to the end of the palette */
	void add(const ColorType& color) {
		palette.push_back(color);
		UpdateBorderColors();
	}

	/** Sets the color palette */
	void setPalette(const PaletteType& colors) {
		palette = colors;
		UpdateBorderColors();
	}

	void useCustomBorderColors(const ColorType& near, const ColorType& far) {
		use_custom_border_colors_ = true;
		underdraw_ = near;
		overdraw_ = far;
	}

	/** Color used for values smaller than the minium value */
	const ColorType& underdrawColor() const {
		return underdraw_;
	}

	/** Color used for values greater than the maximum value */
	const ColorType& overdrawColor() const {
		return overdraw_;
	}

	ColorType operator()(const ValueType& value) const {
		if(palette.size() == 0) {
			// empty palette is an error!
			return Color::Black;
		}
		else {
			// call relative position in the range interval
			float p = (float(value) - float(_min))  * _interval_length_inv; // FIXME officially needs a distance function here
			if( p < 0 ) {
				return underdrawColor();
			}
			if( p >= 1 ) {
				return overdrawColor();
			}
			if(palette.size() == 1) {
				// just use first (and only) color
				return palette[0];
			}
			else {
				// get closest upper and lower color
				float ganz;
				float rest = std::modf(p * (palette.size() - 1), &ganz);
				int ganz_int = (int)ganz;
				ganz_int = std::min<size_t>(palette.size() - 1, std::max<size_t>(0, ganz_int));
				ColorType c1 = palette[ganz_int];
				ColorType c2 = palette[ganz_int + 1];
				// interpolate
				return ColorType::Mix(c1, c2, rest);
			}
		}
	}

public:
	static ContinuousIntervalColorMapping Factor(Palette p) {
		switch(p) {
		case Palettes::Black_White: return FactorBlackWhite();
		case Palettes::White_Black: return FactorWhiteBlack();
		case Palettes::Blue_White_Red: return Factor_Blue_White_Red();
		case Palettes::DarkBlue_Blue_White_Red_DarkRed: return Factor_DarkBlue_Blue_White_Red_DarkRed();
		case Palettes::Blue_Red_Yellow: return Factor_Blue_Red_Yellow();
		case Palettes::Blue_Red_Yellow_White: return Factor_Blue_Red_Yellow_White();
		case Palettes::Yellow_Red_Blue: return Factor_Yellow_Red_Blue();
		case Palettes::Black_Blue_Red_Yellow_White: return Factor_Black_Blue_Red_Yellow_White();
		case Palettes::HueWhiteToRed: return FactorHueWhiteToRed();
		case Palettes::Red_Green_Blue: return FactorRedGreenBlue();
		default: return FactorBlackWhite();
		}
	}

	static ContinuousIntervalColorMapping FactorBlackWhite() {
		ContinuousIntervalColorMapping p;
		p.add(Color::Black);
		p.add(Color::White);
		return p;
	}

	static ContinuousIntervalColorMapping FactorWhiteBlack() {
		ContinuousIntervalColorMapping p;
		p.add(Color::White);
		p.add(Color::Black);
		return p;
	}

	static ContinuousIntervalColorMapping Factor_Blue_White_Red() {
		ContinuousIntervalColorMapping p;
		p.add(Color::Blue);
		p.add(Color::White);
		p.add(Color::Red);
		return p;
	}

	static ContinuousIntervalColorMapping Factor_DarkBlue_Blue_White_Red_DarkRed() {
		ContinuousIntervalColorMapping p;
		p.add(Color::DarkBlue);
		p.add(Color::Blue);
		p.add(Color::White);
		p.add(Color::Red);
		p.add(Color::DarkRed);
		return p;
	}

	static ContinuousIntervalColorMapping Factor_Blue_Red_Yellow() {
		ContinuousIntervalColorMapping p;
		p.add(Color::Blue);
		p.add(Color::Red);
		p.add(Color::Yellow);
		return p;
	}

	static ContinuousIntervalColorMapping Factor_Blue_Red_Yellow_Green() {
		ContinuousIntervalColorMapping p;
		p.add(Color::Blue);
		p.add(Color::Red);
		p.add(Color::Yellow);
		p.add(Color::Green);
		return p;
	}

	static ContinuousIntervalColorMapping Factor_Red_Green() {
		ContinuousIntervalColorMapping p;
		p.add(Color::Red);
		p.add(Color::Green);
		return p;
	}

	static ContinuousIntervalColorMapping Factor_Blue_Black_Red() {
		ContinuousIntervalColorMapping p;
		p.add(Color::Blue);
		p.add(Color::Black);
		p.add(Color::Red);
		return p;
	}

	static ContinuousIntervalColorMapping Factor_LightBlue_Black_Red() {
		ContinuousIntervalColorMapping p;
		p.add(Color::Cyan);
		p.add(Color::Black);
		p.add(Color::Red);
		return p;
	}

	static ContinuousIntervalColorMapping Factor_MinusPlus_Old() {
		ContinuousIntervalColorMapping p;
		p.add(Color::Cyan);
		p.add(Color::LightGrey);
		p.add(Color::Red);
		return p;
	}

	static ContinuousIntervalColorMapping Factor_MinusPlus() {
		ContinuousIntervalColorMapping p;
		p.add(Color::Blue);
		p.add(Color::Grey);
		p.add(Color::Red);
		return p;
	}

	static ContinuousIntervalColorMapping Factor_Blue_Red_Yellow_White() {
		ContinuousIntervalColorMapping p;
		p.add(Color::Blue);
		p.add(Color::Red);
		p.add(Color::Yellow);
		p.add(Color::White);
		return p;
	}

	static ContinuousIntervalColorMapping Factor_Yellow_Red_Blue() {
		ContinuousIntervalColorMapping p;
		p.add(Color::Yellow);
		p.add(Color::Red);
		p.add(Color::Blue);
		return p;
	}

	static ContinuousIntervalColorMapping Factor_Black_Blue_Red_Yellow_White() {
		ContinuousIntervalColorMapping p;
		p.add(Color::Black);
		p.add(Color::Blue);
		p.add(Color::Red);
		p.add(Color::Yellow);
		p.add(Color::White);
		return p;
	}

	static ContinuousIntervalColorMapping FactorHueWhiteToRed() {
		ContinuousIntervalColorMapping p;
		p.add(Color::White);
		p.add(Color::Yellow);
		p.add(Color::Green);
		p.add(Color::Cyan);
		p.add(Color::Blue);
		p.add(Color::Magenta);
		p.add(Color::Red);
		return p;
	}

	static ContinuousIntervalColorMapping FactorRedGreenBlue() {
		ContinuousIntervalColorMapping p;
		p.add(Color::Red);
		p.add(Color::Yellow);
		p.add(Color::Green);
		p.add(Color::Cyan);
		p.add(Color::Blue);
		return p;
	}

private:
	void UpdateBorderColors() {
		if(!use_custom_border_colors_) {
			if(palette.size() == 0) {
				underdraw_ = ColorType(Color::Black);
				overdraw_ = ColorType(Color::Black);
			}
			else {
				underdraw_ = palette[0];
				overdraw_ = palette[palette.size() - 1];
			}
		}
	}

public:
	PaletteType palette;

private:
	/** Lower bound of the value interval */
	ValueType _min;

	/** Upper bound of the value interval */
	ValueType _max;

	/** Precomputed helper value */
	float _interval_length_inv;

	bool use_custom_border_colors_;

	ColorType underdraw_;

	ColorType overdraw_;

};

/** Type of palette mapping a float typed value interval to colors of type Colorf */
typedef ContinuousIntervalColorMapping<float, float> ContinuousIntervalColorMapping_ff;

typedef ContinuousIntervalColorMapping<unsigned char, float> ContinuousIntervalColorMapping_ubf;

/** Type of palette mapping a integer typed value interval to colors of type Colorf */
typedef ContinuousIntervalColorMapping<float, int> ContinuousIntervalColorMapping_fi;

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
