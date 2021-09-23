//==============================================================================
//
//  This file is part of GNSSTk, the GNSS Toolkit.
//
//  The GNSSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GNSSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GNSSTk; if not, write to the Free Software Foundation,
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
#include "MultiFormatNavDataFactory.hpp"
#include "BasicTimeSystemConverter.hpp"
#include "NDFUniqConstIterator.hpp"
#include "NDFUniqIterator.hpp"

namespace gnsstk
{
   MultiFormatNavDataFactory ::
   MultiFormatNavDataFactory()
   {
         // keys for factories are not unique but that doesn't really matter.
      for (const auto& i : factories())
      {
         supportedSignals.insert(i.first);
      }
   }


   MultiFormatNavDataFactory ::
   ~MultiFormatNavDataFactory()
   {
      clear();
   }


   bool MultiFormatNavDataFactory ::
   find(const NavMessageID& nmid, const CommonTime& when,
        NavDataPtr& navOut, SVHealth xmitHealth, NavValidityType valid,
        NavSearchOrder order)
   {
         // Don't use factories.equal_range(nmid), as it can result in
         // range.first and range.second being the same iterator, in
         // which case the loop won't process anything at all.
         // Also don't use the unique iterator as it will result in
         // skipping over valid factories, e.g. looking for CNAV but
         // LNAV is first in the map, the signals don't match and the
         // factory won't be looked at again.
      std::set<NavDataFactory*> uniques;
      for (auto& fi : factories())
      {
         // std::cerr << "fi.first = " << fi.first << "   nmid = " << nmid << std::endl;
         if ((fi.first == nmid) && (uniques.count(fi.second.get()) == 0))
         {
            if (fi.second->find(nmid, when, navOut, xmitHealth, valid, order))
               return true;
            uniques.insert(fi.second.get());
         }
      }
      return false;
   }


   bool MultiFormatNavDataFactory ::
   getOffset(TimeSystem fromSys, TimeSystem toSys,
             const CommonTime& when, NavDataPtr& offset,
             SVHealth xmitHealth, NavValidityType valid)
   {
      for (auto& fi : NDFUniqIterator<NavDataFactoryMap>(factories()))
      {
         if (fi.second->getOffset(fromSys, toSys, when, offset, xmitHealth,
                                   valid))
         {
            return true;
         }
      }
      return false;
   }


   void MultiFormatNavDataFactory ::
   edit(const CommonTime& fromTime, const CommonTime& toTime)
   {
      for (auto& fi : NDFUniqIterator<NavDataFactoryMap>(factories()))
      {
         fi.second->edit(fromTime,toTime);
      }
   }


   void MultiFormatNavDataFactory ::
   edit(const CommonTime& fromTime, const CommonTime& toTime,
        const NavSatelliteID& satID)
   {
      for (auto& fi : NDFUniqIterator<NavDataFactoryMap>(factories()))
      {
         fi.second->edit(fromTime,toTime,satID);
      }
   }


   void MultiFormatNavDataFactory ::
   edit(const CommonTime& fromTime, const CommonTime& toTime,
        const NavSignalID& signal)
   {
      for (auto& fi : NDFUniqIterator<NavDataFactoryMap>(factories()))
      {
         fi.second->edit(fromTime,toTime,signal);
      }
   }


   void MultiFormatNavDataFactory ::
   clear()
   {
      for (auto& fi : NDFUniqIterator<NavDataFactoryMap>(factories()))
      {
         fi.second->clear();
      }
   }


   CommonTime MultiFormatNavDataFactory ::
   getInitialTime() const
   {
         // always use basic time system converter because
         // 1) We don't need nanosecond precision for getInitialTime
         // 2) We don't want to spend a lot of time searching nav data
      BasicTimeSystemConverter btsc;
      CommonTime rv = CommonTime::END_OF_TIME;
      rv.setTimeSystem(TimeSystem::Any);
      for (const auto& fi : NDFUniqConstIterator<NavDataFactoryMap>(factories()))
      {
         CommonTime t = fi.second->getInitialTime();
         if ((rv.getTimeSystem() == TimeSystem::Any) ||
             (t.getTimeSystem() == TimeSystem::Any) ||
             (t.getTimeSystem() == rv.getTimeSystem()))
         {
            rv = std::min(rv,t);
         }
         else
         {
            t.changeTimeSystem(TimeSystem::UTC, &btsc);
            rv.changeTimeSystem(TimeSystem::UTC, &btsc);
            rv = std::min(rv,t);
         }
      }
      return rv;
   }


