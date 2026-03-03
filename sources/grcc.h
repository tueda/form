#pragma once

/**
 * @file grcc.h
 * @brief Public declarations for GRCC graph generation.
 * @details Declares model/process/topology/assignment data structures and
 * callback-oriented entry points used by FORM and standalone GRCC builds.
 */

/* #[ License : */
/*
 *   Copyright (C) 2023-2026 T. Kaneko
 *   When using this file you are requested to refer to the publication
 *   Comput.Phys.Commun. 92 (1995) 127-152
 *
 *   This file is part of FORM.
 *
 *   FORM is free software: you can redistribute it and/or modify it under the
 *   terms of the GNU General Public License as published by the Free Software
 *   Foundation, either version 3 of the License, or (at your option) any later
 *   version.
 *
 *   FORM is distributed in the hope that it will be useful, but WITHOUT ANY
 *   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *   details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with FORM.  If not, see <http://www.gnu.org/licenses/>.
 */
/* #] License : */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CHECK

/**
 * @brief Object model for GRCC graph generation.
 * @details Pipeline overview:
 * Model/Process -> SProcess -> MGraph (topology) -> EGraph (edge objects)
 * -> Assign (particle/interaction assignment) -> Output/callbacks.
 */
//==============================================================
extern "C" {
#include "grccparam.h"
}

//==============================================================
// Name space of this library
// See 'grccparam.h' for #define
#ifdef GRCC_NAMESPACE
namespace Grcc {
#endif

//==============================================================
// Common types
//
#define True   1
#define False  0
typedef int Bool;

//==============================================================
// Big integer (may be replaced by suitable one)

#define BigInt       long
#define ToBigInt(x)  ((BigInt) (x))

//==============================================================
// Macro functions
#define Real(x)      ((double) (x))
#define Abs(x)       (((x) >= 0)? (x): (-x))
#define Sign(x)      (((x) >= 0)? (1): (-1))
#define Max(x, y)    ((x) > (y) ? (x) : (y))
#define Min(x, y)    ((x) < (y) ? (x) : (y))
#define Second(t) ((double)(*(t)=(double)(clock()/((double)CLOCKS_PER_SEC))))
#define isATExternal(x) ((x)==GRCC_AT_Initial||(x)==GRCC_AT_Final||(x)==GRCC_AT_External)

//==============================================================
// types and classes
typedef int  Edge2n[2];   // pair of nodes expressing an edge
class Assign;
class AStack;
class EEdge;
class EGraph;
class ENode;
class Interaction;
class MGraph;
class MNodeClass;
class MCEdge;
class MCOpi; 
class MCBridge;
class MCBlock;
class MConn;
class Model;
class MOrbits;
class Options;
class Output;
class Particle;
class PNodeClass;
class Process;
class SGroup;
class SProcess;
class DCGraph;

//==============================================================
// type of output functions
typedef Bool OutEGB(EGraph *, void *);
typedef void OutEG(EGraph *, void *);
typedef void ErExit(const char *msg, void *);

/**
 * @brief Callback conventions.
 * @details
 * - OutEGB is used for MGraph-stage and end-of-MGraph hooks.
 * - OutEG is used for accepted assigned graphs.
 * - ErExit is a host-provided fatal error callback.
 */

/**
 * @defgroup grcc_api Public API
 * @brief Top-level configuration, callbacks, output, and shared numeric helpers.
 */
/**
 * @defgroup grcc_model Model and interactions
 * @brief Particle/interaction definitions and model-level lookup/classification.
 */
/**
 * @defgroup grcc_process Process decomposition
 * @brief Process splitting into constrained subprocesses and result aggregation.
 */
/**
 * @defgroup grcc_mgraph Topology generation (MGraph)
 * @brief Canonical topology construction, connectivity checks, and graph counters.
 */
/**
 * @defgroup grcc_egraph Edge-object graphs (EGraph)
 * @brief Edge/node object view with momentum bookkeeping and topology filters.
 */
/**
 * @defgroup grcc_symmetry Symmetry/permutation utilities
 * @brief Permutation-group helpers for canonical representatives and weights.
 */
/**
 * @defgroup grcc_assign Particle assignment/backtracking
 * @brief Candidate propagation, recursive assignment, and stack-based rollback.
 */

//==============================================================
/** @ingroup grcc_api */
class Options {
  public:
    
    Model      *model;
    Process    *proc;
    SProcess   *sproc;
    Output     *out;         // for output
    OutEGB     *outmg;       // call back function of mgraph
    OutEGB     *endmg;       // call back function of end of mgraph
    OutEG      *outag;       // call back function of agraph
    void       *argmg;       // additional argument to outmg
    void       *argemg;      // additional argument to endmg
    void       *argag;       // additional argument to outag

    /**
     * @note Registered callbacks and their payload pointers are non-owning
     * references managed by the caller.
     */

    //switches for graph generation
    /**
     * @brief References for both positive and complementary QG option names.
     * @details For example onepi/onepr with sign = +/-1.
     */
    OptQGRef qgref[2*GRCC_QGRAF_OPT_Size]; // array of reference to QG-options
    int      values[GRCC_OPT_Size];        // array of options
    /** @brief Tri-state QGRAF flags: +1(require), -1(require complement), 0(ignore). */
    int      qgopt[GRCC_QGRAF_OPT_Size];   // array of QGRAF options

    int      nqgopt;                       // effective length of qgref

    int          DUMMYPADDING;

    // measuring time
    double time0;
    double time1;

    //------------------
    Options(void);
    ~Options(void);
    void setDefaultValues(void);
    void setOldDefaultValues(void);

    /** @brief Set one generation option by index. */
    /** @param ind Option index (GRCC_OPT_*).
     *  @param val Option value to store. */
    void setValue(int ind, int val);
    /** @brief Get one generation option by index.
     *  @param ind Option index (GRCC_OPT_*).
     *  @return Stored option value.
     */
    int  getValue(int ind);

    /** @brief Set QGRAF-style filter flags.
     *  @param qgopt Input array of size GRCC_QGRAF_OPT_Size.
     */
    void setQGrafOpt(int *qgopt);

    void print(void);

    void setOutputF(Bool outgrf, const char *fname);
    void setOutputP(Bool outgrp, const char *fname);
    void printLevel(int l);

    void printModel(void);
    /** @brief Register callback for accepted MGraphs.
     *  @param omg Callback function pointer (non-owning).
     *  @param pt User payload passed back to callback (non-owning).
     */
    void setOutMG(OutEGB *omg, void *pt);
    /** @brief Register callback for accepted AGraphs.
     *  @param oag Callback function pointer (non-owning).
     *  @param pt User payload passed back to callback (non-owning).
     */
    void setOutAG(OutEG  *oag, void *pt);
    /** @brief Register callback at end of each MGraph branch.
     *  @param omg Callback function pointer (non-owning).
     *  @param pt User payload passed back to callback (non-owning).
     */
    void setEndMG(OutEGB *omg, void *pt);
    /** @brief Register fatal-error callback.
     *  @param ere Error callback (non-owning).
     *  @param pt User payload passed back to callback (non-owning).
     */
    void setErExit(ErExit  *ere, void *pt);
    const OptDef *getDef(void);
    const OptDef *getOldDef(void);
    const OptQGDef *getQGDef(void);

