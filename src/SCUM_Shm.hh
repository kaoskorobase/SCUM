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

    $Id$
*/


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
