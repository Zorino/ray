/*
    Ray -- Parallel genome assemblies for parallel DNA sequencing
    Copyright (C) 2010, 2011, 2012, 2013 Sébastien Boisvert
    Copyright (C) 2013 Charles Joly Beauparlant

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

#ifndef _MachineHelper_h
#define _MachineHelper_h

#include <code/plugin_Mock/Parameters.h>
#include <code/plugin_FusionData/FusionData.h>
#include <code/plugin_NetworkTest/NetworkTest.h>
#include <code/plugin_Example/Example.h>
#include <code/plugin_Partitioner/Partitioner.h>
#include <code/plugin_SequencesLoader/SequencesLoader.h>
#include <code/plugin_SeedingData/SeedingData.h>
#include <code/plugin_Scaffolder/Scaffolder.h>
#include <code/plugin_SeedExtender/SeedExtender.h>
#include <code/plugin_SeedExtender/ExtensionData.h>
#include <code/plugin_SeedExtender/OpenAssemblerChooser.h>
#include <code/plugin_SeedExtender/BubbleData.h>
#include <code/plugin_Searcher/Searcher.h>
#include <code/plugin_SequencesLoader/ArrayOfReads.h>
#include <code/plugin_VerticesExtractor/VerticesExtractor.h>
#include <code/plugin_EdgePurger/EdgePurger.h>
#include <code/plugin_VerticesExtractor/GridTable.h>
#include <code/plugin_KmerAcademyBuilder/KmerAcademyBuilder.h>
#include <code/plugin_CoverageGatherer/CoverageGatherer.h>
#include <code/plugin_SequencesIndexer/SequencesIndexer.h>

#include <RayPlatform/core/ComputeCore.h>
#include <RayPlatform/communication/VirtualCommunicator.h>
#include <RayPlatform/memory/RingAllocator.h>
#include <RayPlatform/structures/StaticVector.h>
#include <RayPlatform/scheduling/SwitchMan.h>
#include <RayPlatform/handlers/MasterModeHandler.h>
#include <RayPlatform/handlers/SlaveModeHandler.h>
#include <RayPlatform/profiling/TimePrinter.h>
#include <RayPlatform/profiling/Profiler.h>

#include <stdint.h>
#include <map>
using namespace std;

__DeclarePlugin(MachineHelper);

__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_LOAD_CONFIG);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_SEND_COVERAGE_VALUES);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_WRITE_KMERS);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_LOAD_SEQUENCES);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_TRIGGER_VERTICE_DISTRIBUTION);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_TRIGGER_GRAPH_BUILDING);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_PURGE_NULL_EDGES);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_TRIGGER_INDEXING);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_PREPARE_DISTRIBUTIONS);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_PREPARE_DISTRIBUTIONS_WITH_ANSWERS);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_PREPARE_SEEDING);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_TRIGGER_SEEDING);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_TRIGGER_DETECTION);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_ASK_DISTANCES);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_START_UPDATING_DISTANCES);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_TRIGGER_EXTENSIONS);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_TRIGGER_FUSIONS);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_TRIGGER_FIRST_FUSIONS);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_START_FUSION_CYCLE);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_ASK_EXTENSIONS);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_SCAFFOLDER);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_KILL_RANKS);
__DeclareMasterModeAdapter(MachineHelper,RAY_MASTER_MODE_KILL_ALL_MPI_RANKS);

__DeclareSlaveModeAdapter(MachineHelper,RAY_SLAVE_MODE_WRITE_KMERS);
__DeclareSlaveModeAdapter(MachineHelper,RAY_SLAVE_MODE_ASSEMBLE_WAVES);
__DeclareSlaveModeAdapter(MachineHelper,RAY_SLAVE_MODE_SEND_EXTENSION_DATA);
__DeclareSlaveModeAdapter(MachineHelper,RAY_SLAVE_MODE_DIE);

__DeclareMessageTagAdapter(MachineHelper,RAY_MPI_TAG_NOTIFY_ERROR);

/** 
 * This file contains __legacy code__
 * Old handlers are here.
 * TODO: move them elsewhere ?
 * \author Sébastien Boisvert */
class MachineHelper: public CorePlugin{