    /** @brief Mark begin of model-level generation context.
     *  @param mdl Active model.
     */
    void begin(Model *mdl);
    void end(void);
    /** @brief Mark begin of process-level generation context.
     *  @param prc Active process.
     */
    void beginProc(Process *prc);
    void endProc(void);
    /** @brief Mark begin of subprocess-level generation context.
     *  @param sprc Active subprocess.
     */
    void beginSubProc(SProcess *sprc);
    void endSubProc(void);
    /** @brief Handle one accepted topology graph.
     *  @param mgr Current MGraph.
     */
    void newMGraph(MGraph *mgr);
    /** @brief Handle one accepted assigned graph.
     *  @param egr Current EGraph.
     */
    void newAGraph(EGraph *egr);
    void outModel(void);
};

//==============================================================
/** @ingroup grcc_api */
class Output {
  public:

    Options    *opt;
    Model      *model;
    Process    *proc;
    SProcess   *sproc;
    char       *outgrf;
    FILE       *outgrfp;
    char       *outgrp;
    FILE       *outgrpp;
    int         procId;
    Bool        outproc;

    Output(Options *optn);
    ~Output(void);

    void setOutgrf(const char *fname);
    void setOutgrp(const char *fname);
    Bool outBeginF(Model *mdl, Bool pr);
    Bool outBeginP(Model *mdl, Bool pr);
    void outEndF(void);
    void outEndP(void);
    void outProcBeginF(Process *prc);
    void outProcBeginP(Process *prc);
    void outProcBegin0(int next, int couple, int loop);
    void outSProcBeginF(SProcess *sprc);
    void outSProcBeginP(SProcess *sprc);
    void outProcEndF(void);
    void outProcEndP(void);
    void outEGraphF(EGraph *egraph);
    void outEGraphP(EGraph *egraph);
    void outModelF(void);
    void outModelP(void);

};

//==============================================================
/** @ingroup grcc_api */
class Fraction {
  public:
    BigInt num, den;
    double ratio;

    Fraction() { num = 0;  den = 1; };
    Fraction(BigInt n, BigInt d);

    void   print(const char *msg);
    void   setValue(BigInt n, BigInt d);
    void   setValue(Fraction &f);
    void   add(BigInt n, BigInt d);
    void   add(Fraction f);
    void   sub(Fraction f);
    BigInt gcd(BigInt n0, BigInt n1);
    void   normal(void);
    Bool   isEq(Fraction f);
};

//**************************************************************
// Model
//==============================================================
//--------------------------------------------------------------
/** @ingroup grcc_model */
class Particle {
  public:
    Model *mdl;             // the model
    char  *name;            // the name of the particle
    char  *aname;           // the name of the anti-particle
    int    id;              // id of this particle
    int    ptype;           // the type of partice : GRCC_PT_Scalar, etc.
    int    neutral;         // True if particle==anti-particle
    int    pcode;           // Particle code
    int    acode;           // Anti-particle code
    int    cmindeg;         // min(leg of connectable interactions)
    int    cmaxdeg;         // max(leg of connectable interactions)

    int    extonly;         // can appear only as external particle

    //-----------------------------
    Particle(Model *modl, int pid, PInput *pinp);
    ~Particle(void);

    char *particleName(int p);
    int   particleCode(int p);
    char *interactionName(int p);
    char *aparticle(void);
    void  prParticle(void);
    int   isNeutral(void);
    const char *typeName(void);
    const char *typeGName(void);

};

//--------------------------------------------------------------
/** @ingroup grcc_model */
class Interaction {
  public:
    Model *mdl;           // the model
    char  *name;          // the name of the interaction
    int   *plist;        // the list of particle codes
    int   *clist;        // the orders of coupling constants
    int   *slist;        // the sorted list of particle codes

    int    id;            // id of this interaction
    int    csum;          // the total orders of coupling constants
    int    nlegs;         // the number of legs
    int    loop;          // the number of loops
    int    icode;         // user defined interaction code 

    int    nplist;        // the list of particle codes
    int    nclist;        // the orders of coupling constants
    int    nslist;        // the sorted list of particle codes

    //-----------------------------
    Interaction(Model *modl, int iid, const char* nam, int icode, int *cpl, int nlgs, int *plst, int csm, int lp);
    ~Interaction(void);

    void prInteraction(void);
};

//--------------------------------------------------------------
/** @ingroup grcc_model */
class Model {
  public:
    char         *name;        // the name of this model
    char        **cnlist;       // the list of coupling constant names
    int           ncouple;     // the number of coupling consants.

    int           nParticles;  // the number of particles
    Particle    **particles;   // the list of particles
    int           pdef;        // the def. of partcls is ended

    // list of particles and anti-p. without Undef.
    int           nallPart;     
    int           allPart[GRCC_MAXMPARTICLES2];

    // list of particles and anti-p. without ones of extloop=True
    int           nintPart;     
    int           intPart[GRCC_MAXMPARTICLES2];

    int           nInteracts;  // the number of interactions.
    Interaction **interacts;   // the list of interactions
    int           vdef;        // the definition of interaction is ended

    int           maxnlegs;    // maximum number of legs of an interaction
    int           maxcpl;      // maximum number of coupling const.
    int           maxloop;     // maximum number of loops inside an intr.

    // classification of interactions
    int          *cplgcp;      // coupling constants
    int          *cplglg;      // degree
    int          *cplgnvl;     // number of vertices
    int         **cplgvl;      // list of vertices
    int           ncplgcp;     // the number of classes

    int           defpart;     // GRCC_DEFBYNAME or GRCC_DEFBYCODE

    // methods
    Model(MInput *minp);
    ~Model(void);

    void  prModel(void);
    void  addParticle(PInput *pinp);
    void  addParticleEnd(void);
    void  addInteraction(IInput *iinp);
    void  addInteractionEnd(void);
    int   findParticleName(const char *name);
    int   findParticleCode(int pcd);
    int   findInteractionName(const char *name);
    int   findInteractionCode(int icd);
    char *particleName(int p);
    int   particleCode(int p);
    int   normalParticle(int pt);
    int   antiParticle(int pt);
    int  *allParticles(int *len);
    int   findMClass(const int cpl, const int dgr);
    void  prParticleArray(int n, int *a, const char *msg);

