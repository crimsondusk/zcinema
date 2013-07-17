#ifndef BYTESTREAM_H
#define BYTESTREAM_H

#include "types.h"

class Bytestream {
private:
	uint8* m_data;
	uint8* m_ptr;
	ulong m_size, m_len;
	
	void doWrite (uint8_t val);
	
public:
	Bytestream (ulong len = 2048);
	Bytestream (const char* data, ulong len);
	
	void init (const char* data, ulong len);
	void rewind ();
	void seek (ulong pos);
	void clear ();
	void merge (const Bytestream& other);
	bool tryMerge (const Bytestream& other);
	void resize (ulong len);
	size_t len () const;
	ulong bytesLeft () const;
	ulong spaceLeft () const;
	void growToFit (ulong bytes);
	const uint8* data () const;
	
	bool readBytes (uint8 numbytes, uint8* val);
	bool readByte (uint8& val);
	bool readShort (uint16& val);
	bool readLong (uint32& val);
	bool readString (str& val);
	bool readFloat (float& val);
	
	void writeBytes (uint8 numbytes, const uint8* val);
	void writeByte (uint8 val);
	void writeShort (uint16 val);
	void writeLong (uint32 val);
	void writeFloat (float val);
	void writeString (str val);
	
	Bytestream& operator<< (const Bytestream& other) {
		merge (other);
		return *this;
	}
	
	uint8& subscript (ulong idx);
	const uint8& const_subscript (ulong idx) const;
	
	uint8& operator[] (ulong idx) {
		return subscript (idx);
	}
	
	const uint8& operator[] (ulong idx) const {
		return const_subscript (idx);
	}
};

#endif // BYTESTREAM_H