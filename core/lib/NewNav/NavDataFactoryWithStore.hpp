//==============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  This software was developed by Applied Research Laboratories at the 
//  University of Texas at Austin.
//  Copyright 2004-2021, The Board of Regents of The University of Texas System
//
//==============================================================================


//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the 
//  University of Texas at Austin, under contract to an agency or agencies 
//  within the U.S. Department of Defense. The U.S. Government retains all 
//  rights to use, duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================
#ifndef GPSTK_NAVDATAFACTORYWITHSTORE_HPP
#define GPSTK_NAVDATAFACTORYWITHSTORE_HPP

#include "NavDataFactory.hpp"
#include "TimeOffsetData.hpp"

namespace gpstk
{
      /// @ingroup NavFactory
      //@{

      /** Define methods and data structures for internally storing
       * navigation message data internally as read from some
       * source.
       * @todo Currently it's possible for health messages from one
       *   signal to stomp health messages on another signal.
       *   Specific example: if you have a CNAV message with the three
       *   signal health bits that get split up into L1, L2 and L5,
       *   it's possible for the L1 signal from the CNAV message to
       *   overwrite the health status from an LNAV message for L1 and
       *   vice versa.  Since it's possible for the health bits to be
       *   different, we probably need to decide if we need to do
       *   something about this issue and if so, what. */
   class NavDataFactoryWithStore : public NavDataFactory
   {
   public:
         /// Initialize internal data.
      NavDataFactoryWithStore();

         /// Clean up.
      virtual ~NavDataFactoryWithStore()
      {
      }

         /** Search the store to find the navigation message that meets
          * the specified criteria.
          * @note In order for xmitHealth matching to occur, one must
          *   have loaded health information, meaning that health data
          *   must have been included in the type filter.  For formats
          *   like SP3, which contain no health information, you won't
          *   get any results if you specify "Healthy", "Unhealthy" or
          *   "Degraded".  Use this option with care.
          * @param[in] nmid Specify the message type, satellite and
          *   codes to match.
          * @param[in] when The time of interest to search for data.
          * @param[in] xmitHealth The desired health status of the
          *   transmitting satellite.
          * @param[out] navOut The resulting navigation message.
          * @param[in] valid Specify whether to search only for valid
          *   or invalid messages, or both.
          * @param[in] order Specify whether to search by receiver
          *   behavior or by nearest to when in time. 
          * @return true if successful.  If false, navData will be untouched. */
      bool find(const NavMessageID& nmid, const CommonTime& when,
                NavDataPtr& navOut, SVHealth xmitHealth, NavValidityType valid,
                NavSearchOrder order) override;

         /** Get the offset, in seconds, to apply to times when
          * converting them from fromSys to toSys.
          * @param[in] fromSys The time system to convert from.
          * @param[in] toSys The time system to convert to.
          * @param[in] when The time being converted, usually in the
          *   time system appropriate for a given nav message source.
          *   The details of what time system this should be in and
          *   any other restrictions will be documented in each leaf
          *   class, e.g. GPSLNavTimeOffset.
          * @param[out] offset The offset when converting fromSys->toSys.
          * @param[in] xmitHealth The desired health status of the
          *   transmitting satellite.
          * @param[in] valid Specify whether to search only for valid
          *   or invalid messages, or both.
          * @return true if an offset is available, false if not. */
      bool getOffset(TimeSystem fromSys, TimeSystem toSys,
                     const CommonTime& when, NavDataPtr& offset,
                     SVHealth xmitHealth = SVHealth::Any,
                     NavValidityType valid = NavValidityType::ValidOnly)
         override;

         /** Remove all data from the internal storage in the time
          * span [fromTime,toTime).
          * @param[in] fromTime The earliest time to be removed.
          * @param[in] toTime The earliest time that will NOT be removed.
          */
      void edit(const CommonTime& fromTime, const CommonTime& toTime) override;

         /** Remove data for a specific satellite signal from the
          * internal storage in the time span [fromTime,toTime).
          * @param[in] fromTime The earliest time to be removed.
          * @param[in] toTime The earliest time that will NOT be removed.
          * @param[in] satID The complete signal specification for the
          *   data to be removed (subject satellite, transmit
          *   satellite, system, carrier, code, nav).
          */
      void edit(const CommonTime& fromTime, const CommonTime& toTime,
                const NavSatelliteID& satID) override;

         /** Remove data for all satellites matching a specific signal
          * from the internal storage in the time span
          * [fromTime,toTime).
          * @param[in] fromTime The earliest time to be removed.
          * @param[in] toTime The earliest time that will NOT be removed.
          * @param[in] signal The signal for the data to be removed
          *   (system, carrier, code, nav).
          */
      void edit(const CommonTime& fromTime, const CommonTime& toTime,
                const NavSignalID& signal) override;

         /// Remove all data from the internal store.
      void clear() override;

         /** Add a nav message to the internal store (data).
          * @param[in] nd The nav data to add.
          * @return true if successful. */
      bool addNavData(const NavDataPtr& nd);

         /** Determine the earliest time for which this object can successfully 
          * determine the Xvt for any object.
          * @return The initial time, or CommonTime::END_OF_TIME if no
          *   data is available. */
      CommonTime getInitialTime() const override
      { return initialTime; }

         /** Determine the latest time for which this object can successfully 
          * determine the Xvt for any object.
          * @return The initial time, or CommonTime::BEGINNING_OF_TIME if no
          *   data is available. */
      CommonTime getFinalTime() const override
      { return finalTime; }