   CommonTime MultiFormatNavDataFactory ::
   getFinalTime() const
   {
         // always use basic time system converter because
         // 1) We don't need nanosecond precision for getFinalTime
         // 2) We don't want to spend a lot of time searching nav data
      BasicTimeSystemConverter btsc;
      CommonTime rv = CommonTime::BEGINNING_OF_TIME;
      rv.setTimeSystem(TimeSystem::Any);
      for (const auto& fi : NDFUniqConstIterator<NavDataFactoryMap>(factories()))
      {
         CommonTime t = fi.second->getFinalTime();
         if ((rv.getTimeSystem() == TimeSystem::Any) ||
             (t.getTimeSystem() == TimeSystem::Any) ||
             (t.getTimeSystem() == rv.getTimeSystem()))
         {
            rv = std::max(rv,t);
         }
         else
         {
            t.changeTimeSystem(TimeSystem::UTC, &btsc);
            rv.changeTimeSystem(TimeSystem::UTC, &btsc);
            rv = std::max(rv,t);
         }
      }
      return rv;
   }


   NavSatelliteIDSet MultiFormatNavDataFactory ::
   getAvailableSats(const CommonTime& fromTime, const CommonTime& toTime)
      const
   {
      NavSatelliteIDSet rv, tmp;
      for (const auto& fi : NDFUniqConstIterator<NavDataFactoryMap>(factories()))
      {
         tmp = fi.second->getAvailableSats(fromTime, toTime);
         for (const auto& i : tmp)
         {
            rv.insert(i);
         }
      }
      return rv;
   }


   NavSatelliteIDSet MultiFormatNavDataFactory ::
   getAvailableSats(NavMessageType nmt,
                    const CommonTime& fromTime,
                    const CommonTime& toTime)
      const
   {
      NavSatelliteIDSet rv, tmp;
      for (const auto& fi : NDFUniqConstIterator<NavDataFactoryMap>(factories()))
      {
         tmp = fi.second->getAvailableSats(nmt, fromTime, toTime);
         for (const auto& i : tmp)
         {
            rv.insert(i);
         }
      }
      return rv;
   }


   NavMessageIDSet MultiFormatNavDataFactory ::
   getAvailableMsgs(const CommonTime& fromTime,
                    const CommonTime& toTime)
      const
   {
      NavMessageIDSet rv, tmp;
      for (const auto& fi : NDFUniqConstIterator<NavDataFactoryMap>(factories()))
      {
         tmp = fi.second->getAvailableMsgs(fromTime, toTime);
         for (const auto& i : tmp)
         {
            rv.insert(i);
         }
      }
      return rv;
   }


   bool MultiFormatNavDataFactory ::
   isPresent(const NavMessageID& nmid,
             const CommonTime& fromTime,
             const CommonTime& toTime)
   {
      for (const auto& fi : NDFUniqConstIterator<NavDataFactoryMap>(factories()))
      {
         if (fi.second->isPresent(nmid, fromTime, toTime))
            return true;
      }
      return false;
   }


   size_t MultiFormatNavDataFactory ::
   size() const
   {
         // this one is easy, it's just the sum of each individual
         // factory's size
      size_t rv = 0;
      for (const auto& fi : NDFUniqConstIterator<NavDataFactoryMap>(factories()))
      {
         NavDataFactory *ndfp = fi.second.get();
         NavDataFactoryWithStore *ndfs =
            dynamic_cast<NavDataFactoryWithStore*>(ndfp);
         if (ndfs != nullptr)
         {
            rv += ndfs->size();
         }
      }
      return rv;
   }