    static void printMInput(MInput *min);
    static void printPInput(PInput *pin);
    static void printIInput(IInput *iin);
};

//**************************************************************
// Process
//===============================================================
/** @ingroup grcc_process */
class PNodeClass {
  public:
    SProcess *sproc;
    int      *deg;       // The degree of each node
    int      *type;      // The type of the class : GRCC_AT_xxx
    int      *particle;  // The particle code of external node.
    int      *couple;    // The orders of coupling constans
    int      *cmindeg;   // min(deg of connectable vertex)
    int      *cmaxdeg;   // max(deg of connectable vertex)
    int      *count;     // The number of nodes in the class
    int      *cl2nd;     // cl2nd[class] <= nd < cl2nd[class+1] = set of nodes
    int      *nd2cl;     // nd2cl[node]  = class
    // Mapping from process-local class id to model interaction-class id.
    int      *cl2mcl;    // cl2mcl[class] = class defined in the model.
    int       nnodes;
    int       nclass;

    PNodeClass(SProcess *sprc, int nnods, int nclss, NCInput *cls);
    PNodeClass(SProcess *sprc, int nnods, int nclss, int *dgs, int *typ, int *ptcl, int *cpl, int *cnt, int *cmind, int *cmaxd);
    ~PNodeClass(void);

    void prPNodeClass(void);
    void prElem(int e);
};

//===============================================================
/** @ingroup grcc_process */
class SProcess {
  //  In sprocess the number of nodes and edges are fixed.
  //  A node is considered as 
  //    (degree, total order of coupling constants),
  //  which pair of data defines a class of nodes.
 
  public:
    Model       *model;       // model
    Process     *proc;        // mother process 
    Options     *opt;         // options
    // Fixed class specification for this subprocess; determines topology space.
    PNodeClass  *pnclass;     // list of classes (cpl and deg is determined)
    AStack      *astack;

    MGraph      *mgraph;
    EGraph      *egraph;
    Assign      *agraph;

    int         *cl2nd;       // (code of nodes in the class[c])
                              //  = [cl2nd[c], ..., cl2nd[c+1]-1]
    int         *nd2cl;       // node nd is in the class pnclass[nd2cl[nd]]
    int          nclass;      // the number of classes

    int          ninitl;      // the number of initial particles
    int          nfinal;      // the number of final particles
    int          nvert;       // the number of vertices

    int          clist[GRCC_MAXNCPLG];  // coupling constans of the process
    int          id;          // sprocess id
    int          loop;        // the number of loops
    int          nNodes;      // the number of nodes
    int          nEdges;      // the number of edges
    int          nExtern;     // the number of external particles
    int          ncouple;     // the number of coupling constants
    int          tCouple;     // the total coupling constants

    int          DUMMYPADDING;

    BigInt       mgrcount;    // count generated mgraph
    BigInt       agrcount;    // count generated agraph
    // Group size from external-state symmetrization (used in weights/output).
    BigInt       extperm;    // count generated agraph

    // the results of the graph generation
    BigInt   nMGraphs;        // the number of generated M-graphs
    BigInt   nMOPI;           // the number of 1PI M-graphs
    Fraction wMGraphs;        // the weighted sum of M-graphs
    Fraction wMOPI;           // the weighted sum of 1PI M-graphs

    BigInt   nAGraphs;        // the number of generated A-graphs
    BigInt   nAOPI;           // the number of 1PI A-graphs
    Fraction wAGraphs;        // the weighted sum of A-graphs
    Fraction wAOPI;           // the weighted sum of 1PI A-graphs

    // methods
    SProcess(Model *mdl, Process *prc, Options *opts, int sid, int *clst, int ncls, NCInput *cls);
    SProcess(Model *mdl, Process *prc, Options *opts, int sid, int *clst, int ncls, int *cdeg, int *ctyp, int *ptcl, int *cpl, int *cnum, int *cmind, int *cmaxd);

    ~SProcess(void);

    void prSProcess(void);
    /** @brief Generate all graphs for this subprocess.
     *  @return Number of generated topology graphs.
     */
    BigInt generate(void);
    /** @brief Assign particles/interactions on one topology.
     *  @param mgr Input topology graph.
     */
    void assign(MGraph *mgr);

    /** @brief Convert process classes to MGraph node-class arrays.
     *  @param[out] ctyp Class type array (length >= nclass).
     *  @param[out] cldeg Degree per class (length >= nclass).
     *  @param[out] clnum Node multiplicity per class (length >= nclass).
     *  @param[out] cmind Lower connectable-degree bound per class (length >= nclass).
     *  @param[out] cmaxd Upper connectable-degree bound per class (length >= nclass).
     *  @return Number of valid entries written to output arrays.
     */
    int toMNodeClass(int *ctyp, int *cldeg, int *clnum, int *cmind, int *cmaxd);
    /** @brief Match current MGraph classing to subprocess classing.
     *  @param mgr Input topology graph.
     *  @return Newly allocated matched PNodeClass.
      *  @note Ownership is transferred to the caller.
     */
    PNodeClass *match(MGraph *mgr);

    /** @brief End-of-topology callback from MGraph pipeline.
     *  @param mgr Current topology graph.
     */
    void endMGraph(MGraph *mgr);
    /** @brief End-of-assignment callback.
     *  @param egr Current assigned graph.
     */
    void endAGraph(EGraph *egr);

    void resultMGraph(BigInt nmgraphs, Fraction mwsum, BigInt nmopi, Fraction mwopi);
    void resultAGraph(BigInt nagraphs, Fraction awsum, BigInt naopi, Fraction awopi);

};

//===============================================================
/** @ingroup grcc_process */
class Process {
  public:
    Model       *model;            // model used for this process
    Options     *opt;              // options
    BigInt       mgrcount;         // count generated mgraph
    BigInt       agrcount;         // count generated agraph
    int         *initlPart;        // list of ids of initial particles
    int         *finalPart;        // list of ids of final particles

    int          id;               // process id
    int          ninitl;           // the number of initial particles
    int          nfinal;           // the number of final particles
    int          ctotal;           // the total number of coupling constants
    int          nExtern;          // the number of external particles
    int          loop;             // the number of external particles
    int          maxnlegs;         // the maximum possible degree of node
    int          clist[GRCC_MAXNCPLG];  // coupling constans of the process

    // table of sprocesses 
    int        nSubproc;           // the number of sprocesses
    SProcess  *sptbl[GRCC_MAXSUBPROCS]; // the table of sprocesses
    SProcess  *sproc;              // the current sprocess

    // stack for the assignment;
    AStack      *astack;

    // the number of graphs
    BigInt       ngraphs;
    BigInt       nopi;
    BigInt       wgraphs;
    BigInt       wopi;

    // the results of the graph generation
    BigInt       nMGraphs;             // the number of generated M-graphs
    BigInt       nMOPI;                // the number of 1PI M-graphs
    Fraction     wMGraphs;             // the weighted sum of M-graphs
    Fraction     wMOPI;                // the weighted sum of 1PI M-graphs