	__AddAdapter(MachineHelper,RAY_MASTER_MODE_LOAD_CONFIG);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_SEND_COVERAGE_VALUES);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_WRITE_KMERS);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_LOAD_SEQUENCES);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_TRIGGER_VERTICE_DISTRIBUTION);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_TRIGGER_GRAPH_BUILDING);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_PURGE_NULL_EDGES);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_TRIGGER_INDEXING);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_PREPARE_DISTRIBUTIONS);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_PREPARE_DISTRIBUTIONS_WITH_ANSWERS);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_PREPARE_SEEDING);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_TRIGGER_SEEDING);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_TRIGGER_DETECTION);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_ASK_DISTANCES);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_START_UPDATING_DISTANCES);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_TRIGGER_EXTENSIONS);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_TRIGGER_FUSIONS);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_TRIGGER_FIRST_FUSIONS);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_START_FUSION_CYCLE);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_ASK_EXTENSIONS);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_SCAFFOLDER);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_KILL_RANKS);
	__AddAdapter(MachineHelper,RAY_MASTER_MODE_KILL_ALL_MPI_RANKS);

	__AddAdapter(MachineHelper,RAY_SLAVE_MODE_WRITE_KMERS);
	__AddAdapter(MachineHelper,RAY_SLAVE_MODE_ASSEMBLE_WAVES);
	__AddAdapter(MachineHelper,RAY_SLAVE_MODE_SEND_EXTENSION_DATA);
	__AddAdapter(MachineHelper,RAY_SLAVE_MODE_DIE);

	__AddAdapter(MachineHelper,RAY_MPI_TAG_NOTIFY_ERROR);

	MessageTag RAY_MPI_TAG_NOTIFY_ERROR;
	MessageTag RAY_MPI_TAG_FINISH_FUSIONS;
	MessageTag RAY_MPI_TAG_GET_CONTIG_CHUNK;
	MessageTag RAY_MPI_TAG_GET_CONTIG_CHUNK_REPLY;
	MessageTag RAY_MPI_TAG_GET_COVERAGE_AND_DIRECTION;
	MessageTag RAY_MPI_TAG_GET_COVERAGE_AND_DIRECTION_REPLY;
	MessageTag RAY_MPI_TAG_GET_COVERAGE_AND_PATHS;
	MessageTag RAY_MPI_TAG_GET_COVERAGE_AND_PATHS_REPLY;
	MessageTag RAY_MPI_TAG_GET_PATH_LENGTH;
	MessageTag RAY_MPI_TAG_GET_PATH_LENGTH_REPLY;
	MessageTag RAY_MPI_TAG_GET_PATH_VERTEX;
	MessageTag RAY_MPI_TAG_GET_PATH_VERTEX_REPLY;
	MessageTag RAY_MPI_TAG_GET_READ_MARKERS;
	MessageTag RAY_MPI_TAG_GET_READ_MARKERS_REPLY;
	MessageTag RAY_MPI_TAG_GET_READ_MATE;
	MessageTag RAY_MPI_TAG_GET_READ_MATE_REPLY;
	MessageTag RAY_MPI_TAG_GET_VERTEX_EDGES_COMPACT;
	MessageTag RAY_MPI_TAG_GET_VERTEX_EDGES_COMPACT_REPLY;
	MessageTag RAY_MPI_TAG_GOOD_JOB_SEE_YOU_SOON;
	MessageTag RAY_MPI_TAG_GOOD_JOB_SEE_YOU_SOON_REPLY;
	MessageTag RAY_MPI_TAG_HAS_PAIRED_READ;
	MessageTag RAY_MPI_TAG_HAS_PAIRED_READ_REPLY;
	MessageTag RAY_MPI_TAG_LOAD_SEQUENCES;
	MessageTag RAY_MPI_TAG_PREPARE_COVERAGE_DISTRIBUTION;
	MessageTag RAY_MPI_TAG_PREPARE_COVERAGE_DISTRIBUTION_QUESTION;
	MessageTag RAY_MPI_TAG_PURGE_NULL_EDGES;
	MessageTag RAY_MPI_TAG_REQUEST_READ_SEQUENCE;
	MessageTag RAY_MPI_TAG_REQUEST_READ_SEQUENCE_REPLY;
	MessageTag RAY_MPI_TAG_REQUEST_SEED_LENGTHS;
	MessageTag RAY_MPI_TAG_REQUEST_VERTEX_COVERAGE;
	MessageTag RAY_MPI_TAG_REQUEST_VERTEX_COVERAGE_REPLY;
	MessageTag RAY_MPI_TAG_REQUEST_VERTEX_OUTGOING_EDGES;
	MessageTag RAY_MPI_TAG_REQUEST_VERTEX_OUTGOING_EDGES_REPLY;
	MessageTag RAY_MPI_TAG_REQUEST_VERTEX_READS;
	MessageTag RAY_MPI_TAG_REQUEST_VERTEX_READS_REPLY;
	MessageTag RAY_MPI_TAG_SCAFFOLDING_LINKS;
	MessageTag RAY_MPI_TAG_SCAFFOLDING_LINKS_REPLY;
	MessageTag RAY_MPI_TAG_SEND_COVERAGE_VALUES;
	MessageTag RAY_MPI_TAG_SEQUENCE_BIOLOGICAL_ABUNDANCES;
	MessageTag RAY_MPI_TAG_SET_WORD_SIZE;
	MessageTag RAY_MPI_TAG_START_FUSION;
	MessageTag RAY_MPI_TAG_START_INDEXING_SEQUENCES;
	MessageTag RAY_MPI_TAG_START_SCAFFOLDER;
	MessageTag RAY_MPI_TAG_START_SEEDING;
	MessageTag RAY_MPI_TAG_START_VERTICES_DISTRIBUTION;
	MessageTag RAY_MPI_TAG_TEST_NETWORK;
	MessageTag RAY_MPI_TAG_TEST_NETWORK_MESSAGE;
	MessageTag RAY_MPI_TAG_TEST_NETWORK_MESSAGE_REPLY;
	MessageTag RAY_MPI_TAG_VERTEX_INFO;
	MessageTag RAY_MPI_TAG_VERTEX_INFO_REPLY;
	MessageTag RAY_MPI_TAG_WRITE_AMOS;
	MessageTag RAY_MPI_TAG_WRITE_KMERS;
	MessageTag RAY_MPI_TAG_WRITE_KMERS_REPLY;

	MessageTag RAY_MPI_TAG_ACTIVATE_RELAY_CHECKER;
	MessageTag RAY_MPI_TAG_ACTIVATE_RELAY_CHECKER_REPLY;
	MessageTag RAY_MPI_TAG_ADD_COLORS;
	MessageTag RAY_MPI_TAG_ADD_KMER_COLOR;
	MessageTag RAY_MPI_TAG_ASK_EXTENSION;
	MessageTag RAY_MPI_TAG_ASK_EXTENSION_DATA;
	MessageTag RAY_MPI_TAG_ASK_LIBRARY_DISTANCES;
	MessageTag RAY_MPI_TAG_ASK_READ_LENGTH;
	MessageTag RAY_MPI_TAG_ASK_READ_LENGTH_REPLY;
	MessageTag RAY_MPI_TAG_ASK_VERTEX_PATH;
	MessageTag RAY_MPI_TAG_ASK_VERTEX_PATH_REPLY;
	MessageTag RAY_MPI_TAG_ASK_VERTEX_PATHS_SIZE;
	MessageTag RAY_MPI_TAG_ASK_VERTEX_PATHS_SIZE_REPLY;
	MessageTag RAY_MPI_TAG_ASSEMBLE_WAVES;
	MessageTag RAY_MPI_TAG_ASSEMBLE_WAVES_DONE;
	MessageTag RAY_MPI_TAG_ATTACH_SEQUENCE;
	MessageTag RAY_MPI_TAG_ATTACH_SEQUENCE_REPLY;
	MessageTag RAY_MPI_TAG_AUTOMATIC_DISTANCE_DETECTION;
	MessageTag RAY_MPI_TAG_BUILD_GRAPH;
	MessageTag RAY_MPI_TAG_CLEAR_DIRECTIONS;
	MessageTag RAY_MPI_TAG_CONTIG_ABUNDANCE;
	MessageTag RAY_MPI_TAG_CONTIG_ABUNDANCE_REPLY;
	MessageTag RAY_MPI_TAG_CONTIG_BIOLOGICAL_ABUNDANCES;
	MessageTag RAY_MPI_TAG_CONTIG_INFO;
	MessageTag RAY_MPI_TAG_CONTIG_INFO_REPLY;
	MessageTag RAY_MPI_TAG_COUNT_FILE_ENTRIES;
	MessageTag RAY_MPI_TAG_COUNT_SEARCH_ELEMENTS;
	MessageTag RAY_MPI_TAG_DISTRIBUTE_FUSIONS;
	MessageTag RAY_MPI_TAG_EXTENSION_DATA_END;

