#include "SCUM_Invocation.hh"
#include "SCUM_Util.hh"
#include <FL/Fl.H>

void SCUM_Invocation::defer()
{
	SCUM_InvocationManager::instance().add(this);
}

void SCUM_Invocation::invoke()
{
	if (m_cb && m_obj) (*m_cb)(this);
}

SCUM_InvocationManager& SCUM_InvocationManager::instance()
{
	static SCUM_InvocationManager s_instance;
	return s_instance;
}

void SCUM_InvocationManager::add(SCUM_Invocation* inv)
{
	if (m_queue.empty()) {
		Fl::add_timeout(0.0, workCB, this);
	}
	m_queue.push_back(inv);
}

void SCUM_InvocationManager::remove(SCUM_Invocation::Callback cb, void* obj)
{
	Queue::iterator it = m_queue.begin();
	while (it != m_queue.end()) {
		if ((!cb || (cb == (*it)->m_cb)) &&
			(!obj || (obj == (*it)->m_obj))) {
			(*it)->m_cb = 0;
			(*it)->m_obj = 0;
		}
	}
}

void SCUM_InvocationManager::workCB(void* arg)
{
	((SCUM_InvocationManager*)arg)->work();
}

void SCUM_InvocationManager::work()
{
	SCUM_DEBUG_PRINT("%d\n", m_queue.size());
	if (!m_queue.empty()) {
		SCUM_Invocation* inv = m_queue.front();
		m_queue.pop_front();
		inv->invoke();
		delete inv;
	}
	if (!m_queue.empty()) {
		Fl::repeat_timeout(0.0, &workCB);
	}
}

// EOF
