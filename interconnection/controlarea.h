// $Id: controlarea.h 5185 2015-06-23 00:13:36Z dchassin $

#ifndef _CONTROLAREA_H
#define _CONTROLAREA_H

#include <armadillo>

class scheduler;

using namespace arma;
using namespace std;

typedef enum {
	CAS_OK=KC_GREEN,
	CAS_OVERCAPACITY=KC_RED,
	CAS_CONSTRAINED=KC_ORANGE,
	CAS_ISLAND=KC_BLUE,
	CAS_UNSCHEDULED=KC_PURPLE,
	CAS_BLACKOUT=KC_BLACK,
} CONTROLAREASTATUS;

typedef enum {
	SM_CENTRAL = 0, // schedule is generated by interconnetion scheduler
	SM_LOCAL = 1, // schedule is generated locally
};

typedef enum {
	SS_ERROR = -1,
	SS_NONE = 0,
	SS_UNCONSTRAINED = 1,
	SS_CONSTRAINED = 2,
} SCHEDULESOLUTION;

typedef enum {
	SFH_NONE = 0x00, // default behavior (error out quietly)
	SFH_IGNORE = 0x01, // suppress error handling on failure
	SFH_DUMP = 0x02, // dump algebraic solution on failure
} SCHEDULEFAILUREHANDLING;

class controlarea : public gld_object {
private:
	static set default_on_schedule_failure;
public:
	GL_ATOMIC(enumeration,schedule_source); ///< LOCAL or CENTRAL (default)
	GL_ATOMIC(int64,update); ///< pseudo variable for receiving messages from generators, loads, and interties
	GL_ATOMIC(double,inertia); ///< total inertia of generators and loads
	GL_ATOMIC(double,capacity); ///< total capacity of generators
	GL_ATOMIC(double,supply); ///< actual generation supply
	GL_ATOMIC(double,demand); ///< actual load demand
	GL_ATOMIC(double,schedule); ///< scheduled intertie exchange
	GL_ATOMIC(double,actual); ///< actual intertie exchange
	GL_ATOMIC(double,ace); ///< ace value
	GL_STRUCT(double_array,ace_filter); ///< ace filter coefficients (maximum is second order)
	GL_ATOMIC(double,bias); ///< frequency bias
	GL_ATOMIC(double,losses); ///< line losses (internal+export)
	GL_ATOMIC(double,internal_losses); ///< internal area losses (per unit generation)
	GL_ATOMIC(double,imbalance); ///< net imbalance of power
	GL_ATOMIC(double,forecast); ///< hourahead forecast energy export (or import)
	GL_ATOMIC(double,pmax); ///< minimum price of demand
	GL_ATOMIC(double,d); ///< marginal price of demand
	GL_ATOMIC(double,qu); ///< quantity of demand at maximum price
	GL_ATOMIC(double,qr); ///< available demand response
	GL_ATOMIC(double,pmin); ///< minimum price of supply
	GL_ATOMIC(double,s); ///< marginal price of supply
	GL_ATOMIC(double,qw); ///< available supply at minimum price
	GL_ATOMIC(double,qg); ///< available dispatchable generation
	GL_ATOMIC(double,dq); ///< desired quantity difference
	GL_ATOMIC(double,dp); ///< desired price difference
	GL_ATOMIC(double,qd); ///< desired price difference
	GL_ATOMIC(double,qs); ///< desired price difference
	GL_ATOMIC(double,pd); ///< desired quantity difference
	GL_ATOMIC(double,ps); ///< desired quantity difference
	GL_ATOMIC(enumeration,status);
	GL_ATOMIC(size_t,node_id); ///< node id
	GL_BITFLAGS(set,on_schedule_failure); // handling of scheduling failure
private:
	double f0; ///< system frequency
	double fr; ///< relative frequency difference
	unsigned int n_intertie;
	gld_object *system;
	scheduler *central_scheduler;
	gld_property update_system;
	gld_property frequency;
	unsigned int init_count;
	OBJECTLIST *intertie_list;
	OBJECTLIST *generator_list;
	OBJECTLIST *load_list;
	TIMESTAMP last_update;
public:
	void add_generator(OBJECT *obj);
	void add_load(OBJECT *obj);
	SCHEDULESOLUTION update_local_schedule(void);
	bool update_central_schedule(void);
private:
	mat A, Ainv;
	vec x, b;
public:
	DECL_IMPLEMENT(controlarea);
	DECL_SYNC;
	DECL_PRECOMMIT;
	DECL_NOTIFY(update);
	int kmldump(int (*stream)(const char*, ...));
public:
	inline double get_frequency(void) { return frequency.get_double(); };
};

#endif // _CONTROLAREA_H