    BigInt       nAGraphs;             // the number of generated A-graphs
    BigInt       nAOPI;                // the number of 1PI A-graphs
    Fraction     wAGraphs;             // the weighted sum of A-graphs
    Fraction     wAOPI;                // the weighted sum of 1PI A-graphs

    double       sec;

    Process(int pid, Model *model, Options *opt, int nin, int *initlPart, int nfin, int *finalPart, int *coupling);
    Process(int pid, Model *model, Options *opt, FGInput *fgi);
    ~Process(void);

    void prProcess(void);
    void outProcP(FILE *fp);
    void prProcessP(const char *fname);
    void mkSProcess(void);

};

//**************************************************************
// classes for EGraph
//==============================================================
// Constants

#define GRCC_ED_Undef   0
#define GRCC_ED_Deleted 1
#define GRCC_ED_Extern  2
#define GRCC_ED_Back    3
#define GRCC_ED_Bridge  4
#define GRCC_ED_Inloop  5
#define GRCC_ED_Size    6
#define GRCC_ED_NAMES   {"Undef", "Deleted", "Extern", "Back_Edge", "Bridge", "In_Loop"}

#define GRCC_ND_Undef   0
#define GRCC_ND_Deleted 1
#define GRCC_ND_Initial 2
#define GRCC_ND_Final   3
#define GRCC_ND_CPoint  4
#define GRCC_ND_VBlock  5
#define GRCC_ND_Inblock 6
#define GRCC_ND_Size    7
#define GRCC_ND_NAMES   {"Undef", "Deleted", "Init", "Final", "CPoint", "VBlock", "In_Block"}

//--------------------------------------------------------------
/** @ingroup grcc_egraph */
class ENode {
  public:
    EGraph *egraph;
    int    *edges;           // list of edges
                             // the value is \pm [(edge index)+1]

    int     id;              // id of the enode
    int     maxdeg;          // maximum degree
    int     deg;             // degree
    int     extloop;         // loop inside this node or AT_Initial etc.
    int     ndtype;          // type of the node
    int     intrct;          // assigned interaction/particle (ext.)

    // used in EGraph::biconn()
    int    *klow; 
    int     visited;

    int    DUMMYPADDING;

    //--------------------------------
    // functions
    ENode(void);
    ENode(EGraph *egrph, int loops, int sdeg);
    ~ENode(void);
    void initAss(EGraph *egrph, int nid, int sdg);
    void setId(EGraph *egrph, const int nid);
    void copy(ENode *en);
    void setExtern(int typ, int pt);
    void setType(int typ);
    void print(void);

};

//--------------------------------------------------------------
/** @ingroup grcc_egraph */
class EEdge {
  // momentum is printed like: ("%s%d", (enode.ext)?"Q":"p", enode.momn)
  public:
    EGraph *egraph;        // egraph

    int  id;               // id
    int  ext;
    int  ptcl;             // assigned particle (agraph)
    int  deleted;          // deleted edge, if true
    int  nodes[2];         // nodes of bothsides
    int  nlegs[2];         // nodes of both side (agraph)

    // for biconn / momentum bookkeeping
    int *emom;             // external momenta
    int *lmom;             // loop momenta
    int *extMom;           // set of external momenta.

    // momentum obtain in searchME
    ULong  momset;         // set of momenta in bit string (leaf --> root)
    int    momdir;         // direction (leg=0 --> leg=1)

    Bool cut;
    int  visited;
    int  conid;            // connected component
    int  edtype;           // type
    int  opicomp;          // id of 1PI component
    int  dir;              // direction of momentum

    int      DUMMYPADDING;

    //--------------------------------
    // functions
    EEdge(void);
    EEdge(EGraph *egrph, int nedges, int nloops);
    ~EEdge(void);
    void copy(EEdge *ee);
    void setId(EGraph *egrph, const int eid);
    void print(void);

    void setType(int typ);
    void setLMom(int k, int dir);
    void setEMom(int nedges, int *extn, int dir);
};

//--------------------------------------------------------------
// type of fermion lines

typedef enum {FL_Open, FL_Closed} FLType;

//--------------------------------------------------------------
/** @ingroup grcc_egraph */
class EFLine {
  public:
    int    elist[GRCC_MAXNODES];     // list of  (\pm [(edge index)+1])
    FLType ftype;
    int    fkind;
    int    nlist;

    EFLine(void);
    void print(const char *msg);
};

//--------------------------------------------------------------
/** @ingroup grcc_egraph */
class EGraph {
  public:
    Options    *opt;          // table of options
    Model      *model;
    Process    *proc;
    SProcess   *sproc;
    MGraph     *mgraph;
    MConn      *econn;

    ENode **nodes;
    EEdge **edges;          // edges[nEdges+1]: index starts from 1

    BigInt mId;             // id of mgraph
    BigInt aId;             // id of agraph in the same mgraph
    BigInt sId;             // sequential no. of agraph
    BigInt gSubId;          // ???
    Bool   assigned;        // mgraph (False) or agraph (True)

    int    fsign;
    // Symmetry decomposition:
    //   nsym: node permutation factor, esym: edge permutation factor.
    //   nsym1 stores node symmetry before external-state symmetrization.
    BigInt nsym, esym;      // symmetry factor with symm. ext.
    BigInt nsym1;           // symmetry factor without symm. ext.
    BigInt extperm;         // the order of group of symm. ext.
    BigInt multp;           // multiplicity of graph in symm. ext.

    int    pId;

    int   sNodes;            // memory size
    int   sEdges;            // memory size
    int   sMaxdeg;           // memory size
    int   sLoops;            // memory size

    int    nNodes;
    int    nEdges;
    int    nExtern;
    int    maxdeg;            // maximum value of degree of nodes
    int    nLoops;
    int    totalc;            // total order of coupling constants

    // biconnect
    int    nopicomp;
    // opi2plp: maximum loop order observed among 2-point 1PI components.
    int    opi2plp;
    int    nopi2p;
    int    nadj2ptv;           // the no. of edges connecting 2point vertices

    int    DUMMYPADDING;

    int   *bidef;
    int   *bilow;
    int   *extMom;
    int    bconn;
    int    bicount;
    int    loopm;
    int    opiCount;

    // Fermion lines
    EFLine *flines[GRCC_MAXFLINES];
    int     nFlines;

    int    DUMMYPADDING1;

    //--------------------------
    // functions
    EGraph(int nnodes, int nedges, int mxdeg);
    ~EGraph(void);

    void copy(EGraph *eg);
    void print(void);
    void printPy(FILE *fp, long mId);
    void fromDGraph(DGraph *dg);
    void fromMGraph(MGraph *mgraph);

    Bool   optQGrafM(Options *opt);
    Bool   optQGrafA(Options *opt);
    Bool isOptE(void);

    ENode *setExtern(int n0, int pt, int ndtp);
    Bool   isExternal(int nd)  { return (nodes[nd]->extloop < 0); };
    Bool   isFermion(int nd);

