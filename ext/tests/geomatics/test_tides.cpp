//==============================================================================
//
//  This file is part of GNSSTk, the ARL:UT GNSS Toolkit.
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
//  Copyright 2004-2022, The Board of Regents of The University of Texas System
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

/// @file test_tides.cpp
/// Compute and dump solid earth tide, pole tide and ocean loading tide.

// system includes
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
// gnsstk
#include "Exception.hpp"
#include "EphTime.hpp"
#include "Position.hpp"
// geomatics
#include "CommandLine.hpp"
#include "logstream.hpp"
#include "singleton.hpp"
#include "expandtilde.hpp"
#include "SolarPosition.hpp"
#include "SolarSystem.hpp"
#include "SunEarthSatGeometry.hpp"
#include "OceanLoadTides.hpp"
#include "AtmLoadTides.hpp"
#include "logstream.hpp"

using namespace std;
using namespace gnsstk;
using namespace StringUtils;

//------------------------------------------------------------------------------------
static const string tidesVersion("4.0 12/3/19");

//------------------------------------------------------------------------------------
// data input from command line
class InputConfig : public Singleton<InputConfig> {
public:
   // input data
   bool doSimple,doOcean,doSolid,doPole,doAtm;
   string logfile, SSEfile, earthfile, oceanfile, atmfile;
   string fmtGPS,fmtCAL,fmt;
   ofstream oflog;
   EphTime beg, end;
   int begmjd,endmjd;
   bool help,verbose;
   int prec, debug, iersyear;          // debug output - prints all the data
   double dt;
   string refPosstr;
   vector<string> oceannames,atmnames;
   Position posset,posotl,posatm;
   SolarSystem SolSys;
   OceanLoadTides oceanStore;
   AtmLoadTides atmStore;

   // for CommandLine::ProcessCommandLine()
   string cmdlineUsage, cmdlineErrors, cmdlineDump;
   vector<string> cmdlineUnrecog;

   // ctor with defaults
   InputConfig(void) throw() {
      fmtGPS = string("%F,%g");
      fmtCAL = string("%Y,%m,%d,%02H,%M,%f");
      fmt = string("%4F %10.3g %4Y %2m %2d %2H %2M %6.3f");
      prec = 5;
      help = verbose = false;
      debug = -1;
      begmjd = 55007.0;
      endmjd = 55008.0;
      dt = 900.0;                // 15 minutes
      refPosstr = string("-740289.9049,-5457071.7352,3207245.5544  #ARL.2012.0000");
      iersyear = 2010;
      // add options to make it look like three programs
      doPole = doSolid = doOcean = doAtm = false;
      // make default NOT to use SSE and earth
      doSimple = true;
      //SSEfile = string("/home/btolman/.grits/SolarSystem1960to2020.405.bin");
      //iersyear = 2010;   // or 2003
      //SSEfile = string("/home/btolman/.grits/SolarSystem1960to2020.405.bin");
      //earthfile = string("/home/btolman/.grits/finals2000A.data");
      //oceanfile = string("/home/btolman/.grits/ocean.blq");
   }

}; // end class InputConfig

//------------------------------------------------------------------------------------
// prototypes
/**
 * @throw Exception
 */
int GetCommandLine(int argc, char **argv);