         /// Return the number of nav messages in data.
      virtual size_t size() const;
         /// Return the number of distinct signals (ignoring PRN) in the data.
      virtual size_t numSignals() const;
         /// Return the number of distinct signals including PRN, in the data.
      virtual size_t numSatellites() const;
         /** Print the contents of this store in a human-readable
          * format.
          * @param[in,out] s The stream to write the data to.
          * @param[in] dl The level of detail the output should contain. */
       void dump(std::ostream& s, DumpDetail dl) const override;

   protected:
         /** Search the store to find the navigation message that meets
          * the specified criteria using User-oriented data.
          * @note In order for xmitHealth matching to occur, one must
          *   have loaded health information, meaning that health data
          *   must have been included in the type filter.  For formats
          *   like SP3, which contain no health information, you won't
          *   get any results if you specify "Healthy", "Unhealthy" or
          *   "Degraded".  Use this option with care.
          * @param[in] nmid Specify the message type, satellite and
          *   codes to match.
          * @param[in] when The time of interest to search for data.
          * @param[in] xmitHealth The desired health status of the
          *   transmitting satellite.
          * @param[out] navData The resulting navigation message.
          * @param[in] valid Specify whether to search only for valid
          *   or invalid messages, or both.
          * @return true if successful.  If false, navData will be untouched. */
      virtual bool findUser(const NavMessageID& nmid, const CommonTime& when,
                            NavDataPtr& navData, SVHealth xmitHealth,
                            NavValidityType valid);

         /** Search the store to find the navigation message that
          * meets the specified criteria using the nearest match in
          * time (e.g. nearest toe to when).
          * @note In order for xmitHealth matching to occur, one must
          *   have loaded health information, meaning that health data
          *   must have been included in the type filter.  For formats
          *   like SP3, which contain no health information, you won't
          *   get any results if you specify "Healthy", "Unhealthy" or
          *   "Degraded".  Use this option with care.
          * @param[in] nmid Specify the message type, satellite and
          *   codes to match.
          * @param[in] when The time of interest to search for data.
          * @param[in] xmitHealth The desired health status of the
          *   transmitting satellite.
          * @param[out] navData The resulting navigation message.
          * @param[in] valid Specify whether to search only for valid
          *   or invalid messages, or both.
          * @return true if successful.  If false, navData will be untouched. */
      virtual bool findNearest(const NavMessageID& nmid, const CommonTime& when,
                               NavDataPtr& navData, SVHealth xmitHealth,
                               NavValidityType valid);

         /** Performs an appropriate validity check based on the
          * desired validity.
          * @param[in] ti A container iterator pointing to the nav
          *   data to check.
          * @param[in] nm The NavMap that ti is iterating over.
          * @param[in] valid The desired validity for navigation data.
          * @param[in] xmitHealth The desired health status of the
          *   transmitting satellite.
          * @return true if the validity of the nav data pointed to by
          *   ti matches the requested validity described by valid and
          *   the health status of the transmitting satellite matches
          *   xmitHealth. */
      bool validityCheck(const NavMap::iterator& ti,
                         NavMap& nm,
                         NavValidityType valid,
                         SVHealth xmitHealth,
                         const CommonTime& when);

         /** Performs an appropriate validity check based on the
          * desired validity.
          * @param[in] ndp The NavDataPtr object whose validity is to
          *   be checked.
          * @param[in] valid The desired validity for navigation data.
          * @param[in] xmitHealth The desired health status of the
          *   transmitting satellite.
          * @return true if the validity of ndp matches the requested
          *   validity described by valid and the health status of the
          *   transmitting satellite matches xmitHealth. */
      bool validityCheck(const NavDataPtr& ndp,
                         NavValidityType valid,
                         SVHealth xmitHealth,
                         const CommonTime& when);

         /** Check the SV health status of the transmitting satellite
          * of a navigation message.
          * @param[in] ndp A pointer to the NavData to verify.
          * @param[in] xmitHealth The desired health status of the
          *   transmitting satellite.
          * @return true if the health status of the satellite that
          *   transmitted ndp matches xmitHealth. */
      bool matchHealth(NavData *ndp, SVHealth xmitHealth);

         /// Internal storage of navigation data for User searches
      NavMessageMap data;
         /// Internal storage of navigation data for Nearest searches
      NavNearMessageMap nearestData;
         // Time offset information is organized differently due to the use case
         /** Map that will contain all TimeOffsetData objects with the
          * same conversion pair broadcast at a given time. */
      using OffsetMap = std::map<NavSatelliteID, NavDataPtr>;
         /** Map from the timeStamp of a TimeOffsetData object to the
          * collection of TimeOffsetData objects. */
      using OffsetEpochMap = std::map<CommonTime, OffsetMap>;
         /// Map from the time system conversion pair to the conversion objects.
      using OffsetCvtMap = std::map<TimeCvtKey, OffsetEpochMap>;
         /** Store the time offset data separate from the other nav
          * data because searching is very different. */
      OffsetCvtMap offsetData;
         /// Store the earliest applicable orbit time here, by addNavData
      CommonTime initialTime;
         /// Store the latest applicable orbit time here, by addNavData
      CommonTime finalTime;

         /// Grant access to MultiFormatNavDataFactory for various functions.
      friend class MultiFormatNavDataFactory;
   };

      //@}

}

#endif // GPSTK_NAVDATAFACTORYWITHSTORE_HPP
