/*
 *  ZanDemo: Zandronum demo launcher
 *  Copyright (C) 2013 Santeri Piippo
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "bytestream.h"
#include "misc.h"
#include <string.h>

union {
	uint32 i;
	float f;
} g_floatunion;

Bytestream::Bytestream (ulong len) {
	m_data = null;
	resize (len);
	clear();
}

Bytestream::Bytestream (const char* data, ulong len) {
	m_data = null;
	init (data, len);
}

void Bytestream::resize (ulong newsize) {
	char* olddata = null;
	ulong oldsize;

	if (m_data) {
		oldsize = m_size;
		olddata = new char[oldsize];
		memcpy (olddata, m_data, oldsize);
	}

	delete[] m_data;
	m_data = new uint8[newsize];
	m_size = newsize;

	if (olddata)
		memcpy (m_data, olddata, min<ulong> (oldsize, newsize));
}

void Bytestream::init (const char* data, ulong len) {
	resize (len);
	memcpy (m_data, data, len);
	m_ptr = &m_data[0];
	m_len = len;
}

size_t Bytestream::len() const {
	return m_len;
}

void Bytestream::clear() {
	m_ptr = &m_data[0];
	m_len = 0;
}

uint8& Bytestream::subscript (ulong idx) {
	return m_data[idx];
}

const uint8& Bytestream::const_subscript (ulong idx) const {
	return m_data[idx];
}

void Bytestream::seek (ulong pos) {
	m_ptr = m_data + pos;
}

// =============================================================================
void Bytestream::rewind() {
	m_ptr = m_data;
}

ulong Bytestream::bytesLeft() const {
	return (m_len - (m_ptr - &m_data[0]));
}

ulong Bytestream::spaceLeft() const {
	return (m_size - m_len);
}

// =============================================================================
bool Bytestream::readByte (uint8& val) {
	if (bytesLeft() < 1)
		return false;

	val = *m_ptr++;
	return true;
}

// =============================================================================
bool Bytestream::readShort (uint16& val) {
	if (bytesLeft() < 2)
		return false;

	val = 0;

	for (int i = 0; i < 2; ++i)
		val |= *m_ptr++ << (i * 8);

	return true;
}

// =============================================================================
bool Bytestream::readLong (uint32& val) {
	if (bytesLeft() < 4)
		return false;

	val = 0;

	for (int i = 0; i < 4; ++i)
		val |= *m_ptr++ << (i * 8);

	return true;
}

// =============================================================================
bool Bytestream::readFloat (float& val) {
	if (!readLong (g_floatunion.i))
		return false;

	val = g_floatunion.f;
	return true;
}

// =============================================================================
bool Bytestream::readString (str& val) {
	if (bytesLeft() < 1)  // need at least the null terminator
		return false;

	uint8_t c;

	while (readByte (c) && c != '\0')
		val += (char) c;

	return true;
}

// =============================================================================
void Bytestream::doWrite (uint8_t val) {
	*m_ptr++ = val;
	m_len++;
}

void Bytestream::growToFit (ulong bytes) {
	if (spaceLeft() < bytes)
		resize (m_size + bytes + 128);
}

bool Bytestream::readBytes (uint8 numbytes, uint8* val) {
	while (numbytes--)
		if (!readByte (*val++))
			return false;

	return true;
}

void Bytestream::writeBytes (uint8 numbytes, const uint8* val) {
	growToFit (numbytes);

	while (numbytes--)
		writeByte (*val++);
}

// =============================================================================
void Bytestream::writeByte (uint8 val) {
	growToFit (1);
	doWrite (val);
}

// =============================================================================
void Bytestream::writeShort (uint16 val) {
	growToFit (2);

	for (int i = 0; i < 2; ++i)
		doWrite ( (val >> (i * 8)) & 0xFF);
}

// =============================================================================
void Bytestream::writeLong (uint32 val) {
	growToFit (4);

	for (int i = 0; i < 4; ++i)
		doWrite ( (val >> (i * 8)) & 0xFF);
}

// =============================================================================
void Bytestream::writeFloat (float val) {
	g_floatunion.f = val;
	writeLong (g_floatunion.i);
}

// =============================================================================
void Bytestream::writeString (str val) {
	growToFit (val.length() + 1);

for (qchar c : val)
		doWrite (c.toAscii());

	doWrite ('\0');
}

// =============================================================================
bool Bytestream::tryMerge (const Bytestream& other) {
	if (spaceLeft() < other.len())
		return false;

	for (ulong i = 0; i < other.len(); ++i)
		writeByte (other[i]);

	return true;
}

void Bytestream::merge (const Bytestream& other) {
	growToFit (other.len());

	if (!tryMerge (other)) {
		// Shouldn't happen
		fprint (stderr, "ByteStream: Not enough space for merge (%1 bytes left, need %2)",
				spaceLeft(), other.len());
		abort();
	}
}

const uint8* Bytestream::data() const {
	return m_data;
}
