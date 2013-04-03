/*
    Ray -- Parallel genome assemblies for parallel DNA sequencing
    Copyright (C) 2011, 2012, 2013 Sébastien Boisvert

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

#ifndef _Scaffolder_h
#define _Scaffolder_h

#include "ScaffoldingLink.h"
#include "SummarizedLink.h"

#include <code/SeedExtender/ReadFetcher.h>
#include <code/Mock/Parameters.h>
#include <code/Mock/constants.h>
#include <code/SeedingData/GraphPath.h>

#include <RayPlatform/structures/StaticVector.h>
#include <RayPlatform/memory/RingAllocator.h>
#include <RayPlatform/communication/VirtualCommunicator.h>
#include <RayPlatform/scheduling/SwitchMan.h>
#include <RayPlatform/profiling/TimePrinter.h>
#include <RayPlatform/handlers/SlaveModeHandler.h>
#include <RayPlatform/handlers/MasterModeHandler.h>
#include <RayPlatform/plugins/CorePlugin.h>

#include <vector>
#include <sstream>
using namespace std;

__DeclarePlugin(Scaffolder);

__DeclareMasterModeAdapter(Scaffolder,RAY_MASTER_MODE_WRITE_SCAFFOLDS);
__DeclareSlaveModeAdapter(Scaffolder,RAY_SLAVE_MODE_SCAFFOLDER);
__DeclareMessageTagAdapter(Scaffolder,RAY_MPI_TAG_GET_CONTIG_CHUNK);
__DeclareMessageTagAdapter(Scaffolder,RAY_MPI_TAG_GET_CONTIG_PACKED_CHUNK);

/**
 * Scaffolder class, it uses MPI through the virtual communicator.
 *
 * \author Sébastien Boisvert
 */
class Scaffolder :  public CorePlugin{

	__AddAdapter(Scaffolder,RAY_MASTER_MODE_WRITE_SCAFFOLDS);
	__AddAdapter(Scaffolder,RAY_SLAVE_MODE_SCAFFOLDER);
	__AddAdapter(Scaffolder,RAY_MPI_TAG_GET_CONTIG_CHUNK);
	__AddAdapter(Scaffolder,RAY_MPI_TAG_GET_CONTIG_PACKED_CHUNK);

	ostringstream m_operationBuffer;

	MessageTag RAY_MPI_TAG_SCAFFOLDING_LINKS_REPLY;
	MessageTag RAY_MPI_TAG_START_SCAFFOLDER;

	MessageTag RAY_MPI_TAG_REQUEST_VERTEX_READS;
	MessageTag RAY_MPI_TAG_CONTIG_INFO;
	MessageTag RAY_MPI_TAG_GET_CONTIG_CHUNK;
	MessageTag RAY_MPI_TAG_GET_CONTIG_CHUNK_REPLY;
	MessageTag RAY_MPI_TAG_GET_CONTIG_PACKED_CHUNK;
	MessageTag RAY_MPI_TAG_GET_CONTIG_PACKED_CHUNK_REPLY;
	MessageTag RAY_MPI_TAG_GET_COVERAGE_AND_DIRECTION;
	MessageTag RAY_MPI_TAG_GET_PATH_LENGTH;
	MessageTag RAY_MPI_TAG_GET_READ_MARKERS;
	MessageTag RAY_MPI_TAG_GET_READ_MATE;
	MessageTag RAY_MPI_TAG_GET_VERTEX_EDGES_COMPACT;
	MessageTag RAY_MPI_TAG_HAS_PAIRED_READ;
	MessageTag RAY_MPI_TAG_I_FINISHED_SCAFFOLDING;
	MessageTag RAY_MPI_TAG_SCAFFOLDING_LINKS;

	MasterMode RAY_MASTER_MODE_WRITE_SCAFFOLDS;
	MasterMode RAY_MASTER_MODE_CONTIG_BIOLOGICAL_ABUNDANCES;
	MasterMode RAY_MASTER_MODE_COUNT_SEARCH_ELEMENTS;

	SlaveMode RAY_SLAVE_MODE_DO_NOTHING;
	SlaveMode RAY_SLAVE_MODE_SCAFFOLDER;

	bool m_coverageWasComputedWithJustice;
	int m_skippedRepeatedObjects;

	SwitchMan*m_switchMan;
	
	TimePrinter*m_timePrinter;

/**
 * Added to skip scaffolding in case of unpaired reads
 */
	bool m_skipScaffolding;    

	int m_rankIdForContig;
	bool m_hasContigSequence_Initialised;
	ofstream m_fp;
	bool m_hasContigSequence;
	string m_contigSequence;
	map<PathHandle,int> m_contigLengths;
	int m_position;
	int m_theLength;
	GraphPath m_contigPath;
	bool m_requestedContigChunk;

	vector<int> m_allScaffoldLengths;
	vector<int> m_allContigLengths;

	/* coverage values for the current path */
	vector<int> m_vertexCoverageValues;

	int m_scaffoldId;
	int m_positionOnScaffold;
	bool m_writeContigRequested;

