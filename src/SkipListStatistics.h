#pragma once

#include <cstdint>

class SkipListStatistics
{
  public:
    SkipListStatistics();

    void reset();

    void insertionStart();
    void insertionRetry();
    void insertionFailure();
    void insertionSuccess();

    void deletionStart();
    void deletionRetry();
    void deletionFailure();
    void deletionSuccess();

    void lookupStart();
    void lookupRetry();
    void lookupDone();

    void mergeInto(SkipListStatistics& other) const;

    static SkipListStatistics& threadLocalInstance();

    std::size_t numberOfInserts() const;
    double averageNumberOfRetriesDuringInsert() const;
    std::size_t maximumNumberOfRetriesDuringInsert() const;
    double percentageFailedInserts() const;

    std::size_t numberOfDeletions() const;
    double averageNumberOfRetriesDuringDeletion() const;
    std::size_t maximumNumberOfRetriesDuringDeletion() const;
    double percentageFailedDeletions() const;

    std::size_t numberOfLookups() const;
    double averageNumberOfRetriesDuringLookup() const;
    std::size_t maximumNumberOfRetriesDuringLookup() const;

  private:
    std::size_t m_numberOfInsertions;
    std::size_t m_numberOfInsertionRetries;
    std::size_t m_maxRetriesDuringInsert;
    std::size_t m_numberOfFailedInsertions;
    std::size_t m_numberOfSuccessfulInsertions;
    std::size_t m_insertionRetryCounter; // to determine the max. no. retries

    std::size_t m_numberOfDeletions;
    std::size_t m_numberOfDeletionRetries;
    std::size_t m_maxRetriesDuringDeletion;
    std::size_t m_numberOfFailedDeletions;
    std::size_t m_numberOfSuccessfulDeletions;
    std::size_t m_deletionRetryCounter; // to determine the max. no. retries

    std::size_t m_numberOfLookups;
    std::size_t m_numberOfLookupRetries;
    std::size_t m_maxRetriesDuringLookup;
    std::size_t m_lookupRetryCounter; // to determine the max. no. retries
};
