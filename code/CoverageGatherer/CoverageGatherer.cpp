/*
 	Ray
    Copyright (C) 2011, 2012 Sébastien Boisvert

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

#include "CoverageGatherer.h"

#include <code/Mock/constants.h>
#include <code/KmerAcademyBuilder/Kmer.h>
#include <code/VerticesExtractor/GridTableIterator.h>
#include <code/VerticesExtractor/Vertex.h>

#include <RayPlatform/core/OperatingSystem.h>
#include <RayPlatform/core/slave_modes.h>
#include <RayPlatform/communication/Message.h>
#include <RayPlatform/communication/mpi_tags.h>

#include <sstream>
#include <stdio.h>
#include <stdint.h>
using namespace std;

#ifdef ASSERT
#include <assert.h>
#endif

__CreatePlugin(CoverageGatherer);

__CreateSlaveModeAdapter(CoverageGatherer,RAY_SLAVE_MODE_SEND_DISTRIBUTION);

void CoverageGatherer::writeKmers(){
	#ifdef ASSERT
	LargeCount n=0;
	#endif

	if(m_subgraph->size()==0){
		(*m_slaveMode)=RAY_SLAVE_MODE_DO_NOTHING;
		Message aMessage(NULL,0,MASTER_RANK,RAY_MPI_TAG_COVERAGE_END,
			m_parameters->getRank());
		m_outbox->push_back(&aMessage);
		return;
	}
	GridTableIterator iterator;
	iterator.constructor(m_subgraph,m_parameters->getWordSize(),m_parameters);
	FILE* kmerFile=NULL;
	ostringstream buffer;
	ostringstream name;
	name<<m_parameters->getPrefix()<<"/kmers.txt";
	if(m_parameters->getRank()==0)
		kmerFile=fopen(name.str().c_str(),"w"); // create empty file
	else
		kmerFile=fopen(name.str().c_str(),"a"); // append to file

	if(m_parameters->getRank()==MASTER_RANK)
		writeHeader(kmerFile);

	while(iterator.hasNext()){
		Vertex*node=iterator.next();
		Kmer key=*(iterator.getKey());
		CoverageDepth coverage=node->getCoverage(&key);
		m_distributionOfCoverage[coverage]++;
		#ifdef ASSERT
		n++;
		#endif
		string kmerSequence=key.idToWord(m_parameters->getWordSize(),m_parameters->getColorSpaceMode());
		vector<Kmer> parents=node->getIngoingEdges(&key,m_parameters->getWordSize());
		vector<Kmer> children=node->getOutgoingEdges(&key,m_parameters->getWordSize());

		//fprintf(kmerFile,"%s;%i;",kmerSequence.c_str(),coverage);
		buffer << kmerSequence << ";" << coverage << ";";
		for(int i=0;i<(int)parents.size();i++){
			string printableVersion=parents[i].idToWord(m_parameters->getWordSize(),m_parameters->getColorSpaceMode());
			if(i!=0)
				buffer << " ";
				//fprintf(kmerFile," ");

			//fprintf(kmerFile,"%c",printableVersion[0]);
			buffer << printableVersion[0];
		}
		//fprintf(kmerFile,";");
		buffer << ";";
		for(int i=0;i<(int)children.size();i++){
			string printableVersion=children[i].idToWord(m_parameters->getWordSize(),m_parameters->getColorSpaceMode());
			if(i!=0)
				buffer << " ";
				//fprintf(kmerFile," ");

			//fprintf(kmerFile,"%c",printableVersion[m_parameters->getWordSize()-1]);
			buffer << printableVersion[m_parameters->getWordSize()-1];
		}
		//fprintf("\n");
		buffer << endl;
		flushFileOperationBuffer_FILE(false, &buffer, kmerFile, CONFIG_FILE_IO_BUFFER_SIZE);
	}
	flushFileOperationBuffer_FILE(true, &buffer, kmerFile, CONFIG_FILE_IO_BUFFER_SIZE);
	fclose(kmerFile);

	#ifdef ASSERT
	if(n!=m_subgraph->size()){
		cout<<"n="<<n<<" size="<<m_subgraph->size()<<endl;
	}
	assert(n==m_subgraph->size());
	#endif
	m_waiting=false;
	m_coverageIterator=m_distributionOfCoverage.begin();
}

void CoverageGatherer::call_RAY_SLAVE_MODE_SEND_DISTRIBUTION(){

	if(m_distributionOfCoverage.size()==0){
		#ifdef ASSERT
		LargeCount n=0;
		#endif

		if(m_subgraph->size()==0){
			(*m_slaveMode)=RAY_SLAVE_MODE_DO_NOTHING;
			Message aMessage(NULL,0,MASTER_RANK,RAY_MPI_TAG_COVERAGE_END,
				m_parameters->getRank());
			m_outbox->push_back(&aMessage);
			return;
		}
		GridTableIterator iterator;
		iterator.constructor(m_subgraph,m_parameters->getWordSize(),m_parameters);
		while(iterator.hasNext()){
			Vertex*node=iterator.next();
			Kmer key=*(iterator.getKey());
			CoverageDepth coverage=node->getCoverage(&key);
			m_distributionOfCoverage[coverage]++;

			#ifdef ASSERT
			n++;
			#endif
		}

		#ifdef ASSERT
		if(n!=m_subgraph->size()){
			cout<<"Expected (from iterator)="<<n<<" Actual (->size())="<<m_subgraph->size()<<endl;
		}
		assert(n==m_subgraph->size());
		#endif
		m_waiting=false;
		m_coverageIterator=m_distributionOfCoverage.begin();
	}else if(m_waiting){
		if((*m_inbox).size()>0&&(*m_inbox)[0]->getTag()==RAY_MPI_TAG_COVERAGE_DATA_REPLY){
			m_waiting=false;
		}
	}else{
		MessageUnit*messageContent=(MessageUnit*)m_outboxAllocator->allocate(MAXIMUM_MESSAGE_SIZE_IN_BYTES);
		int count=0;
		int maximumElements=MAXIMUM_MESSAGE_SIZE_IN_BYTES/sizeof(MessageUnit);
		while(count<maximumElements && m_coverageIterator!=m_distributionOfCoverage.end()){
			CoverageDepth coverage=m_coverageIterator->first;
			LargeCount numberOfVertices=m_coverageIterator->second;
			messageContent[count]=coverage;
			messageContent[count+1]=numberOfVertices;
			count+=2;
			m_coverageIterator++;
		}

		if(count!=0){
			Message aMessage(messageContent,count,MASTER_RANK,RAY_MPI_TAG_COVERAGE_DATA,
				m_parameters->getRank());

			m_outbox->push_back(&aMessage);
			m_waiting=true;
		}else{
			m_distributionOfCoverage.clear();
			(*m_slaveMode)=RAY_SLAVE_MODE_DO_NOTHING;
			Message aMessage(NULL,0,MASTER_RANK,RAY_MPI_TAG_COVERAGE_END,
				m_parameters->getRank());
			m_outbox->push_back(&aMessage);
		}
	}
}

void CoverageGatherer::constructor(Parameters*parameters,StaticVector*inbox,StaticVector*outbox,int*slaveMode,
	GridTable*subgraph,RingAllocator*outboxAllocator){
	m_parameters=parameters;
	m_slaveMode=slaveMode;
	m_outboxAllocator=outboxAllocator;
	m_inbox=inbox;
	m_outbox=outbox;
	m_subgraph=subgraph;
}

void CoverageGatherer::writeHeader(FILE*kmerFile){

	fprintf(kmerFile,"# Generated by Ray %s\n",CONFIG_RAY_VERSION);
	fprintf(kmerFile,"# The length of k-mers is %i\n",m_parameters->getWordSize());
	if(m_parameters->getColorSpaceMode())
		fprintf(kmerFile,"# This file contains the k-mer graph (subgraph of the de Bruijn graph for alphabet={0,1,2,3})\n");
	else
		fprintf(kmerFile,"# This file contains the k-mer graph (subgraph of the de Bruijn graph for alphabet={A,C,G,T})\n");
	fprintf(kmerFile,"# The genome sequence you are looking for is a path in this maze\n");
	fprintf(kmerFile,"# You can kickstart your assembly algorithm development by loading this file\n");
	fprintf(kmerFile,"# Note that vertices with a coverage of 1 are not considered.\n");
	fprintf(kmerFile,"# Format:\n");
	if(m_parameters->getColorSpaceMode())
		fprintf(kmerFile,"# k-mer color sequence; coverage value; first color of parents; last color of children\n");
	else
		fprintf(kmerFile,"# k-mer nucleotide sequence; coverage value; first nucleotide of parents; last nucleotide of children\n");
	if(m_parameters->getColorSpaceMode()){
		fprintf(kmerFile,"# Example in color space:\n# 0312;10;3 2;1 0\n#  0312 has a coverage value of 10\n");
		fprintf(kmerFile,"#  Ingoing arcs: 3031 -> 0312 and 2031 -> 0312\n");
		fprintf(kmerFile,"#  Outgoing arcs: 0312 -> 3121 and 0312 -> 3120\n");
	}else{
		fprintf(kmerFile,"# Example in nucleotide space:\n# ATCG;10;T G;C A\n#  ATCG has a coverage value of 10\n");
		fprintf(kmerFile,"#  Ingoing arcs: TATC -> ATCG and GATC -> ATCG\n");
		fprintf(kmerFile,"#  Outgoing arcs: ATCG -> TCGC and ATCG -> TCGA\n");
	}

}

void CoverageGatherer::registerPlugin(ComputeCore*core){
	m_core=core;
	PluginHandle plugin=core->allocatePluginHandle();
	m_plugin=plugin;

	core->setPluginName(plugin,"CoverageGatherer");
	core->setPluginDescription(plugin,"Sends coverage depth frequencies");
	core->setPluginAuthors(plugin,"Sébastien Boisvert");
	core->setPluginLicense(plugin,"GNU General Public License version 3");

	__ConfigureSlaveModeHandler(CoverageGatherer, RAY_SLAVE_MODE_SEND_DISTRIBUTION);

	RAY_MPI_TAG_COVERAGE_DATA_REPLY=core->allocateMessageTagHandle(plugin);
	core->setMessageTagSymbol(plugin,RAY_MPI_TAG_COVERAGE_DATA_REPLY,"RAY_MPI_TAG_COVERAGE_DATA_REPLY");

	RAY_MPI_TAG_GET_COVERAGE_AND_DIRECTION_REPLY=core->allocateMessageTagHandle(plugin);
	core->setMessageTagSymbol(plugin,RAY_MPI_TAG_GET_COVERAGE_AND_DIRECTION_REPLY,"RAY_MPI_TAG_GET_COVERAGE_AND_DIRECTION_REPLY");

	RAY_MPI_TAG_GET_COVERAGE_AND_MARK_REPLY=core->allocateMessageTagHandle(plugin);
	core->setMessageTagSymbol(plugin,RAY_MPI_TAG_GET_COVERAGE_AND_MARK_REPLY,"RAY_MPI_TAG_GET_COVERAGE_AND_MARK_REPLY");

	RAY_MPI_TAG_GET_COVERAGE_AND_PATHS_REPLY=core->allocateMessageTagHandle(plugin);
	core->setMessageTagSymbol(plugin,RAY_MPI_TAG_GET_COVERAGE_AND_PATHS_REPLY,"RAY_MPI_TAG_GET_COVERAGE_AND_PATHS_REPLY");

	__BindPlugin(CoverageGatherer);
}

void CoverageGatherer::resolveSymbols(ComputeCore*core){
	RAY_SLAVE_MODE_SEND_DISTRIBUTION=core->getSlaveModeFromSymbol(m_plugin,"RAY_SLAVE_MODE_SEND_DISTRIBUTION");
	RAY_SLAVE_MODE_DO_NOTHING=core->getSlaveModeFromSymbol(m_plugin,"RAY_SLAVE_MODE_DO_NOTHING");

	RAY_MPI_TAG_COVERAGE_DATA=core->getMessageTagFromSymbol(m_plugin,"RAY_MPI_TAG_COVERAGE_DATA");
	RAY_MPI_TAG_COVERAGE_DATA_REPLY=core->getMessageTagFromSymbol(m_plugin,"RAY_MPI_TAG_COVERAGE_DATA_REPLY");
	RAY_MPI_TAG_COVERAGE_END=core->getMessageTagFromSymbol(m_plugin,"RAY_MPI_TAG_COVERAGE_END");

	RAY_MPI_TAG_COVERAGE_DATA_REPLY=core->getMessageTagFromSymbol(m_plugin,"RAY_MPI_TAG_COVERAGE_DATA_REPLY");
	RAY_MPI_TAG_GET_COVERAGE_AND_DIRECTION_REPLY=core->getMessageTagFromSymbol(m_plugin,"RAY_MPI_TAG_GET_COVERAGE_AND_DIRECTION_REPLY");
	RAY_MPI_TAG_GET_COVERAGE_AND_MARK_REPLY=core->getMessageTagFromSymbol(m_plugin,"RAY_MPI_TAG_GET_COVERAGE_AND_MARK_REPLY");
	RAY_MPI_TAG_GET_COVERAGE_AND_PATHS_REPLY=core->getMessageTagFromSymbol(m_plugin,"RAY_MPI_TAG_GET_COVERAGE_AND_PATHS_REPLY");
}