//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
try {
   InputConfig& C=InputConfig::Instance();
   int i, iret;

      // Title and description
   string Title =
      "tides, a program to compute solid Earth, ocean loading and pole tides, Ver "
      + tidesVersion;
   LOG(INFO) << Title;

   // TEMP, for debugging CommandLine;
   //LOGlevel = ConfigureLOG::Level("DEBUG");

   iret = GetCommandLine(argc, argv);
   LOG(DEBUG) << "GetCommandLine returned " << iret;
   //LOG(INFO) << "LOG level is " << LOGlevel;

   // return with help or errors - print to the screen - LOG is cout here
   if(iret) {
      if(iret == 1) {                    // help, else -1 == error
         if(!C.cmdlineUsage.empty()) { LOG(INFO) << C.cmdlineUsage; }
         if(!C.cmdlineDump.empty()) { LOG(INFO) << endl << C.cmdlineDump; }
      }
      else
         LOG(ERROR) << C.cmdlineErrors;
      return iret;
   }

   // open output file
   if(!C.logfile.empty()) {
      C.oflog.open(C.logfile.c_str(),ios_base::out);
      if(!C.oflog.is_open()) {
         LOG(ERROR) << "Failed to open log file " << C.logfile;
         return -1;
      }
      else {
         C.oflog.exceptions(ios::failbit);
         LOG(INFO) << "Output directed to file " << C.logfile;
         pLOGstrm = &C.oflog; // ConfigureLOG::Stream() = &C.oflog;
         ConfigureLOG::ReportLevels() = false;
         ConfigureLOG::ReportTimeTags() = false;
         // debug and verbose handled earlier in GetCommandLine
         LOG(INFO) << Title;
      }
   }

   //// turn off screen if output is cout
   //if(pLOGstrm == &cout) C.screen = false;

   // dump configuration
   LOG(VERBOSE) << endl << C.cmdlineDump;

   // -------------------------------------------------------------------------
   // initialize
   bool isValid(true);

   if(!C.SSEfile.empty()) {
      C.doSimple = false;

      // initialize solar system
      if(C.iersyear == 1996) C.SolSys.setConvention(IERSConvention::IERS1996);
      if(C.iersyear == 2003) C.SolSys.setConvention(IERSConvention::IERS2003);
      if(C.iersyear == 2010) C.SolSys.setConvention(IERSConvention::IERS2010);

      // read solar system ephemeris file
      expand_filename(C.SSEfile);
      C.SolSys.initializeWithBinaryFile(C.SSEfile);
      LOG(INFO) << "Solar System Ephemeris is DE"
         << C.SolSys.EphNumber() << "; timespan "
         << C.SolSys.startTime() << " to " << C.SolSys.endTime()
         << " with " << C.SolSys.getConvention();

      // read EarthOP file and fill store
      expand_filename(C.earthfile);
      C.SolSys.addFile(C.earthfile);

      // trim the EOP list, as we might be using e.g. finals2000A.data (huge);
      // trim generously so ephemeris limits the data, not EOP
      if(C.beg.dMJD()-10 > C.SolSys.getFirstTimeMJD() ||
         C.end.dMJD()+10 < C.SolSys.getLastTimeMJD()) {
            //LOG(VERBOSE) << "Trim EOP store based on input time limits.";
            C.SolSys.edit(C.beg.lMJD()-10, C.end.lMJD()+10);
      }

      // dump the EOP summary
      if(C.verbose) C.SolSys.dump(C.debug > -1 ? 1 : 0, LOGstrm);
   }
   else {
      LOG(INFO) << "Solar System Ephemeris is simple Solar Position";
   }

   // doSimple and doPole inconsistent
   if(C.doSimple && C.doPole) {
      LOG(ERROR) << "Error - pole option requires SSEfile and earthfile; abort.";
      isValid = false;
   }

   // fill ocean store
   if(C.doOcean && (C.oceanfile.empty() || C.oceannames.size()==0)) {
      // no ocean file and doOcean inconsistent
      LOG(ERROR) << "Error - ocean option requires oceanfile and oceansite; abort.";
      isValid = false;
   }
   else if(C.doOcean) {
      vector<string> sites(C.oceannames);
      // add the ocean file(s) and name(s) to the store
      try {
         C.oceanStore.initializeSites(sites,C.oceanfile);
      }
      catch(Exception& e) {
         LOG(ERROR) << "Error - failed to open ocean loading file: "
            << C.oceanfile << " :\n" << e.what();
         isValid = false;
      }
      catch(exception& e) {
         LOG(ERROR) << "Error - failed to open ocean loading file: "
            << C.oceanfile << " :\n" << e.what();
         isValid = false;
      }

      // get the site
      for(i=0; i<C.oceannames.size(); i++) {
         if(C.oceannames[i].empty())
            isValid = false;
         else try {
            Triple pos = C.oceanStore.getPosition(C.oceannames[i]);
            if(pos[0] == 0.0 && pos[1] == 0.0) {
               LOG(ERROR) << "Error - Failed to find ocean site name "
                              << C.oceannames[i];
               isValid = false;
            }
            else LOG(VERBOSE) << "Found ocean loading site " << C.oceannames[i]
                     << " at position " << pos[0] << "N, " << pos[1] << "E";

            C.posotl.setGeodetic(pos[0],pos[1],0.0);

         }
         catch(Exception& e) {
            LOG(ERROR) << "Error - failed to get ocean loading site: "
               << C.oceannames[i] << " from ocean loading files" << " :\n"<< e.what();
            isValid = false;
         }
         catch(exception& e) {
            LOG(ERROR) << "Error - failed to get ocean loading site: "
               << C.oceannames[i] << " from ocean loading files\n" << e.what();
            isValid = false;
         }
      }
   }

   // fill atmospheric loading store
   if(C.doAtm && (C.atmfile.empty() || C.atmnames.size()==0)) {
      // no atm file and doAtm inconsistent
      LOG(ERROR) << "Error - atm option requires atmfile and atmsite; abort.";
      isValid = false;
   }
   else if(C.doAtm) {
      vector<string> sites(C.atmnames);
      // add the atm file(s) and name(s) to the store
      try {
         C.atmStore.initializeSites(sites,C.atmfile);
      }
      catch(Exception& e) {
         LOG(ERROR) << "Error - failed to open atm loading file: "
            << C.atmfile << " :\n" << e.what();
         isValid = false;
      }
      catch(exception& e) {
         LOG(ERROR) << "Error - failed to open atm loading file: "
            << C.atmfile << " :\n" << e.what();
         isValid = false;
      }

      // get the site
      for(i=0; i<C.atmnames.size(); i++) {
         if(C.atmnames[i].empty())
            isValid = false;
         else try {
            Triple pos = C.atmStore.getPosition(C.atmnames[i]);
            if(pos[0] == 0.0 && pos[1] == 0.0) {
               LOG(ERROR) << "Error - Failed to find atm site name "
                              << C.atmnames[i];
               isValid = false;
            }
            else LOG(VERBOSE) << "Found atm loading site " << C.atmnames[i]
                     << " at position " << pos[0] << "N, " << pos[1] << "E";

            C.posatm.setGeodetic(pos[0],pos[1],0.0);

         }
         catch(Exception& e) {
            LOG(ERROR) << "Error - failed to get atm loading site: "
               << C.atmnames[i] << " from atm loading files" << " :\n"<< e.what();
            isValid = false;
         }
         catch(exception& e) {
            LOG(ERROR) << "Error - failed to get atm loading site: "
               << C.atmnames[i] << " from atm loading files\n" << e.what();
            isValid = false;
         }
      }
   }

   // get rotation matrix XYZ->NEU for pos
   Matrix<double> Rotate,RotOTL,RotATM;
   Rotate = NorthEastUp(C.posset);
   RotOTL = NorthEastUp(C.posotl);
   RotATM = NorthEastUp(C.posatm);

   // -------------------------------------------------------------------------
   if(!isValid) {
      if(C.oflog.is_open()) C.oflog.close();
      return iret;
   }

   // do it
   if(C.doSolid) {
   LOG(INFO) << "SET   MJD HH:MM:SS.sss "
            << "SET_X_cm  SET_Y_cm  SET_Z_cm  SET_N_cm  SET_E_cm  SET_U_cm";
   }
   if(C.doOcean) {
      LOG(INFO) << "OLT   MJD HH:MM:SS.sss "
            << "OLT_X_cm  OLT_Y_cm  OLT_Z_cm  OLT_N_cm  OLT_E_cm  OLT_U_cm  site";
   }
   if(C.doPole) {
      LOG(INFO) << "POT   MJD HH:MM:SS.sss "
            << "POT_X_cm  POT_Y_cm  POT_Z_cm  POT_N_cm  POT_E_cm  POT_U_cm";
   }
   if(C.doAtm) {
      LOG(INFO) << "ATL   MJD HH:MM:SS.sss "
            << "ATL_X_cm  ATL_Y_cm  ATL_Z_cm  ATL_N_cm  ATL_E_cm  ATL_U_cm  site";
   }

   // loop over times
   int w(C.prec+3);
   double arad;
   double mjd;
   EphTime ttag;
   Vector<double> XYZ(3),NEU(3);
   Triple dXYZ,dNEU;
   for(mjd=static_cast<double>(C.begmjd);
       mjd<static_cast<double>(C.endmjd); mjd += C.dt/SEC_PER_DAY)
   {
      ttag.setMJD(mjd);
      ttag.setTimeSystem(TimeSystem::UTC);
      if(C.doSolid) {
         if(C.doSimple) {
            Position Sun(SolarPosition(ttag, arad));
            Position Moon(LunarPosition(ttag, arad));
            dXYZ = computeSolidEarthTides(C.posset, ttag, Sun, Moon);
         }
         else {
            dXYZ = C.SolSys.computeSolidEarthTides(C.posset, ttag);
         }
         for(i=0; i<3; i++) XYZ(i) = dXYZ[i];
         NEU = Rotate * XYZ;

         LOG(INFO) << "SET " << ttag.asMJDString()
            << fixed << setprecision(C.prec)
            << " " << setw(w) << XYZ(0)*100.
            << " " << setw(w) << XYZ(1)*100.
            << " " << setw(w) << XYZ(2)*100.
            << " " << setw(w) << NEU(0)*100.
            << " " << setw(w) << NEU(1)*100.
            << " " << setw(w) << NEU(2)*100.;
      }

      if(C.doOcean) {
         for(size_t j=0; j<C.oceannames.size(); j++) {
            dNEU = C.oceanStore.computeDisplacement(C.oceannames[j], ttag);
            for(i=0; i<3; i++) NEU(i) = dNEU[i];
            XYZ = transpose(RotOTL) * NEU;

            LOG(INFO) << "OLT " << ttag.asMJDString()
               << fixed << setprecision(C.prec)
               << " " << setw(w) << XYZ(0)*100.
               << " " << setw(w) << XYZ(1)*100.
               << " " << setw(w) << XYZ(2)*100.
               << " " << setw(w) << NEU(0)*100.
               << " " << setw(w) << NEU(1)*100.
               << " " << setw(w) << NEU(2)*100.
               << "  " << C.oceannames[j];
         }
      }

      if(C.doPole) {
         dXYZ = C.SolSys.computePolarTides(C.posset, ttag);
         for(i=0; i<3; i++) XYZ(i) = dXYZ[i];
         NEU = Rotate * XYZ;

         LOG(INFO) << "POT " << ttag.asMJDString()
            << fixed << setprecision(C.prec)
            << " " << setw(w) << XYZ(0)*100.
            << " " << setw(w) << XYZ(1)*100.
            << " " << setw(w) << XYZ(2)*100.
            << " " << setw(w) << NEU(0)*100.
            << " " << setw(w) << NEU(1)*100.
            << " " << setw(w) << NEU(2)*100.;
      }

      if(C.doAtm) {
         for(size_t j=0; j<C.atmnames.size(); j++) {
            dNEU = C.atmStore.computeDisplacement(C.atmnames[j], ttag);
            for(i=0; i<3; i++) NEU(i) = dNEU[i];
            XYZ = transpose(RotATM) * NEU;

            LOG(INFO) << "ATL " << ttag.asMJDString()
               << fixed << setprecision(C.prec)
               << " " << setw(w) << XYZ(0)*100.
               << " " << setw(w) << XYZ(1)*100.
               << " " << setw(w) << XYZ(2)*100.
               << " " << setw(w) << NEU(0)*100.
               << " " << setw(w) << NEU(1)*100.
               << " " << setw(w) << NEU(2)*100.
               << "  " << C.atmnames[j];
         }
      }

   }  // end loop over times

   if(C.oflog.is_open()) C.oflog.close();

   return iret;
}
catch(gnsstk::Exception& e) { cerr << "Exception: " << e; }
catch (...) { cerr << "Unknown exception.  Abort." << endl; }
   return 1;
}   // end main()