    void setExtLoop(int nd, int val);
    void endSetExtLoop(void);

    int connComp(void);
    int connVisit(int nd, int ncc);

    void biconnE(void);
    void biinitE(void);
    void bisearchE(int nd, int *extlst, int *intlst, int *opiext, int *opiloop);

    int  findRoot(void);
    int  dirEdge(int n, int e);
    void extMomConsv(void);
    int  cmpMom(int *lm0, int *em0, int *lm1, int *em1);
    int  groupLMom(int *grp, int *ed2gr);

    void chkMomConsv(void);

    void prFLines(void);
    void getFLines(void);
    int  fltrace(int fk, int nd0, int *fl);
    void addFLine(const FLType ft, int fk, int nfl, int *fl);

    int  legParticle(int ed, int lg);
};

//**************************************************************
//  Symmetry group of graphs
//===============================================================
/** @ingroup grcc_symmetry */
class SGroup {
  public:
    BigInt  size;              // # of allocated elements
    BigInt  nelem;             // # of saved elements
    int   **elem;              // saved elements
    int     nnodes;            // # of nodes.
    int     neclass;           // # of classes
    int     eclass[GRCC_MAXNODES];  // table of classes
    int     cgen;              // counter for the generation
    int     csav;              // counter for the saved elements
    int     permg[GRCC_MAXNODES];   // resulting permutation
    int     perms[GRCC_MAXNODES];   // curr. elem of saved ones.

    int     pgr[GRCC_MAXNODES];  // work
    int     pgq[GRCC_MAXNODES];  // work
    int     psr[GRCC_MAXNODES];  // work
    int     psq[GRCC_MAXNODES];  // work

    //-------------------------------
    // functions

    SGroup(void);
    ~SGroup(void);

    void  print(void);
    void  newGroup(int nelm, int nclss, int *clss);
    void  clearGroup(void);
    void  delGroup(void);
    int  *genNext(void);
    void  addGroup(int *p);
    BigInt  nElem(void);
    int  *nextElem(void);
};

//**************************************************************
// MGraph
//==============================================================
/** @ingroup grcc_mgraph */
/** @brief Node descriptor used by MGraph topology generation. */
class MNode {
  public:
    int id;        // node id
    int deg;       // degree(node) = the number of legs
    int clss;      // initial class number in which the node belongs
    int extloop;
    int cmindeg;
    int cmaxdeg;

    int freelg;    // the number of free legs
    int visited;

    MNode(int id, int clss, NCInput *mgi);
    MNode(int id, int deg, int extloop, int clss, int cmind, int cmaxd);
};

/** @ingroup grcc_mgraph */
/**
 * @brief Scalar topology graph represented by an adjacency matrix.
 * @details Input is a classified set of nodes; accepted candidates are
 * forwarded to EGraph for momentum analysis and assignment.
 * @note Invariant: `adjMat` is symmetric. Diagonal entries encode twice the
 * number of self-loops on each node.
 */
class MGraph {

  public:

    // initial conditions
    Options  *opt;         // options

    // symmetry group
    SGroup   *group;       // symmetry group
    MOrbits  *orbits;      // Orbits of nodes with respect to symmetry group

    MNode **nodes;         // table of MNode object
    BigInt mId;            // process/sprocess ID
    int   *clist;          // list of initial classes
    int    pId;            // process/sprocess ID
    int    nNodes;         // the number of nodes
    int    nEdges;         // the number of edges
    int    nLoops;         // the number of loops
    int    nExtern;        // the number of external nodes
    int    nClasses;       // the number of initial classes
    int    mindeg;         // minimum value of degree of nodes
    int    maxdeg;         // maximum value of degree of nodes

    // the current graph
    int **adjMat;          // adjacency matrix
    MNodeClass *curcl;     // the current 'MNodeClass' object
    EGraph *egraph;
    BigInt nsym;           // symmetry factor from nodes
    BigInt esym;           // symmetry factor from edges

    // generated set of graphs
    BigInt cDiag;          // the total number of generated graphs
    BigInt c1PI;           // the total number of 1PI graphs
    BigInt cNoTadpole;     // the total number of graphs without tadpoles
    BigInt cNoTadBlock;    // the total number of graphs without tad-blocks
    BigInt c1PINoTadBlock; // the total number of 1PI graphs without tad-blocks
    Fraction wscon;         // weighted sum of graphs
    Fraction wsopi;         // weighted sum of 1PI graphs

    // measures of efficiency
    BigInt ngen;              // generated graph before check
    BigInt ngconn;            // generated connected graph before check
  
    BigInt nCallRefine;        
    BigInt discardRefine;        
    BigInt discardDisc;        
    BigInt discardIso;        

    // for options
    Bool opi;
    Bool opiloop;
    Bool extself;
    Bool selfloop;
    Bool multiedge;
    Bool tadpole;
    Bool tadblock;
    Bool block;
    Bool bipart;

    int    DUMMYPADDING1;

    // table of n edge-connected components
    MConn *mconn;

    // work space for isomorphism
    int **modmat;           // permutated adjacency matrix

    // work space for biconnected component
    int *bidef;
    int *bilow;
    int *bicol;
    int  bicount;

    int    DUMMYPADDING2;

    //----------
    // functions 
    MGraph(int pid, int ncl, NCInput *mgi, Options *opt);
    MGraph(int pid, int ncl, int *cldeg, int *clnum, int *clexl, int *cmind, int *cmaxd, Options *opt);
    ~MGraph(void);

    void   init(void);
    BigInt   generate(void);
    Bool   isExternal(int nd)  { return (nodes[nd]->extloop < 0); };
    void   printAdjMat(MNodeClass *cl);
    void   print(void);
    void   printPy(FILE *fp, long mId);

    Bool   isConnected(void);
    Bool   visit(int nd);
    Bool   isIsomorphic(MNodeClass *cl);
    void   permMat(int size, int *perm, int **mat0, int **mat1);
    int    compMat(int size, int **mat0, int **mat1);
    MNodeClass *refineClass(MNodeClass *cl);
    void   bisearchME(int nd, int pd, int ned, int col, MCOpi *mopi, MCBlock *mblk, ULong *momset, int *next, int *nart);
    void   biconnME(void);

    Bool   isOptM(void);
    void   connectClass(MNodeClass *cl);
    void   connectNode(int sc, int ss, MNodeClass *cl);
    void   connectLeg(int sc, int sn, int tc, int ts, MNodeClass *cl);
    void   newGraph(MNodeClass *cl);
};

/** @ingroup grcc_mgraph */
/** @brief Refinable partition of nodes used for canonical generation checks. */
class MNodeClass {
  public:
    int   clmat[GRCC_MAXNODES][GRCC_MAXNODES];   // matrix used for classification
    int   clist[GRCC_MAXNODES];    // the number of nodes in each class
    int   ndcl[GRCC_MAXNODES];     // node --> class
    int   flist[GRCC_MAXNODES+1];  // the first node in each class
    int   clord[GRCC_MAXNODES];    // ordering of classes
    int   cmindeg[GRCC_MAXNODES];    // min(deg of connectable node)
    int   cmaxdeg[GRCC_MAXNODES];    // max(deg of connectable node)
    int   nNodes;         // the number of nodes
    int   nClasses;       // the number of classes
    int   maxdeg;         // maximal value of degree(node)

