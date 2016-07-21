#ifndef PROFILERGRAPH_H_
#define PROFILERGRAPH_H_

// DREAM OS
// DreamOS/Dimension/Primitives/ProfilerGraph.h
// ProfilerGraph is a class for managing a graph with a time unit axis

#include <chrono>
#include <vector>

template<typename T>
class ProfilerGraph {
public:
	// Explicitly set storage size
	ProfilerGraph(size_t size = 0) :
		m_minTime(std::chrono::high_resolution_clock::now()),
		m_recordTime(0) // no duration is set
	{
		Resize(size);
	}

	// Graph recording the past [time] seconds
	ProfilerGraph(double time) :
		m_minTime(std::chrono::high_resolution_clock::now()),
		m_recordTime(time)
	{
		Resize(static_cast<size_t>(time / m_recordRate) + 1);
	}

	// Graph recording the past [time] seconds, [record_rate] is the density measured in seconds
	ProfilerGraph(double time, double record_rate) :
		m_minTime(std::chrono::high_resolution_clock::now()),
		m_recordRate(record_rate),
		m_recordTime(time)
	{
		Resize(time / m_recordRate + 1);
	}

	~ProfilerGraph()
	{

	}

	// Resize the graph data storage size
	void Resize(size_t size)
	{
		m_nRecords = size;

		T empty = 0;
		auto now = std::chrono::high_resolution_clock::now();
		m_data.resize(m_nRecords, std::make_pair(empty, now));

		m_maxTime = now;
		m_minIndex = m_maxIndex = 0;
	}

	// Adds a measurement to the graph
	void AddMeasurement(T&& record)
	{
		auto now = std::chrono::high_resolution_clock::now();

		if (std::chrono::duration<double>(now - m_maxTime).count() < m_recordRate)
		{
			// Record sampled too quick
			return;
		}

		m_maxTime = now;
		m_minTime = m_data[m_currentRecord].second;
		m_data[m_currentRecord++] = std::make_pair(record, now);

		if (m_currentRecord >= m_nRecords)
			m_currentRecord = 0;
	}

	typedef std::chrono::time_point<std::chrono::high_resolution_clock> time_point_t;
	typedef std::vector<std::pair<T, time_point_t>>	data_t;

	data_t& GetData()
	{
		return m_data;
	}

	size_t	GetRecordsSize()
	{
		return m_nRecords;
	}

	size_t	GetNewestIndex()
	{
		return (m_currentRecord == 0) ? m_nRecords - 1 : m_currentRecord - 1;
	}

	double  GetRecordTime()
	{
		return m_recordTime;
	}

private:
	data_t m_data;

	size_t	m_currentRecord;
	size_t	m_nRecords;

	time_point_t	m_minTime;
	time_point_t	m_maxTime;

	size_t	m_minIndex;
	size_t	m_maxIndex;

	// minimum time in seconds between records
	double	m_recordRate = 0.1;

	// recording duration
	double	m_recordTime;
};

#endif // !PROFILERGRAPH_H_
