/*  -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4 -*-
    vim: et sta sw=4:

    SCUM. copyright (c) 2004 stefan kersten.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA

    $Id: SCUM_Button.cpp 13 2005-03-10 17:21:31Z sk $
*/


#include "SCUM_Shm.hh"

#include <errno.h>
#include <fcntl.h>
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