/*
 * Stuff for sending entries in files.
 */

	int m_numberOfRanksThatReplied;
	bool m_theEntriesForFileWasSent;
	int m_fileIndex;
	bool m_startedToSendCounts;
	MessageTag RAY_MPI_TAG_SET_FILE_ENTRIES;
	MessageTag RAY_MPI_TAG_SET_FILE_ENTRIES_REPLY;

	MasterMode RAY_MASTER_MODE_ADD_COLORS;
	MasterMode RAY_MASTER_MODE_AMOS;
	MasterMode RAY_MASTER_MODE_ASK_DISTANCES;
	MasterMode RAY_MASTER_MODE_ASK_EXTENSIONS;
	MasterMode RAY_MASTER_MODE_CONTIG_BIOLOGICAL_ABUNDANCES;
	MasterMode RAY_MASTER_MODE_COUNT_FILE_ENTRIES;
	MasterMode RAY_MASTER_MODE_COUNT_SEARCH_ELEMENTS;
	MasterMode RAY_MASTER_MODE_DO_NOTHING;
	MasterMode RAY_MASTER_MODE_KILL_ALL_MPI_RANKS;
	MasterMode RAY_MASTER_MODE_KILL_RANKS;
	MasterMode RAY_MASTER_MODE_LOAD_CONFIG;
	MasterMode RAY_MASTER_MODE_LOAD_SEQUENCES;
	MasterMode RAY_MASTER_MODE_PREPARE_DISTRIBUTIONS;
	MasterMode RAY_MASTER_MODE_PREPARE_DISTRIBUTIONS_WITH_ANSWERS;
	MasterMode RAY_MASTER_MODE_PREPARE_SEEDING;
	MasterMode RAY_MASTER_MODE_PURGE_NULL_EDGES;
	MasterMode RAY_MASTER_MODE_SCAFFOLDER;
	MasterMode RAY_MASTER_MODE_SEND_COVERAGE_VALUES;
	MasterMode RAY_MASTER_MODE_SEQUENCE_BIOLOGICAL_ABUNDANCES;
	MasterMode RAY_MASTER_MODE_START_FUSION_CYCLE;
	MasterMode RAY_MASTER_MODE_START_UPDATING_DISTANCES;
	MasterMode RAY_MASTER_MODE_TEST_NETWORK;
	MasterMode RAY_MASTER_MODE_TRIGGER_DETECTION;
	MasterMode RAY_MASTER_MODE_TRIGGER_EXTENSIONS;
	MasterMode RAY_MASTER_MODE_TRIGGER_FIRST_FUSIONS;
	MasterMode RAY_MASTER_MODE_TRIGGER_FUSIONS;
	MasterMode RAY_MASTER_MODE_TRIGGER_GRAPH_BUILDING;
	MasterMode RAY_MASTER_MODE_TRIGGER_INDEXING;
	MasterMode RAY_MASTER_MODE_TRIGGER_SEEDING;
	MasterMode RAY_MASTER_MODE_TRIGGER_VERTICE_DISTRIBUTION;
	MasterMode RAY_MASTER_MODE_UPDATE_DISTANCES;
	MasterMode RAY_MASTER_MODE_WRITE_KMERS;
	MasterMode RAY_MASTER_MODE_WRITE_SCAFFOLDS;
	MasterMode RAY_MASTER_MODE_STEP_A;
	MasterMode RAY_MASTER_MODE_EVALUATE_PATHS;

	SlaveMode RAY_SLAVE_MODE_EXTENSION;
	SlaveMode RAY_SLAVE_MODE_ADD_COLORS;
	SlaveMode RAY_SLAVE_MODE_AMOS;
	SlaveMode RAY_SLAVE_MODE_ASSEMBLE_WAVES;
	SlaveMode RAY_SLAVE_MODE_AUTOMATIC_DISTANCE_DETECTION;
	SlaveMode RAY_SLAVE_MODE_ADD_VERTICES;
	SlaveMode RAY_SLAVE_MODE_CONTIG_BIOLOGICAL_ABUNDANCES;
	SlaveMode RAY_SLAVE_MODE_COUNT_FILE_ENTRIES;
	SlaveMode RAY_SLAVE_MODE_COUNT_SEARCH_ELEMENTS;
	SlaveMode RAY_SLAVE_MODE_DIE;
	SlaveMode RAY_SLAVE_MODE_DISTRIBUTE_FUSIONS;
	SlaveMode RAY_SLAVE_MODE_DO_NOTHING;
	SlaveMode RAY_SLAVE_MODE_ADD_EDGES;
	SlaveMode RAY_SLAVE_MODE_FINISH_FUSIONS;
	SlaveMode RAY_SLAVE_MODE_FUSION;
	SlaveMode RAY_SLAVE_MODE_INDEX_SEQUENCES;
	SlaveMode RAY_SLAVE_MODE_LOAD_SEQUENCES;
	SlaveMode RAY_SLAVE_MODE_SCAFFOLDER;
	SlaveMode RAY_SLAVE_MODE_SEND_DISTRIBUTION;
	SlaveMode RAY_SLAVE_MODE_SEND_EXTENSION_DATA;
	SlaveMode RAY_SLAVE_MODE_SEND_LIBRARY_DISTANCES;
	SlaveMode RAY_SLAVE_MODE_SEND_SEED_LENGTHS;
	SlaveMode RAY_SLAVE_MODE_SEQUENCE_BIOLOGICAL_ABUNDANCES;
	SlaveMode RAY_SLAVE_MODE_START_SEEDING;
	SlaveMode RAY_SLAVE_MODE_TEST_NETWORK;
	SlaveMode RAY_SLAVE_MODE_WRITE_KMERS;

	bool m_oldDirectoryExists;

	SequencesLoader*m_sl;
	time_t*m_lastTime;
	bool*m_writeKmerInitialised;
	Partitioner*m_partitioner;
	map<int,map<int,LargeCount> > m_edgeDistribution;

	VirtualCommunicator*m_virtualCommunicator;
	KmerAcademyBuilder*m_kmerAcademyBuilder;

	int*m_mode_send_vertices_sequence_id;
	CoverageGatherer*m_coverageGatherer;
	GridTable*m_subgraph;
	SequencesIndexer*m_si;

	ArrayOfReads*m_myReads;
	int*m_last_value;
	VerticesExtractor*m_verticesExtractor;
	EdgePurger*m_edgePurger;

	int m_coverageRank;
	Searcher*m_searcher;

	int*m_numberOfRanksDoneSeeding;
	int*m_numberOfRanksDoneDetectingDistances;
	int*m_numberOfRanksDoneSendingDistances;
	bool m_loadSequenceStep;

	bool m_cycleStarted;
	int*m_CLEAR_n;
	int*m_DISTRIBUTE_n;
	int*m_FINISH_n;
	OpenAssemblerChooser*m_oa;
	bool*m_isFinalFusion;
	BubbleData*m_bubbleData;
	bool*m_alive;
	TimePrinter*m_timePrinter;
	SeedExtender*m_seedExtender;
	Scaffolder*m_scaffolder;

	bool m_coverageInitialised;
	int m_currentCycleStep;
	int m_cycleNumber;
	ExtensionData*m_ed;
	FusionData*m_fusionData;
	Profiler*m_profiler;
	NetworkTest*m_networkTest;
	SeedingData*m_seedingData;

	bool m_mustStop;
	bool*m_reductionOccured;
	/** indicator of the killer initialization */
	bool m_initialisedKiller;

	int m_machineRank;
	int m_numberOfRanksDone;

	int*m_numberOfMachinesDoneSendingVertices;
	bool*m_initialisedAcademy;
	int*m_repeatedLength;
	int*m_readyToSeed;
	int*m_ranksDoneAttachingReads;
	// SEQUENCE DISTRIBUTION
	bool m_reverseComplementVertex;

	int m_argc;
	char**m_argv;
	Parameters*m_parameters;
	SwitchMan*m_switchMan;
	RingAllocator*m_outboxAllocator;
	StaticVector*m_outbox;
	StaticVector*m_inbox;
	bool*m_aborted;
	map<CoverageDepth,LargeCount>*m_coverageDistribution;
	int*m_numberOfMachinesDoneSendingCoverage;
	int*m_numberOfRanksWithCoverageData;

	int getRank();
	int getSize();
	void performAssemblyWorkflow(ComputeCore*core);

	void writeCheckpointForContigPaths();

