/*
 * ColorLists.h
 *
 *  Created on: 08.02.2010
 *      Author: david
 */

#ifndef INCLUDED_DANVIL_COLORLISTS_H_
#define INCLUDED_DANVIL_COLORLISTS_H_
//---------------------------------------------------------------------------
#include <Danvil/Color.h>
#include <vector>
#include <map>
#include <boost/assign.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_inserter.hpp>
using namespace boost::assign;
//---------------------------------------------------------------------------
namespace Danvil {
//---------------------------------------------------------------------------
namespace ColorLists {

inline
std::vector<Color::EColorName> DefaultListNames() {
	std::vector<Color::EColorName> v;
	v += Color::Black,
			Color::Red, Color::Green, Color::Blue,
			Color::Yellow, Color::Cyan, Color::Magenta;
	return v;
}

template<typename K>
std::vector<TColor<K> > DefaultList() {
	std::vector<TColor<K> > v;
	std::vector<Color::EColorName> default_names = DefaultListNames();
	for(std::vector<Color::EColorName>::const_iterator it=default_names.begin(); it!=default_names.end(); ++it) {
		v.push_back(*it);
	}
	return v;
}

template<typename IntType>
std::map<IntType, Color::EColorName> DefaultMapNames() {
	std::map<IntType, Color::EColorName> v;
	insert(v)
		(0, Color::Black)
		(1, Color::Red) (2, Color::Green) (3, Color::Blue)
		(4, Color::Yellow) (5, Color::Cyan) (6, Color::Magenta);
	return v;
}

template<typename IntType, typename K>
std::map<IntType, TColor<K> > DefaultMap() {
	typedef typename std::map<IntType, Color::EColorName>::value_type P;
	std::map<IntType, TColor<K> > v;
	std::map<IntType, Color::EColorName> default_names = DefaultMapNames<IntType>();
	for(typename std::map<IntType, Color::EColorName>::const_iterator it=default_names.begin(); it!=default_names.end(); ++it) {
		v[it->first] = it->second;
	}
	return v;
}

}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