    int   flg0;
    int   flg1;
    int   flg2;

    MNodeClass(int nnodes, int nclasses);
    ~MNodeClass(void);

    void  init(int *cl, int mxdeg, int **adjmat);
    void  copy(MNodeClass* mnc);
    int   clCmp(int nd0, int nd1, int cn);
    void  printMat(void);

    void  mkFlist(void);
    void  mkNdCl(void);
    void  mkClMat(int **adjmat);
    void  incMat(int nd, int td, int val);
    int   cmpMNCArray(int *a0, int *a1, int ma);
    void  reorder(MGraph *mg);
};

/** @ingroup grcc_mgraph */
/** @brief Edge record used in connectivity-component analysis. */
class MCEdge {
  public:
    Edge2n nodes;  // nodes at the both size of the edge (leaf --> root)
    ULong  momset; // set of momenta in bit string
    int    momdir; // set of momenta in bit string

    int      DUMMYPADDING;

    MCEdge(void);
    ~MCEdge(void);
};

/** @ingroup grcc_mgraph */
/** @brief One edge-connected (1PI-related) component summary. */
class MCOpi {
  public:
    int *nodes;    // array of nodes in 
    int  nnodes;   // # nodes in the 1PI component
    int  nlegs;    // # leg (bridges) of the 1PI component
    int  next;     // # external particles of the 1PI comp.
    int  nedges;   // # edges in the 1PI comp.
    int  loop;     // # loops in the 1PI comp.
    int  ctloop;   // # loops in the counter terms in the OP comp.
    int  mom0lg;   // # leg (bridges) with 0 momentum

    int      DUMMYPADDING;

    MCOpi(void);
    ~MCOpi(void);

    void init(void);
};

/** @ingroup grcc_mgraph */
/** @brief Bridge summary between edge-connected components. */
class MCBridge {
  public:
    Edge2n nodes;  // nodes at the both size of the bridge
    int    next;   // # momenta of ext. particles flowing on the bridge

    MCBridge(void);
    ~MCBridge(void);
};

/** @ingroup grcc_mgraph */
/** @brief Node-biconnected block summary. */
class MCBlock {
  public:
    Edge2n *edges;   // array of edges in the block
    int     nmedges; // # edges in the block
    int     nartps;  // # articulation points of the block
    int     loop;    // # loop in the block

    int    DUMMYPADDING;

    MCBlock(void);
    ~MCBlock(void);
    void init(void);
};

/** @ingroup grcc_mgraph */
/** @brief Workspace and counters for connectivity decomposition results. */
class MConn {
  public:
    // 2-edge connected components
    MCEdge    *cedges;      // table of edges
    MCOpi     *opics;       // table of n-edge-connected components
    MCBridge  *bridges;     // table of bridges
    MCBlock   *blocks;      // table of blocks
    int       *articuls;    // buffer for nodes for articulation points

    int       *opisp;       // buffer for nodes in 1PI components
    int       *opistk;      // stack of nodes for 1PI components.
    Edge2n    *blksp;       // buffer for edges in blocks
    Edge2n    *blkstk;      // stack of edges for blocks
    int        snodes;      // # nodes
    int        sedges;      // # edges

    // opi components (edge-connected)
    int        nopic;       // # 1PI components (n1PIComps)
    int        nlpopic;     // # looped 1PI components (n1PIComps)
    int        nctopic;     // # 1PI components of one counter term.

    // edges
    int        nbacked;     // # back edges

    // bridges
    int        nbridges;    // # bridges
    int        ne0bridges;  // # bridges whose next=0
    int        ne1bridges;  // # bridges whose next=1
    int        nselfloops;  
    int        nmultiedges;

    // blocks (node-connected)
    int        nblocks;     // # blocks
    int        na1blocks;   // # bridges whose next=0
    int        narticuls;   // # articulation points
    int        neblocks;    // # effective looped blocks

    // indices to work spaces
    int        nopisp;      // # used in opisp
    int        opistkptr;   // # stack pointer of opistk
    int        nblksp;      // # used in blksp
    int        blkstkptr;   // # stack pointer of tlkstk

    int    DUMMYPADDING;

    MConn(int nnod, int nedg);
    ~MConn(void);

    void init(void);
    void initCEdges(MGraph *);
    void pushNode(int nd);
    void pushEdge(int n0, int n1);
    void addCEdge(int n0, int n1, ULong momset);
    void addOPIc(MCOpi *mopi, int stp);
    void addBridge(int n0, int n1, int nex, int nextot);
    void addArtic(int nd, int mul);
    void addBlock(MCBlock *eblk, int stp);
    void addBlockSelf(int nd, int mul);
    void print(void);
    void prEdges(void);
};

/** @ingroup grcc_mgraph */
/** @brief Orbit mapping of nodes under symmetry permutations. */
class MOrbits {
  // Usage
  // 1. node ==> orbit
  //    (class) = nd2or[(node)]
  //
  // 2. class ==> nodes
  //    for (c = 0; c < nClass; c++) {
  //        for (j = flist[c]; j < flist[c+1]; j++) {
  //            (node) = or2nd[c];
  //        }
  //    }
  public:
    int nOrbits;
    int nNodes;
    int nd2or[GRCC_MAXNODES];
    int or2nd[GRCC_MAXNODES];
    int flist[GRCC_MAXNODES+1];

    MOrbits(void);
    ~MOrbits(void);

    void print(void);

    // construction of orbits
    void initPerm(int nnodes);
    void fromPerm(int *perm);
    void toOrbits(void);
};

//**************************************************************
// Particle assignment
//===============================================================
typedef int CheckPt[2];

typedef enum {
    AS_UnAssLegs, AS_Assigned, AS_Assigned0, AS_AssExt, AS_Impossible
} NCandSt;

//===============================================================
/** @ingroup grcc_assign */
/** @brief Candidate interactions for one vertex during assignment. */
class NCand {
  public:
    int      deg;                     // degree of the node
    NCandSt  st;                      // status
    int      nilist;                  // length of the list
    int      ilist[GRCC_MAXMINTERACT];     // list of candidates

    //========
    NCand(const NCandSt sta, const int dega, const int nilst, int *ilst);
    ~NCand(void);