public:
	void constructor(int argc,char**argv,Parameters*parameters,
		SwitchMan*switchMan,RingAllocator*outboxAllocator,
		StaticVector*outbox,bool*aborted,
	map<CoverageDepth,LargeCount>*coverageDistribution,
	int*numberOfMachinesDoneSendingCoverage,int*numberOfRanksWithCoverageData,
bool*reductionOccured,ExtensionData*ed,FusionData*fusionData,
Profiler*p,NetworkTest*nt,SeedingData*sd,
TimePrinter*timePrinter,SeedExtender*seedExtender,Scaffolder*scaffolder,
StaticVector*inbox,	OpenAssemblerChooser*oa,	bool*isFinalFusion,	BubbleData*bubbleData, bool*alive,
 int*CLEAR_n,int*DISTRIBUTE_n,int*FINISH_n,Searcher*searcher,
	int*numberOfRanksDoneSeeding,	int*numberOfRanksDoneDetectingDistances,	int*numberOfRanksDoneSendingDistances,
	ArrayOfReads*myReads,	int*last_value,	VerticesExtractor*verticesExtractor,	EdgePurger*edgePurger,
int*mode_send_vertices_sequence_id,CoverageGatherer*coverageGatherer,GridTable*m_subgraph,SequencesIndexer*m_si,
VirtualCommunicator*virtualCommunicator,KmerAcademyBuilder*kmerAcademyBuilder,
	int*numberOfMachinesDoneSendingVertices,
	bool*initialisedAcademy,
	int*repeatedLength,
	int*readyToSeed,
	int*ranksDoneAttachingReads,
SequencesLoader*sl,time_t*lastTime,bool*writeKmerInitialised,Partitioner*partitioner
);

	void call_RAY_MASTER_MODE_LOAD_CONFIG();
	void call_RAY_MASTER_MODE_SEND_COVERAGE_VALUES();
	void call_RAY_MASTER_MODE_WRITE_KMERS();
	void call_RAY_MASTER_MODE_LOAD_SEQUENCES();
	void call_RAY_MASTER_MODE_TRIGGER_VERTICE_DISTRIBUTION();
	void call_RAY_MASTER_MODE_TRIGGER_GRAPH_BUILDING();
	void call_RAY_MASTER_MODE_PURGE_NULL_EDGES();
	void call_RAY_MASTER_MODE_TRIGGER_INDEXING();
	void call_RAY_MASTER_MODE_PREPARE_DISTRIBUTIONS();
	void call_RAY_MASTER_MODE_PREPARE_DISTRIBUTIONS_WITH_ANSWERS();
	void call_RAY_MASTER_MODE_PREPARE_SEEDING();
	void call_RAY_MASTER_MODE_TRIGGER_SEEDING();
	void call_RAY_MASTER_MODE_TRIGGER_DETECTION();
	void call_RAY_MASTER_MODE_ASK_DISTANCES();
	void call_RAY_MASTER_MODE_START_UPDATING_DISTANCES();
	void call_RAY_MASTER_MODE_TRIGGER_EXTENSIONS();
	void call_RAY_MASTER_MODE_TRIGGER_FUSIONS();
	void call_RAY_MASTER_MODE_TRIGGER_FIRST_FUSIONS();
	void call_RAY_MASTER_MODE_START_FUSION_CYCLE();
	void call_RAY_MASTER_MODE_ASK_EXTENSIONS();
	void call_RAY_MASTER_MODE_SCAFFOLDER();
	void call_RAY_MASTER_MODE_KILL_RANKS();
	void call_RAY_MASTER_MODE_KILL_ALL_MPI_RANKS();

	void call_RAY_SLAVE_MODE_WRITE_KMERS();
	void call_RAY_SLAVE_MODE_ASSEMBLE_WAVES();
	void call_RAY_SLAVE_MODE_SEND_EXTENSION_DATA();
	void call_RAY_SLAVE_MODE_DIE();

	void call_RAY_MPI_TAG_NOTIFY_ERROR(Message*message);

	void notifyThatOldDirectoryExists();

	void registerPlugin(ComputeCore*core);
	void resolveSymbols(ComputeCore*core);
};

#endif


