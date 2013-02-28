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

#ifndef _common_functions
#define _common_functions

#include "constants.h"

#ifdef CONFIG_MPI_IO
#include <mpi.h>
#endif

#include <code/plugin_KmerAcademyBuilder/Kmer.h>
#include <code/plugin_SeedingData/GraphPath.h>

#include <RayPlatform/memory/allocator.h>
#include <RayPlatform/core/slave_modes.h>
#include <RayPlatform/core/master_modes.h>
#include <RayPlatform/communication/mpi_tags.h>

#include <string.h>
#include <string>
#include <iostream>
#include <vector>

#ifdef ASSERT
#include <assert.h>
#endif

using namespace std;

/*
 * Encode a char
 */
uint8_t charToCode(char a);

/*
 * verify that x has only A,T,C, and G
 */
bool isValidDNA(char*x);

/*
 * transform a string in a Kmer
 */
Kmer wordId(const char*a);

/*
 * add line breaks to a string
 */
string addLineBreaks(string sequence,int a);

/*
 * TODO: move this in GraphPath
 */
string convertToString(GraphPath*b,int m_wordSize,bool color);

Kmer kmerAtPosition(const char*string,int pos,int w,char strand,bool color);

PathHandle getPathUniqueId(int rank,int id);
int getIdFromPathUniqueId(PathHandle a);
int getRankFromPathUniqueId(PathHandle a);

void print64(uint64_t a);
void print8(uint8_t a);

char complementNucleotide(char c);

/*
 *  complement the sequence of a biological thing
 */
string reverseComplement(string*a);

MessageUnit pack_pointer(void**pointer);
void unpack_pointer(void**pointer,MessageUnit integerValue);

bool flushFileOperationBuffer(bool force,ostringstream*buffer,ostream*file,int bufferSize);
bool flushFileOperationBuffer_FILE(bool force,ostringstream*buffer,FILE*file,int bufferSize);

#ifdef CONFIG_MPI_IO
bool flushFileOperationBuffer_MPI_IO(bool force,ostringstream*buffer,MPI_File file,int bufferSize);
#endif

#endif


