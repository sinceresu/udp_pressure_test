#ifndef _BOUNDED_POOL_H
#define _BOUNDED_POOL_H

#include <boost/circular_buffer.hpp>
#include <vector>
#include <thread>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include <boost/progress.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp> 


template <class T>
class BoundedPool {
public:

	typedef std::vector<T> container_type;
	typedef boost::circular_buffer<T> pool_type;
	typedef typename pool_type::size_type size_type;
	typedef typename pool_type::value_type value_type;
	typedef typename boost::call_traits<value_type>::param_type param_type;

	explicit BoundedPool(size_type capacity) : m_unread(0), m_freePool(capacity)
	{
	}
	void add(value_type item)
	{
		m_container.push_back(item);
		push(item);
	}
	bool push(value_type item) {
		// param_type represents the "best" way to pass a parameter of type value_type to a method

		boost::mutex::scoped_lock lock(m_mutex);
		m_not_full.wait(lock, boost::bind(&BoundedPool<value_type>::is_not_full, this));

		m_freePool.push_front(item);
		++m_unread;
		lock.unlock();
		m_not_empty.notify_one();
		return true;
	}

	bool pop(value_type* pItem, int iTimeOutMilS) {
		boost::posix_time::time_duration tdTimeout(0, 0, 0, iTimeOutMilS*(boost::posix_time::time_duration::ticks_per_second())/1000);
		boost::mutex::scoped_lock lock(m_mutex);
		if(false == m_not_empty.timed_wait(lock, tdTimeout, boost::bind(&BoundedPool<value_type>::is_not_empty, this)))
		return false;
     	
		*pItem = m_freePool[--m_unread];
		lock.unlock();
		m_not_full.notify_one();
     
		return true;
	}

	void clear()
	{
		boost::mutex::scoped_lock lock(m_mutex);
		m_unread = 0;
		m_freePool.clear();
	}

	size_t getSize()
	{
	return (size_t)m_unread;
	}


private:
	BoundedPool(const BoundedPool&);              // Disabled copy constructor
	BoundedPool& operator = (const BoundedPool&); // Disabled assign operator

	bool is_not_empty() const { return m_unread > 0; }
	bool is_not_full() const { return m_unread < m_freePool.capacity(); }

	boost::mutex m_mutex;
	size_type m_unread;
	container_type m_container;
	pool_type m_freePool;
	boost::condition m_not_empty;
	boost::condition m_not_full;
};



#endif  /* _BOUNDEDBUFFER_H */
