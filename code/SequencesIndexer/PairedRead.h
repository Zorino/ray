/*
 	Ray
    Copyright (C) 2010, 2011, 2012 Sébastien Boisvert

	http://DeNovoAssembler.SourceForge.Net/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You have received a copy of the GNU General Public License
    along with this program (gpl-3.0.txt).  
	see <http://www.gnu.org/licenses/>

*/

#ifndef _PairedRead
#define _PairedRead

#include <code/SequencesLoader/ReadHandle.h>
#include <code/Mock/common_functions.h>

#include <fstream>
using namespace std;

/*
 *    LeftRead							RightRead
 * ------------>
 *                                                       <-----------
 *
 *                   AverageFragmentLength  +/- StandardDeviation
 * <----------------------------------------------------------------->
 * \author Sébastien Boisvert
 */
class PairedRead{
	uint32_t m_readIndex;
	uint16_t m_rank; // should be Rank
	LibraryHandle m_library;
public:
	void constructor(int rank,int id,int library);
	Rank getRank();
	uint32_t getId();
	ReadHandle getUniqueId();
	int getLibrary();

	void read(istream*f);
	void write(ostream*f);
} ATTRIBUTE_PACKED;

#endif