    // print
    void prNCand(const char* msg);
};

//===============================================================
/** @ingroup grcc_assign */
/** @brief Candidate particles for one edge during assignment. */
class ECand {
  public:
    Bool  det;                     // determined or not
    int   nplist;                  // size of the list of candidates
    int   plist[GRCC_MAXMPARTICLES2];   // list of candidates

    ECand(int dt, int nplist, int *plst);
    ~ECand(void);

    // print
    void   prECand(const char *msg);
};

//===============================================================
/** @ingroup grcc_assign */
/**
 * @brief Local node-side connectivity view used by the assignment solver.
 * @details Stores adjacent nodes/edges per leg for incremental propagation.
 * @note Invariant: `0 <= nlegs <= deg`.
 */
class ANode {
  public:
    int    deg;              // degree of the node
    int    nlegs;            // the number of legs already assigned
    int   *anodes;           // anodes[j] = (next node of leg j)
    int   *aedges;           // aedges[j] = (next edge of leg j)
    int   *aelegs;           // aelegs[j] = (leg of the next edge)
    NCand *cand;             // candidate list

    ANode(int dg);
    ~ANode(void);

    int  newleg(void);        // get a new leg
};

//===============================================================
/** @ingroup grcc_assign */
/**
 * @brief Local edge-side connectivity and particle state for assignment.
 * @details Edge orientation is encoded by legs 0->1 for particle flow.
 */
class AEdge {
  public:

    ECand *cand;            // candidate
    int    nodes[2];        // nodes of both sides of the edge
    int    nlegs[2];        // nodes of both sides of the edge
    int    ptcl;            // particle defined in the model

    int    DUMMYPADDING;

    AEdge(int n0, int l0, int n1, int l1);
    ~AEdge(void);
};

//===============================================================
/** @ingroup grcc_assign */
/**
 * @brief Backtracking solver for particle/interaction assignment on EGraph.
 * @note Invariants in active solver state:
 * - `0 <= nETotal <= nEdges`
 * - `nodes` and `edges` tables are allocated with capacities `nNodes` and
 *   `nEdges`, respectively
 * - `cplleft[j]` is interpreted for `0 <= j < model->ncouple`
 */
class Assign {
  public:

    EGraph     *egraph;       // EGraph object
    MGraph     *mgraph;       // MGraph object
    SProcess   *sproc;        // Sub-process object
    Process    *proc;         // Process object
    Model      *model;        // Model object
    Options    *opt;          // Option object
    AStack     *astack;       // Stack for saving candidates
    PNodeClass *pnclass;      // class of nodes
    MOrbits    *orbits;       // orbits of nodes by symmetry group

    int         nNodes;       // the number of nodes
    int         nEdges;       // the number of edges
    int         nExtern;      // the number of external particles.
    int         nETotal;      // the total number of edges

    BigInt      nAGraphs;     // the number of assigned graphs
    Fraction    wAGraphs;     // the weighted sum of graphs
    BigInt      nAOPI;        // the number of assigned 1PI
    Fraction    wAOPI;        // the weighted sum of 1PI

    ANode     **nodes;        // table of nodes
    AEdge     **edges;        // table of edges

    CheckPt     checkpoint0;  // save stack pointers

    int         cplleft[GRCC_MAXNCPLG];  // coupling constants left

    //===========================================
    /** @brief Construct assignment solver for one subprocess and topology.
     *  @param[in] sprc Active subprocess.
     *  @param[in] mgr Input topology graph.
     *  @param[in] pnc Matched node classes.
     *  @note `sprc`, `mgr`, and `pnc` are borrowed (non-owning) references.
     *  @note Current implementation executes the full assignment search during
     *  construction.
     */
    Assign(SProcess *sprc, MGraph *mgr, PNodeClass *pnc);
    ~Assign(void);

    //===========================================
    // print lists of candidates
    void    prCand(const char *msg);

    // check
    void checkAG(const char *msg);

    //===========================================
    // control of assignment

    /** @brief Entry point of recursive assignment.
     *  @pre fromMGraph() has initialized node/edge candidate tables.
     *  @post Internal counters (`nAGraphs`, `wAGraphs`, `nAOPI`, `wAOPI`)
     *  are updated for accepted assignments discovered during recursion.
     *  @return True if at least one valid assignment path is found.
     */
    Bool    assignAllVertices(void);

    // select a source node for assignment
    Bool    selectVertex(void);
    Bool    selectVertexSimp(int lastv);

    /** @brief Recursively assign one leg of a selected vertex.
     *  @param v Vertex index.
     *  @param lastlg Last processed leg index (-1 at start).
     *  @return True if search can continue from this branch.
     */
    Bool    selectLeg(int v, int lastlg);

    /** @brief Assign one interaction candidate to vertex.
     *  @param v Vertex index.
     *  @return True if assignment branch remains feasible.
     */
    Bool    assignVertex(int v);

    /** @brief Finalize one complete assignment and apply filters.
     *  @return True if the completed assignment is accepted.
     */
    Bool    allAssigned(void);

    //===========================================
    // Input and output

    /** @brief Initialize assignment structures from MGraph.
     *  @pre `mgraph`, `egraph`, and `pnclass` are non-null and mutually
     *  consistent for the current subprocess.
     *  @post `nodes`/`edges` tables are populated and candidate lists are
     *  initialized for subsequent assignment steps.
     *  @return True on successful initialization.
     */
    Bool    fromMGraph(void);

    /** @brief Add one assignment-edge record.
     *  @param[in] n0 Endpoint node 0.
     *  @param[in] n1 Endpoint node 1.
     *  @param[in] nplist Number of particle candidates in plist.
     *  @param[in] plist Candidate particle codes in edge orientation.
     */
    void    addEdge(int n0, int n1, int nplist, int *plist);

    /** @brief Connect node-leg and edge-leg incidences.
     *  @param[in] n0 Node index at endpoint 0.
     *  @param[in] l0 Leg index on node n0.
     *  @param[in] eg Edge index.
     *  @param[in] el Edge-side leg index (0 or 1).
     *  @param[in] n1 Opposite endpoint node index.
     *  @param[in] l1 Leg index on node n1.
     */
    void    connect(int n0, int l0, int eg, int el, int n1, int l1);

    /** @brief Materialize current assignment into EGraph.
     *  @param[in] aid Assignment id inside current topology (1-based sequence).
     *  @param[in] nsym Node symmetry factor.
     *  @param[in] esym Edge symmetry factor.
     *  @param[in] nsym1 Node symmetry factor without external symmetrization.
     *  @return True on success.
     */
    Bool    fillEGraph(int aid, BigInt nsym, BigInt esym, BigInt nsym1);

    /** @brief Compute leg reorder map to match interaction leg order.
     *  @param[in] n Number of legs.
     *  @param[out] reord Permutation map (length >= n).
     *  @param[in] plist Current particle list (length >= n).
     *  @param[in,out] used Scratch marks for permutation construction (length >= n).
     *  @return Pointer to reord.
     */
    int    *reordLeg(int n, int *reord, int *plist, int *used);