//------------------------------------------------------------------------------------
int GetCommandLine(int argc, char **argv)
{
try {
   InputConfig& C=InputConfig::Instance();
   int i;

   // ---------------------------------------------------------------
   // create list of command line options, and fill it
   CommandLine opts;

   // build the command line == syntax page
   opts.DefineUsageString("tides [options]");
   string PrgmDesc =
   "Prgm tides computes tides (solid earth, ocean loading, pole) for a given\n"
   " time (UTC) and site, and dumps them to the screen.\n"
   " NB One or more of options (solid ocean pole atm) must be provided.\n"
   " NB ocean option requires oceanfile and oceansite.\n"
   " NB atm option requires atmfile and atmsite.\n"
   " NB pole requires SSEfile and earthfile.\n"
   " NB SSEfile and earthfile are optional (unless pole); they are more accurate.\n"
   " Input is on the command line, or of the same format in a file (see --file);\n"
   " lines in that file which begin with '#' are ignored.\n"
   " Options are shown below, with a description and default value, if any, in ().\n"
   ;

   // opts.Add(char, opt, arg, repeat?, required?, &target, pre-descript, descript.);
   bool req(false);  // obs is not req'd b/c filenames can appear alone (Unrecog)
   // optional args
   string dummy("");         // dummy for --file
   opts.Add(0, "solid", "", false, req, &C.doSolid,
            "# Computation: Require one or more of the following:",
            "Output Solid Earth tide");
   opts.Add(0, "ocean", "", false, req, &C.doOcean, "",
            "Output Ocean loading [requires oceanfile and oceansite]");
   opts.Add(0, "pole", "", false, req, &C.doPole, "",
            "Output Polar tide [requires SSEfile and earthfile]");
   opts.Add(0, "atm", "", false, req, &C.doAtm, "",
            "Output Atmospheric loading [requires atmfile and atmsite]");
   opts.Add('f', "file", "name", true, req, &dummy, "# File I/O:",
            "Name of file containing more options [#-EOL = comment]");
   opts.Add('o', "log", "fn", false, req, &C.logfile, "",
            "Output the summary to a file named <fn>");
   opts.Add(0, "start", "mjd", false, req, &C.begmjd, "",
            "Start processing the input data at this MJD");
   opts.Add(0, "stop", "mjd", false, req, &C.endmjd, "",
            "Stop processing the input data at this MJD");
   opts.Add(0, "dt", "sec", false, req, &C.dt, "",
            "Timestep in seconds");
   opts.Add(0, "refPos", "X,Y,Z", false, req, &C.refPosstr, "",
            "Position for SET (ECEF XYZ)");
   opts.Add(0, "IERS", "year", false, req, &C.iersyear, "",
            "Year of IERS convention: 1996, 2003 or 2010");
   opts.Add(0, "SSEfile", "fn", false, req, &C.SSEfile, "",
            "Solar System ephemeris binary file name [else use simple ephem]");
   opts.Add(0, "earthfile", "fn", false, req, &C.earthfile, "",
            "Earth orientation parameter file name [if & only if --SSEfile]");
   opts.Add(0, "oceanfile", "fn", true, req, &C.oceanfile, "",
            "Ocean loading file name");
   opts.Add(0, "oceansite", "name", true, req, &C.oceannames, "",
            "Site name in ocean loading file");
   opts.Add(0, "atmfile", "fn", true, req, &C.atmfile, "",
            "Ocean loading file name");
   opts.Add(0, "atmsite", "name", true, req, &C.atmnames, "",
            "Site name in atmospheric loading file");
   opts.Add(0, "timefmt", "f", false, req, &C.fmt, "# Output",
            "Output format for time tag");
   opts.Add('p', "prec", "n", false, req, &C.prec, "",
            "Output precision for offsets");
   opts.Add('d', "debug", "", false, req, &C.debug, "",
            "Print debug output at level 0 [debug<n> for level n=1-7]");
   opts.Add(0, "verbose", "", false, req, &C.verbose, "",
            "print extended output information");
   opts.Add('h', "help", "", false, req, &C.help, "",
            "Print this syntax page and quit");

   // options to ignore
   //opts.Add_ignore_off("--Scan");
   // deprecated args
   //opts.Add_deprecated("--dummy","--thisisatest");
   // undocumented args
   //opts.Add(0, "progress", "", false, req, &C.progress, "", "", false);

   // ---------------------------------------------------------------
   // declare it and parse it; write all errors to string C.cmdlineErrors
   int iret = opts.ProcessCommandLine(argc, argv, PrgmDesc,
                                 C.cmdlineUsage, C.cmdlineErrors, C.cmdlineUnrecog);
   if(iret == -2) return -1;     // bad alloc
   if(iret == -3) return -1;     // invalid command line

   // ---------------------------------------------------------------
   // do extra parsing - append errors to C.cmdlineErrors
   string msg;
   vector<string> fields;
   ostringstream oss;

   // unrecognized arguments are an error
   if(C.cmdlineUnrecog.size() > 0) {
      oss << " Error - unrecognized arguments:\n";
      for(i=0; i<C.cmdlineUnrecog.size(); i++)
         oss << C.cmdlineUnrecog[i] << "\n";
      oss << " End of unrecognized arguments\n";
   }

   // start and stop times
   C.beg.setMJD(C.begmjd);
   C.end.setMJD(C.endmjd);

   // reference position
   if(!C.refPosstr.empty()) {
      fields = StringUtils::split(C.refPosstr,',');
      if(fields.size() != 3)
         oss << "Error - invalid field in --refPos input: " << C.refPosstr << endl;
      else {
         try { C.posset.setECEF(StringUtils::asDouble(fields[0]),
                             StringUtils::asDouble(fields[1]),
                             StringUtils::asDouble(fields[2]));
         }
         catch(Exception& e) {
            oss << "Error - invalid position in --refPos input: "
               << C.refPosstr << endl;
         }
      }
   }

   if(C.iersyear != 1996 && C.iersyear != 2003 && C.iersyear != 2010) {
      oss << "Error - invalid field in --IERS input: " << C.iersyear
            << " - use 1996, 2003 or 2010." << endl;
   }

   // append errors onto cmdlineErrors
   C.cmdlineErrors += oss.str();
   stripTrailing(C.cmdlineErrors,'\n');

   // ---------------------------------------------------------------
   // dump a summary of command line configuration
   if(C.verbose) {
      oss.str("");                  // clear it
      oss << "------ Summary of tides command line configuration --------" << endl;
      opts.DumpConfiguration(oss);
      oss << endl << "   Begin time is " << C.beg.asMJDString() << endl;
      oss << "   End time is " << C.end.asMJDString() << endl;
      oss << "   Position is "
         << C.posset.printf("ECEF %.4x %.4y %.4z meters") << endl;
      oss << "------ End configuration summary --------" << endl;
      C.cmdlineDump = oss.str();
      stripTrailing(C.cmdlineDump,'\n');
   }

   // ---------------------------------------------------------------
   // return
   if(!C.cmdlineErrors.empty()) return -1;      // errors
   if(C.help) return 1;                         // help

   return 0;                                    // ok
}
catch(Exception& e) { GNSSTK_RETHROW(e); }
catch(exception& e) { Exception E("std except: "+string(e.what())); GNSSTK_THROW(E); }
catch(...) { Exception e("Unknown exception"); GNSSTK_THROW(e); }
   return -1;
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
