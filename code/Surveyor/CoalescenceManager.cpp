/*
    Copyright 2013 Sébastien Boisvert
    Copyright 2013 Université Laval
    Copyright 2013 Centre Hospitalier Universitaire de Québec

    This file is part of Ray Surveyor.

    Ray Surveyor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    Ray Surveyor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ray Surveyor.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "CoalescenceManager.h"

#include <code/Mock/constants.h>
#include <code/Mock/common_functions.h>
#include <code/KmerAcademyBuilder/Kmer.h>
#include <code/VerticesExtractor/Vertex.h>

#include <iostream>
using namespace std;

CoalescenceManager::CoalescenceManager() {

	m_kmerLength = 0;
	m_colorSpaceMode = false;
}

CoalescenceManager::~CoalescenceManager() {

}

void CoalescenceManager::receive(Message & message) {

	int tag = message.getTag();

	/*
	printName();
	cout << " CoalescenceManager DEBUG receive message !";
	cout << endl;
*/

	if(tag == PAYLOAD) {

		receivePayload(message);

	} else if(tag == DIE) {

		die();

	} else if(tag == SET_KMER_LENGTH) {

		int kmerLength = 0;
		char * buffer = (char*)message.getBufferBytes();
		memcpy(&kmerLength, buffer, sizeof(kmerLength));

		if(m_kmerLength == 0)
			m_kmerLength = kmerLength;

		if(m_kmerLength != kmerLength) {

			printName();
			cout << " Error: the k-mer length is not the same in all input files !";
			cout << endl;
		}

		// cout << "DEBUG m_kmerLength = " << m_kmerLength << endl;

		// the color space mode is an artefact.
		m_colorSpaceMode = false;

		Message response;
		response.setTag(SET_KMER_LENGTH_OK);

		int source = message.getSourceActor();

		/*
		printName();
		cout << "DEBUG Sending SET_KMER_LENGTH_OK to " << source << endl;
		*/

		send(source, response);
	}
}

void CoalescenceManager::receivePayload(Message & message) {

	int source = message.getSourceActor();

	char * buffer = (char*)message.getBufferBytes();
	//int bytes = message.getNumberOfBytes();

	int position = 0;
	Vertex vertex;
	position += vertex.load(buffer);

#if 0
	printName();
	cout << "DEBUG received PAYLOAD from " << source;
	cout << " ";
	vertex.print(m_kmerLength, m_colorSpaceMode);
	cout << endl;

#endif

	Message response;
	response.setTag(PAYLOAD_RESPONSE);

	send(source, response);
}