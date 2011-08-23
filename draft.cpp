
using namespace std;

struct Mutex {
	void lock();
	void unlock();
};

typedef shared_ptr<Mutex> MutexP;

struct Pool {
	Mutex m;
	void lock() { m.lock(); }
	void unlock() { m.unlock(); }
	
	map<PyObject*, set<MutexP> > mutexes;
	map<MutexP, list<PyObject*> > objsByMutex;
	
	void registerMutex(MutexP m, list<PyObject*> objs) {
		objsByMutex[m] = objs;
		for(PyObject* o : objs)
			mutexes[o].insert(m);
	}
	
	void unregisterMutex(MutexP m) {
		for(PyObject* o : objsByMutex[m])
			mutexes[o].erase(m);
		objsByMutex.erase(m);
	}
	
	set<MutexP> getMutexesForObjs(list<PyObject*> objs) {
		set<MutexP> ret;
		for(PyObject* o : objs) {
			set<MutexP> ms& = mutexes[o];
			ret.insert(ms.begin(), ms.end());
		}
		return ret;
	}
};

Pool pool;


PyObject* wrapped_func(func, list<PyObject*> args) {
	MutexP m = new Mutex();
	m->lock();
	while(true) {
		pool.lock();
		set<MutexP> ms = pool.getMutexesForObjs(args);
		if(ms.empty()) {
			pool.registerMutex(m, args);
			pool.unlock();
			break;
		}
		pool.unlock();
		for(MutexP m2 : ms) {
			m2.lock(); m2.unlock();
		}
	}
	
	PyObject* ret = func(args);
	
	pool.lock();
	pool.unregisterMutex(m);
	pool.unlock();
	
	return ret;
}

// for all builtin functions: wrap via wrapped_func
