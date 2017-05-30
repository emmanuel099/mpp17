#include "SkipListStatistics.h"

#include <algorithm>

SkipListStatistics::SkipListStatistics()
{
    reset();
}

void SkipListStatistics::reset()
{
    m_numberOfInsertions = 0;
    m_numberOfInsertionRetries = 0;
    m_maxRetriesDuringInsert = 0;
    m_numberOfFailedInsertions = 0;
    m_numberOfSuccessfulInsertions = 0;

    m_numberOfDeletions = 0;
    m_numberOfDeletionRetries = 0;
    m_maxRetriesDuringDeletion = 0;
    m_numberOfFailedDeletions = 0;
    m_numberOfSuccessfulDeletions = 0;

    m_numberOfLookups = 0;
    m_numberOfLookupRetries = 0;
    m_maxRetriesDuringLookup = 0;
}

void SkipListStatistics::insertionStart()
{
    ++m_numberOfInsertions;
    m_insertionRetryCounter = 0;
}

void SkipListStatistics::insertionRetry()
{
    ++m_numberOfInsertionRetries;
    ++m_insertionRetryCounter;
}

void SkipListStatistics::insertionFailure()
{
    ++m_numberOfFailedInsertions;
    m_maxRetriesDuringInsert =
        std::max(m_maxRetriesDuringInsert, m_insertionRetryCounter);
}

void SkipListStatistics::insertionSuccess()
{
    ++m_numberOfSuccessfulInsertions;
    m_maxRetriesDuringInsert =
        std::max(m_maxRetriesDuringInsert, m_insertionRetryCounter);
}

void SkipListStatistics::deletionStart()
{
    ++m_numberOfDeletions;
    m_deletionRetryCounter = 0;
}

void SkipListStatistics::deletionRetry()
{
    ++m_numberOfDeletionRetries;
    ++m_deletionRetryCounter;
}

void SkipListStatistics::deletionFailure()
{
    ++m_numberOfFailedDeletions;
    m_maxRetriesDuringDeletion =
        std::max(m_maxRetriesDuringDeletion, m_deletionRetryCounter);
}

void SkipListStatistics::deletionSuccess()
{
    ++m_numberOfSuccessfulDeletions;
    m_maxRetriesDuringDeletion =
        std::max(m_maxRetriesDuringDeletion, m_deletionRetryCounter);
}

void SkipListStatistics::lookupStart()
{
    ++m_numberOfLookups;
    m_lookupRetryCounter = 0;
}

void SkipListStatistics::lookupRetry()
{
    ++m_numberOfLookupRetries;
    ++m_lookupRetryCounter;
}

void SkipListStatistics::lookupDone()
{
    m_maxRetriesDuringLookup =
        std::max(m_maxRetriesDuringLookup, m_lookupRetryCounter);
}

void SkipListStatistics::mergeInto(SkipListStatistics& other) const
{
    other.m_numberOfInsertions += m_numberOfInsertions;
    other.m_numberOfInsertionRetries += m_numberOfInsertionRetries;
    other.m_maxRetriesDuringInsert =
        std::max(m_maxRetriesDuringInsert, other.m_maxRetriesDuringInsert);
    other.m_numberOfFailedInsertions += m_numberOfFailedInsertions;
    other.m_numberOfSuccessfulInsertions += m_numberOfSuccessfulInsertions;

    other.m_numberOfDeletions += m_numberOfDeletions;
    other.m_numberOfDeletionRetries += m_numberOfDeletionRetries;
    other.m_maxRetriesDuringDeletion =
        std::max(m_maxRetriesDuringDeletion, other.m_maxRetriesDuringDeletion);
    other.m_numberOfFailedDeletions += m_numberOfFailedDeletions;
    other.m_numberOfSuccessfulDeletions += m_numberOfSuccessfulDeletions;

    other.m_numberOfLookups += m_numberOfLookups;
    other.m_numberOfLookupRetries += m_numberOfLookupRetries;
    other.m_maxRetriesDuringLookup =
        std::max(m_maxRetriesDuringLookup, other.m_maxRetriesDuringLookup);
}

SkipListStatistics& SkipListStatistics::threadLocalInstance()
{
    static thread_local SkipListStatistics statistics;
    return statistics;
}

std::size_t SkipListStatistics::numberOfInserts() const
{
    return m_numberOfInsertions;
}

double SkipListStatistics::averageNumberOfRetriesDuringInsert() const
{
    return static_cast<double>(m_numberOfInsertionRetries) /
           m_numberOfInsertions;
}

std::size_t SkipListStatistics::maximumNumberOfRetriesDuringInsert() const
{
    return m_maxRetriesDuringInsert;
}

std::size_t SkipListStatistics::numberOfDeletions() const
{
    return m_numberOfDeletions;
}

double SkipListStatistics::averageNumberOfRetriesDuringDeletion() const
{
    return static_cast<double>(m_numberOfDeletionRetries) / m_numberOfDeletions;
}

std::size_t SkipListStatistics::maximumNumberOfRetriesDuringDeletion() const
{
    return m_maxRetriesDuringDeletion;
}

std::size_t SkipListStatistics::numberOfLookups() const
{
    return m_numberOfLookups;
}

double SkipListStatistics::averageNumberOfRetriesDuringLookup() const
{
    return static_cast<double>(m_numberOfLookupRetries) / m_numberOfLookups;
}

std::size_t SkipListStatistics::maximumNumberOfRetriesDuringLookup() const
{
    return m_maxRetriesDuringLookup;
}
