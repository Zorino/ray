/*
    Ray -- Parallel genome assemblies for parallel DNA sequencing
    Copyright (C) 2010, 2011, 2012, 2013 Sébastien Boisvert

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

#ifndef _SeedWorker
#define _SeedWorker

#include <code/plugin_SeedingData/GraphPath.h>
#include <code/plugin_Mock/Parameters.h>

#include <RayPlatform/memory/RingAllocator.h>
#include <RayPlatform/communication/VirtualCommunicator.h>
#include <RayPlatform/scheduling/Worker.h>

#include <stdint.h>
#include <vector>
using namespace std;

/*
 * Given a Kmer, SeedWorker determines if it spawns a seed.
 * If yes, it computes the seed.
 *
 * \author Sébastien Boisvert
 */
class SeedWorker : public Worker {

	MessageTag RAY_MPI_TAG_GET_VERTEX_EDGES_COMPACT;
	MessageTag RAY_MPI_TAG_REQUEST_VERTEX_COVERAGE;

	int m_mainVertexCoverage;

	bool m_hasDeadEnd;
	bool m_debugSeeds;

	bool m_elongationMode;

	map<Kmer,int> m_cache;
	WorkerHandle m_workerIdentifier;
	bool m_finished;
	Kmer m_SEEDING_currentChildVertex;
	bool m_SEEDING_InedgesReceived;
	Kmer m_SEEDING_currentParentVertex;
	bool m_SEEDING_ingoingEdgesDone;
	int m_SEEDING_numberOfOutgoingEdgesWithSeedCoverage;
	Kmer m_SEEDING_currentVertex;
	int m_SEEDING_numberOfIngoingEdges;
	bool m_SEEDING_vertexCoverageRequested;
	bool m_SEEDING_edgesReceived;
	bool m_SEEDING_testInitiated;
	int m_SEEDING_ingoingEdgeIndex;
	int m_SEEDING_outgoingEdgeIndex;
	vector<Kmer> m_SEEDING_receivedIngoingEdges;
	vector<int> m_ingoingCoverages;
	vector<int> m_outgoingCoverages;
	Parameters*m_parameters;
	bool m_SEEDING_vertexCoverageReceived;
	vector<Kmer> m_SEEDING_receivedOutgoingEdges;
	int m_SEEDING_numberOfOutgoingEdges;
	int m_SEEDING_numberOfIngoingEdgesWithSeedCoverage;
	bool m_SEEDING_outgoingEdgesDone;
	bool m_SEEDING_InedgesRequested;
	bool m_outgoingEdgesReceived;
	bool m_SEEDING_edgesRequested;
	bool m_ingoingEdgesReceived;
	int m_wordSize;

	GraphPath m_SEEDING_seed;

	bool m_SEEDING_firstVertexParentTestDone;	

/*
 * Store visited vertices.
 */
	set<Kmer> m_SEEDING_vertices;
	Kmer m_SEEDING_first;
	bool m_SEEDING_firstVertexTestDone;
	int m_SEEDING_numberOfSeedCoverageCandidates;
	int m_rank;
	int m_size;
	RingAllocator*m_outboxAllocator;
	int m_SEEDING_receivedVertexCoverage;
	bool m_SEEDING_1_1_test_result;
	int getRank();
	int getSize();
	bool m_SEEDING_1_1_test_done;
	VirtualCommunicator*m_virtualCommunicator;

/*
 * Additional quality control tests.
 */
	bool m_checkedHead;
	bool m_checkedTail;
	bool m_endChecksModeStarted;
	bool m_endChecksMode;
	bool m_vertexFetcherStarted;
	vector<Kmer> m_vertexFetcherParents;
	vector<Kmer> m_vertexFetcherChildren;
	CoverageDepth m_vertexFetcherCoverage;
	bool m_vertexFetcherReceivedData;
	bool m_vertexFetcherRequestedData;
	bool m_headIsDeadEnd;
	bool m_tailIsDeadEnd;

// topological algorithm for graphs
	vector<Kmer> m_verticesBefore;
	vector<Kmer> m_verticesAfter;

	void performChecksOnPathEnds();
	bool fetchVertexData(Kmer*kmer);

	bool getPathBefore(Kmer*kmer,int depth);
	bool getPathAfter(Kmer*kmer,int depth);

public:
	void constructor(Kmer*vertex,Parameters*parameters,RingAllocator*outboxAllocator,
		VirtualCommunicator*vc,WorkerHandle workerId,

	MessageTag RAY_MPI_TAG_GET_VERTEX_EDGES_COMPACT,
	MessageTag RAY_MPI_TAG_REQUEST_VERTEX_COVERAGE
);

	GraphPath*getSeed();

	void do_1_1_test();

	/** work a little bit 
	 * the class Worker provides no implementation for that 
	*/
	void work();

	/** is the worker done doing its things */
	bool isDone();

	/** get the worker number */
	WorkerHandle getWorkerIdentifier();

	bool isHeadADeadEnd();
	bool isTailADeadEnd();
	void enableDebugMode();
	bool isBubbleWeakComponent();
};

#endif