    //===========================================
    // direction of particle on an edge and at (node, leg)

    /**
     * @note Signed-edge encoding follows I2Vedge/V2Iedge/V2Ileg in
     * grccparam.h. Conversion helpers below expose this convention
     * in assignment-oriented node-leg coordinates.
     */

    // convert particle code
    //   at node-leg ('n', 'ln') <==> edge at ('n', 'ln')
    int     getLegParticle(int n,  int ln);

    // convert particle 'pt' on the edge to ('n', 'ln')
    int     legEdgeParticle(int n, int ln, int pt);

    /** @brief Convert edge-oriented particle candidates to node-leg orientation.
     *  @param[in] v Node index.
     *  @param[in] lg Leg index on node v.
     *  @param[in] nplst Input candidate count.
     *  @param[in] plst Input candidate particles in edge orientation.
     *  @param[out] rlist Output candidates in node-leg incoming orientation.
     *  @param[in] size Capacity of rlist.
     *  @return Number of entries written to rlist.
     */
    int     legPart(int v, int lg, int nplst, int *plst, int *rlist, const int size);

    /** @brief Convenience wrapper of legPart for current edge candidates.
     *  @param[in] v Node index.
     *  @param[in] ln Leg index on node v.
     *  @param[out] plist Output candidate particles in node-leg orientation.
     *  @param[in] size Capacity of plist.
     *  @return Number of entries written to plist.
     */
    int     candPart(int v, int ln, int *plist, const int size);

    //===========================================
    // assignment

    // find a vertex to be assigned
    int     selUnAssVertex(void);
    int     selUnAssVertexSimp(int lastv);

    // find a leg of the vertex to be assigned
    int     selUnAssLeg(int v, int lastlg);

    // assign the interaction 'ia' to the vertex 'v'.
    NCandSt assignIVertex(int v, int ia);

    // assign particle 'pt' to leg 'ln' of node 'n'.
    Bool    assignPLeg(int n, int ln, int pt);
    Bool    isOrdPLeg(int n, int ln, int pt);
    Bool    detEdge(int e);

    //===========================================
    // candidates

    /** @brief Partition node-leg candidates into decided and undecided sets.
     *  @param[in] v Node index.
     *  @param[out] npdass Count written to pdass.
     *  @param[out] pdass Candidates from already-determined adjacent edges.
     *  @param[out] npuass Count written to puass.
     *  @param[out] puass Candidates from unresolved adjacent edges.
     *  @param[in] size Capacity of pdass/puass arrays.
     *  @return False if any adjacent edge has an empty candidate list.
     */
    Bool    candPartClassify(int v, int *npdass, int *pdass, int *npuass, int *puass, const int size);

    /** @brief Propagate constraints and shrink candidate sets at one vertex.
      *  @param[in] v Vertex index.
      *  @pre Candidate lists for vertex `v` and adjacent edges are initialized.
      *  @post Candidate lists may be reduced; newly singleton edge candidates
      *  may be fixed via detEdge().
     *  @return False if constraints become inconsistent.
     */
    Bool    updateCandNode(int v);

    //===========================================
    // filter 

    Bool    checkOrderCpl(void);
    Bool    isOrdLegs(void);
    /** @brief Canonical isomorphism check for completed assignment.
     *  @param cl Refined MGraph node classes.
     *  @param nsym Output node symmetry factor.
     *  @param esym Output edge symmetry factor.
     *  @param nsym1 Output node symmetry before external symmetrization.
     *  @return True if this assignment is the canonical representative.
     */
    Bool    isIsomorphic(MNodeClass *cl, BigInt *nsym, BigInt *esym, BigInt *nsym1);
    /** @brief Compare current assignment with one permuted by p.
     *  @param[in] p Node permutation array (length >= nNodes).
     *  @param[in] cl Refined node-class information used for node ordering.
     *  @return Negative/zero/positive according to lexicographic ordering.
     */
    int     cmpPermGraph(int *p, MNodeClass *cl);
    /** @brief Compare two assigned internal nodes under current ordering rules.
     *  @param[in] nd0 First node index.
     *  @param[in] nd1 Second node index.
     *  @param[in] cn Refined node classes.
     *  @return Negative/zero/positive according to node ordering.
     */
    int     cmpNodes(int nd0, int nd1, MNodeClass *cn);
    /** @brief Compute multiplicative symmetry factor from indistinguishable edges.
     *  @return Edge permutation factor (>= 1 for valid assignments).
     */
    BigInt  edgeSym(void);

    /** @brief Save current remaining coupling-order counters.
     *  @param[out] sav Output buffer, length >= model->ncouple.
     */
    void    saveCouple(int *sav);
    /** @brief Restore remaining coupling-order counters.
     *  @param[in] sav Input buffer previously filled by saveCouple().
     */
    void    restoreCouple(int *sav);
    /** @brief Subtract one interaction coupling vector from remaining counters.
     *  @param[in] cpl Coupling-order vector, length >= model->ncouple.
     *  @return False if any component would become negative.
     */
    Bool    subCouple(int *cpl);

#ifdef CHECK
    //===========================================
    // check
    Bool    checkCand(const char *msg);

    void    checkNode(int n, const char *msg);
#endif
};

//**************************************************************
// Stack of candidate lists
//===============================================================
/** @ingroup grcc_assign */
/** @brief Snapshot entry for node candidate rollback. */
class NStack {
  public:
    int     noden;
    int     deg;
    NCandSt st;
    int     nilist;
    int     ilist[GRCC_MAXMINTERACT];

    NStack() { };
    ~NStack() { };

    void print(const char *msg);

};

//===============================================================
/** @ingroup grcc_assign */
/** @brief Snapshot entry for edge candidate rollback. */
class EStack {
  public:
    int    edgen;
    int    det;
    int    nplist;
    int    plist[GRCC_MAXMPARTICLES2];

    EStack() { };
    ~EStack() { };

    void print(const char *msg);
};


//===============================================================
/** @ingroup grcc_assign */
/** @brief Stack manager for reversible assignment-state mutations. */
class AStack {
  public:
    Assign     *agraph;

    NStack    **nStack;       // stack for nodes
    int         nStackP;      // stack pointer for nodes
    int         nSize;        // stack size

    EStack    **eStack;       // stack for edges
    int         eStackP;      // stack pointer for edges
    int         eSize;        // stack size

    AStack(int nSize, int eSize);
    ~AStack(void);

    void setAGraph(Assign *ag);

    void checkPoint(CheckPt sav);
    void restore(CheckPt sav);
    void restoreMsg(CheckPt sav, const char *msg);
    void prStack(void);

    void pushNode(int n);
    void pushEdge(int e);

  private:
    void restoreNode(int spr);
    void restoreEdge(int spr);
};

//==============================================================
// end of namespace Grcc
#ifdef GRCC_NAMESPACE
}
#endif