	vector<vector<PathHandle> > m_scaffoldContigs;
	vector<vector<char> >m_scaffoldStrands;
	vector<vector<int> >m_scaffoldGaps;

	bool m_sentContigInfo;
	bool m_sentContigMeta;
	vector<PathHandle> m_masterContigs;
	vector<int> m_masterLengths;
	vector<SummarizedLink>m_masterLinks;
	int m_summaryIterator;
	bool m_summarySent;
	vector<SummarizedLink>m_summary;
	bool m_summaryPerformed;
	bool m_entrySent;
	map<PathHandle,map<char,map<PathHandle,map<char,vector<ScaffoldingLink> > > > > m_scaffoldingSummary;
	bool m_reverseDirectionsReceived;
	bool m_reverseDirectionLengthReceived;
	PathHandle m_pairedReverseDirectionName;
	int m_pairedReverseDirectionPosition;
	CoverageDepth m_pairedReverseMarkerCoverage;
	bool m_pairedReverseHasDirection;
	bool m_reverseDirectionLengthRequested;
	int m_pairedReverseDirectionLength;
	// these are mostly flags for sends/receives
	bool m_forwardDirectionLengthRequested;
	bool m_forwardDirectionLengthReceived;
	int m_pairedForwardDirectionLength;
	bool m_forwardDirectionsRequested;
	bool m_forwardDirectionsReceived;
	CoverageDepth m_pairedForwardMarkerCoverage;
	bool m_pairedForwardHasDirection;
	PathHandle m_pairedForwardDirectionName;
	int m_pairedForwardDirectionPosition;
	bool m_reverseDirectionsRequested;
	int m_readLength;
	bool m_markersRequested;
	bool m_markersReceived;
	int m_pairedForwardOffset;
	int m_pairedReverseOffset;
	Kmer m_pairedForwardMarker;
	Kmer m_pairedReverseMarker;
	bool m_pairReceived;
	int m_pairedReadRank;
	int m_pairedReadIndex;
	int m_pairedReadLibrary;
	int m_pairedReadLength;
	bool m_pairRequested;
	bool m_hasPairReceived;
	bool m_hasPairRequested;
	bool m_hasPair;
	bool m_initialisedFetcher;
	int m_readAnnotationId;
	ReadFetcher m_readFetcher;
	vector<WorkerHandle> m_activeWorkers;
	WorkerHandle m_workerId;
	VirtualCommunicator*m_virtualCommunicator;
	bool m_coverageRequested;
	bool m_coverageReceived;
	CoverageDepth m_receivedCoverage;

	bool m_reverseDone;
	bool m_forwardDone;
	int m_contigId;
	int m_positionOnContig;
	vector<GraphPath>*m_contigs;
	vector<PathHandle>*m_contigNames;

	Parameters*m_parameters;
	StaticVector*m_inbox;
	StaticVector*m_outbox;
	RingAllocator*m_outboxAllocator;
	bool m_ready;

/**
 *	gets a contig sequence by receiving several MPI messages
 */

	map<PathHandle,int>*m_contigNameIndex;

	void getContigSequence(PathHandle id);
	void getContigSequenceFromKmers(PathHandle id);
	void getContigSequenceFromPackedObjects(PathHandle id);
	void processContig();
	void processContigPosition();
	void processVertex(Kmer*vertex);
	void processAnnotations();
	void processAnnotation();
	void performSummary();
	void sendSummary();
	void sendContigInfo();
	void extractScaffolds(char state,map<PathHandle,int>*colors,PathHandle vertex,
		map<PathHandle,map<char,vector<vector<PathHandle> > > >*parents,
		map<PathHandle,map<char,vector<vector<PathHandle> > > >*children,set<int>*completedColours);


	void computeStatistics(vector<int>*lengths,int minimumLength,ostream*outputStream);
	void printInStream(ostream*outputStream);

	void getCoverageOfBlockOfLife();
	
	Rank m_rank;

/*
 * Variables for fetching remote paths.
 */

	int m_theLengthInNucleotides;
	ostringstream m_contigPathBuffer;
public:

	bool m_initialised;
/**
 *	Number of ranks that have finished scaffolding
 */
	int m_numberOfRanksFinished;
	
/**
 *	Constructor of the scaffolder
 */
	void constructor(StaticVector*outbox,StaticVector*inbox,RingAllocator*outboxAllocator,Parameters*parameters,
		VirtualCommunicator*vc,SwitchMan*switchMan);
	void setContigPaths(vector<PathHandle>*names,vector<GraphPath>*paths);
	void addMasterLink(SummarizedLink*link);
	void solve();
	void addMasterContig(PathHandle name,int length);

	void call_RAY_MASTER_MODE_WRITE_SCAFFOLDS();

	void call_RAY_SLAVE_MODE_SCAFFOLDER();

	void call_RAY_MPI_TAG_GET_CONTIG_CHUNK(Message*message);
	void call_RAY_MPI_TAG_GET_CONTIG_PACKED_CHUNK(Message*message);

	void printFinalMessage();

	void setTimePrinter(TimePrinter*a);

	void registerPlugin(ComputeCore*core);
	void resolveSymbols(ComputeCore*core);
};

#endif