   size_t MultiFormatNavDataFactory ::
   numSignals() const
   {
      std::set<NavSignalID> uniqueSig;
      for (const auto& fi : NDFUniqConstIterator<NavDataFactoryMap>(factories()))
      {
         NavDataFactory *ndfp = fi.second.get();
         NavDataFactoryWithStore *ndfs =
            dynamic_cast<NavDataFactoryWithStore*>(ndfp);
         if (ndfs != nullptr)
         {
            for (const auto& mti : ndfs->data)
            {
               for (const auto& sigIt : mti.second)
               {
                  uniqueSig.insert(sigIt.first);
               }
            }
         }
      }
      return uniqueSig.size();
   }


   size_t MultiFormatNavDataFactory ::
   numSatellites() const
   {
      std::set<NavSatelliteID> uniqueSat;
      for (const auto& fi : NDFUniqConstIterator<NavDataFactoryMap>(factories()))
      {
         NavDataFactory *ndfp = fi.second.get();
         NavDataFactoryWithStore *ndfs =
            dynamic_cast<NavDataFactoryWithStore*>(ndfp);
         if (ndfs != nullptr)
         {
            for (const auto& mti : ndfs->data)
            {
               for (const auto& satIt : mti.second)
               {
                  uniqueSat.insert(satIt.first);
               }
            }
         }
      }
      return uniqueSat.size();
   }


   void MultiFormatNavDataFactory ::
   setValidityFilter(NavValidityType nvt)
   {
         // This will end up setting the validity filter multiple
         // times for any factory that has multiple supported signals,
         // but the end result is the same whether we check for
         // duplicates or not.
      for (auto& i : NDFUniqIterator<NavDataFactoryMap>(factories()))
      {
         i.second->setValidityFilter(nvt);
      }
   }


   void MultiFormatNavDataFactory ::
   setTypeFilter(const NavMessageTypeSet& nmts)
   {
         // This will end up setting the type filter multiple times
         // for any factory that has multiple supported signals, but
         // the end result is the same whether we check for duplicates
         // or not.
      for (auto& i : NDFUniqIterator<NavDataFactoryMap>(factories()))
      {
         i.second->setTypeFilter(nmts);
      }
   }


   bool MultiFormatNavDataFactory ::
   addFactory(NavDataFactoryPtr& fact)
   {
         // make sure it's a valid class first
      NavDataFactory *ndfp = fact.get();
      // std::cerr << "MultiFormatNavDataFactory::addFactory() ndfp = " << ndfp << std::endl;
      if (dynamic_cast<NavDataFactoryWithStoreFile*>(ndfp) == nullptr)
      {
         return false;
      }
      if (dynamic_cast<MultiFormatNavDataFactory*>(ndfp) != nullptr)
      {
         return false;
      }
         // Yes, we do add multiple copies of the NavDataFactoryPtr to
         // the map, it's a convenience.
      for (const auto& si : fact->supportedSignals)
      {
         factories().insert(NavDataFactoryMap::value_type(si,fact));
      }
      return true;
   }


   bool MultiFormatNavDataFactory ::
   addDataSource(const std::string& source)
   {
      for (auto& fi : NDFUniqIterator<NavDataFactoryMap>(factories()))
      {
         NavDataFactory *ptr = fi.second.get();
         NavDataFactoryWithStoreFile *fact =
            dynamic_cast<NavDataFactoryWithStoreFile*>(ptr);
         if (fact != nullptr)
         {
            if (fact->addDataSource(source))
            {
               return true;
            }
         }
      }
         // none of the existing factories were able to load the data
      return false;
   }


   void MultiFormatNavDataFactory ::
   dump(std::ostream& s, DumpDetail dl) const
   {
      for (const auto& fi : NDFUniqConstIterator<NavDataFactoryMap>(factories()))
      {
         NavDataFactory *ptr = fi.second.get();
         ptr->dump(s,dl);
      }
   }


   std::string MultiFormatNavDataFactory ::
   getFactoryFormats() const
   {
      std::string rv;
      for (const auto& fi : NDFUniqConstIterator<NavDataFactoryMap>(factories()))
      {
         NavDataFactory *ptr = fi.second.get();
         std::string ff(ptr->getFactoryFormats());
         if (!ff.empty())
         {
            if (!rv.empty())
               rv += ", ";
            rv += ff;
         }
      }
      return rv;
   }


   NavDataFactoryMap& MultiFormatNavDataFactory ::
   factories()
   {
      static NavDataFactoryMap rv;
      return rv;
   }
}