/*
 * $Id: cache.h,v 1.4 2004/06/16 18:46:50 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_CACHE_HPP
#define VDR_TEXT2SKIN_CACHE_HPP

#include "common.h"
#include <vdr/tools.h>

// template class generic cache

template<class K,class D>
class cText2SkinCache {
private:
	struct Item {
		K      _key;
		D      _data;
		time_t _lastUsed;

		Item  *_next;
		Item  *_prev;

		Item() { _next = _prev = NULL; }
	};

	typedef map <K,Item*> DataMap;

	DataMap _items;
	int     _maxItems;
	Item   *_first;
	Item   *_last;
	
	void Unlink(Item *item);
	void Update(Item *item);
	void Delete(Item *item);
	void Delete(K &key, D &data);

public:
	cText2SkinCache(int maxItems);
	~cText2SkinCache();

	void Flush(void);
	bool Contains(const K &key);
	D &operator[](const K &key);
};

template<class K,class D>
inline void cText2SkinCache<K,D>::Unlink(Item *item) {
	if (item == _first) {
		_first = item->_next;
		if (_first)
			_first->_prev = NULL;
		else
			_last = NULL;
	} else if (item == _last) {
		_last = item->_prev;
		_last->_next = NULL;
	} else {
		item->_prev->_next = item->_next;
		item->_next->_prev = item->_prev;
	}
}

template<class K,class D>
inline void cText2SkinCache<K,D>::Delete(Item *item) {
	Delete(item->_key, item->_data);
	delete item;
}

template<class K,class D>
inline void cText2SkinCache<K,D>::Update(Item *item) {
	item->_lastUsed = time_ms();
	if (item->_next != NULL || item->_prev != NULL)
		Unlink(item);

	item->_next = NULL;
	item->_prev = _last;
	if (_last)
		_last->_next = item;
	_last = item;
	if (!_first)
		_first = item;

	while ((int)_items.size() > _maxItems) {
		Item *aged = _first;
		_items.erase(aged->_key);
		Unlink(aged);
		Delete(aged);
	}
}

template<class K,class D>
inline bool cText2SkinCache<K,D>::Contains(const K &key) {
	return (_items.find(key) != _items.end());
}

template<class K,class D>
cText2SkinCache<K,D>::cText2SkinCache(int maxItems) {
	_maxItems = maxItems;
	_first = _last = NULL;
}

template<class K,class D>
cText2SkinCache<K,D>::~cText2SkinCache() {
	Flush();
}

template<class K,class D>
void cText2SkinCache<K,D>::Delete(K &key, D &Data) {
	abort();
}

template<class K,class D>
void cText2SkinCache<K,D>::Flush(void) {
	Item *cur = _first;
	while (cur) {
		Item *tmp = cur->_next;
		_items.erase(cur->_key);
		Unlink(cur);
		Delete(cur);
		cur = tmp;
	}
}

template<class K,class D>
D &cText2SkinCache<K,D>::operator[](const K &key) {
	Item *item;
	if (Contains(key)) {
		item = _items[key];
	} else {
		item = new Item;
		item->_key = key;
		_items[key] = item;
	}
	Update(item);
	return item->_data;
}

#endif // VDR_TEXT2SKIN_CACHE_HPP
