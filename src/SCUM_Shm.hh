#ifndef SCUM_SHM_HH_INCLUDED
#define SCUM_SHM_HH_INCLUDED

#include <semaphore.h>
#include <stdint.h>

#ifndef SHM_NAME_MAX
# define SHM_NAME_MAX 30
#endif

class SCUM_SharedMemory
{
public:
	struct Segment
	{
		uint32_t	m_size;
		uint8_t		m_data[1];
	};

	SCUM_SharedMemory();
	~SCUM_SharedMemory();

	int attach(const char* name);
	void detach();

	bool isAttached() { return m_fd != -1; }
	inline size_t getSize() const { return m_seg->m_size; }
	uint8_t* getData() { return m_seg->m_data; }

	void lock();
	void unlock();

private:
	int			m_fd;
	sem_t*		m_sem;
	Segment*	m_seg;
};

#endif // SCUM_SHM_HH_INCLUDED
