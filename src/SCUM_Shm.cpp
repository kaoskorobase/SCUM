#include "SCUM_Shm.hh"

#include <errno.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

SCUM_SharedMemory::SCUM_SharedMemory()
	: m_fd(-1),
	  m_sem((sem_t*)SEM_FAILED),
	  m_seg((Segment*)MAP_FAILED)
{ }

SCUM_SharedMemory::~SCUM_SharedMemory()
{
}

int SCUM_SharedMemory::attach(const char* name)
{
	uint32_t size;
	int err;

	if (isAttached()) return 0;

	m_fd = shm_open(name, O_RDWR, 0);
	if (m_fd == -1) {
		err = errno;
		perror("shm_open");
		goto error;
	}

	m_seg = (Segment*)mmap(0, sizeof(Segment), PROT_READ, MAP_SHARED, m_fd, 0);
	if (m_seg == MAP_FAILED) {
		err = errno;
		perror("mmap");
		goto error;
	}

	size = m_seg->m_size;
	munmap(m_seg, sizeof(Segment));

	m_seg = (Segment*)mmap(0, sizeof(Segment) + size, PROT_READ|PROT_WRITE, MAP_SHARED, m_fd, 0);
	if (m_seg == MAP_FAILED) {
		err = errno;
		perror("mmap");
		goto error;
	}

	char semName[SHM_NAME_MAX];
	if (snprintf(semName, SHM_NAME_MAX, "%s.sem", name) >= SHM_NAME_MAX) {
		err = EINVAL;
		goto error;
	}

	m_sem = sem_open(semName, 0);
	if (m_sem == (void*)SEM_FAILED) {
		err = errno;
		perror("sem_open");
		goto error;
	}

	return 0;

error:
	detach();
	return err;
}

void SCUM_SharedMemory::detach()
{
	if (m_sem != (void*)SEM_FAILED) {
		sem_close(m_sem);
		m_sem = (sem_t*)SEM_FAILED;
	}
	if (m_seg != MAP_FAILED) {
		munmap(m_seg, sizeof(Segment) + m_seg->m_size);
		m_seg = (Segment*)MAP_FAILED;
	}
	if (m_fd != -1) {
		close(m_fd);
		m_fd = -1;
	}
}

void SCUM_SharedMemory::lock()
{
	if (m_sem != (void*)SEM_FAILED)
		sem_wait(m_sem);
}

void SCUM_SharedMemory::unlock()
{
	if (m_sem != (void*)SEM_FAILED)
		sem_post(m_sem);
}

// EOF
