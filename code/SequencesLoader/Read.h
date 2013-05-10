/*
 	Ray
    Copyright (C)  2010, 2011, 2012 Sébastien Boisvert

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

#ifndef _Read
#define _Read

#include <code/SequencesIndexer/PairedRead.h>

#include <RayPlatform/memory/MyAllocator.h>

#include <string>
#include <stdint.h>
#include <vector>
#include <fstream>
using namespace std;

#define TYPE_SINGLE_END 0
#define TYPE_LEFT_END 1
#define TYPE_RIGHT_END 2

/**
 * a read is represented as a uint8_t*,
 * 2 bits per nucleotide
 * and a (possible) link to paired information.
 * \author Sébastien Boisvert
 */
class Read{
	PairedRead m_pairedRead;// the read on the left
	uint8_t *m_sequence;

	/* maximum value: 65535 */
	uint16_t m_length;
	uint8_t m_type;
	
	// for the scaffolder:
	uint8_t m_forwardOffset;
	uint8_t m_reverseOffset;

	char*trim(char*a,const char*b);
public:
	void constructor(const char*sequence,MyAllocator*seqMyAllocator,bool trim);
	void constructorWithRawSequence(const char*sequence,uint8_t*raw,bool trim);
	void getSeq(char*buffer,bool color,bool doubleEncoding)const;
	int length()const;
	Kmer getVertex(int pos,int w,char strand,bool color)const;
	bool hasPairedRead()const;
	PairedRead*getPairedRead();
	uint8_t*getRawSequence();
	int getRequiredBytes();
	void setRawSequence(uint8_t*seq,int length);
	void setRightType();
	void setLeftType();
	int getType();
	void setType(uint8_t type);
	void setForwardOffset(int a);
	void setReverseOffset(int a);
	int getForwardOffset();
	int getReverseOffset();

	void writeOffsets(ostream*f);
	void readOffsets(istream*f);

	void write(ostream*f);
	void read(istream*f,MyAllocator*seqMyAllocator);
} ATTRIBUTE_PACKED;

#endif
