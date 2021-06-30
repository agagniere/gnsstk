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

// This file is used in building the swig bindings of the GPSTk and is
// not really intended to be used by C++ code directly


#include "Exception.hpp"
#include "TimeConstants.hpp"
#include "TimeSystem.hpp"
#include "TimeSystemConverter.hpp"
#include "CommonTime.hpp"
#include "HexDumpDataConfig.hpp"
 //#include "StringUtils.hpp"
#include "TimeTag.hpp"
#include "ANSITime.hpp"
#include "AllanDeviation.hpp"
#include "gps_constants.hpp"
#include "EnumIterator.hpp"
#include "SatelliteSystem.hpp"
#include "SatID.hpp"
#include "MathBase.hpp"
#include "VectorBase.hpp"
#include "VectorBaseOperators.hpp"
#include "Vector.hpp"
#include "Triple.hpp"
#include "EllipsoidModel.hpp"
#include "ReferenceFrame.hpp"
#include "DeprecatedConsts.hpp"
#include "FreqConsts.hpp"
#include "GNSSconstants.hpp"
#include "Xvt.hpp"
#include "AlmOrbit.hpp"
#include "AngleReduced.hpp"
#include "Angle.hpp"
#include "FFStreamError.hpp"
#include "FFData.hpp"
#include "AntexBase.hpp"
#include "FFStream.hpp"
#include "AntexHeader.hpp"
#include "CivilTime.hpp"
#include "AntexData.hpp"
#include "FFTextStream.hpp"
#include "AntexStream.hpp"
#include "AntennaStore.hpp"
#include "DataStatus.hpp"
#include "AshtechData.hpp"
#include "AshtechALB.hpp"
#include "AshtechEPB.hpp"
#include "AshtechMBEN.hpp"
#include "AshtechPBEN.hpp"
#include "FFBinaryStream.hpp"
#include "AshtechStream.hpp"
#include "MJD.hpp"
#include "TimeConverters.hpp"
#include "EphTime.hpp"
#include "AtmLoadTides.hpp"
#include "Rinex3ObsBase.hpp"
#include "ObservationType.hpp"
#include "CarrierBand.hpp"
#include "TrackingCode.hpp"
#include "XmitAnt.hpp"
#include "ObsID.hpp"
#include "OrbitEph.hpp"
#include "BDSEphemeris.hpp"
#include "XvtStore.hpp"
#include "OrbitEphStore.hpp"
#include "BDSEphemerisStore.hpp"
#include "Week.hpp"
#include "WeekSecond.hpp"
#include "BDSWeekSecond.hpp"
#include "MatrixBase.hpp"
#include "MatrixBaseOperators.hpp"
#include "Matrix.hpp"
#include "BLQDataReader.hpp"
/* #include "CommandOption.hpp" */
/* #include "CommandOptionParser.hpp" */
/* #include "BasicFramework.hpp" */
#include "BasicTimeSystemConverter.hpp"
#include "BinexData.hpp"
#include "FileSpec.hpp"
#include "FileFilter.hpp"
#include "BinexFilterOperators.hpp"
#include "BinexStream.hpp"
#include "MiscMath.hpp"
#include "Stats.hpp"
#include "BivarStats.hpp"
#include "GPSWeekSecond.hpp"
#include "NMCTMeta.hpp"
#include "EngNav.hpp"
#include "YDSTime.hpp"
#include "BrcClockCorrection.hpp"
#include "WGS84Ellipsoid.hpp"
#include "GPSEllipsoid.hpp"
#include "RinexSatID.hpp"
#include "GPS_URA.hpp"
#include "BrcKeplerOrbit.hpp"
#include "CCIR.hpp"
#include "CGCS2000Ellipsoid.hpp"
#include "NavFilterKey.hpp"
#include "NavFilter.hpp"
#include "CNav2SanityFilter.hpp"
#include "NavType.hpp"
#include "NavID.hpp"
#include "PackedNavBits.hpp"
#include "CNavFilterData.hpp"
#include "CNavCookFilter.hpp"
#include "NavFilterMgr.hpp"
#include "CNavCrossSourceFilter.hpp"
#include "SatMetaData.hpp"
#include "SatMetaDataSort.hpp"
#include "SatMetaDataStore.hpp"
/* #include "CNavDataElement.hpp" */
/* #include "CNavDataElementStore.hpp" */
/* #include "CNavEOP.hpp" */
/* #include "CNavEmptyFilter.hpp" */
/* #include "CNavGGTO.hpp" */
/* #include "CNavISC.hpp" */
#include "OrbData.hpp"
#include "OrbDataSys.hpp"
#include "CNavMidiAlm.hpp"
#include "CNavParityFilter.hpp"
#include "CNavReducedAlm.hpp"
#include "CNavTOWFilter.hpp"
/* #include "CNavText.hpp" */
/* #include "CNavUTC.hpp" */
#include "ClockModel.hpp"
#include "TimeString.hpp"
/* #include "TabularSatStore.hpp" */
#include "FileStore.hpp"
/* #include "ClockSatStore.hpp" */
#include "PCodeConst.hpp"
#include "CodeBuffer.hpp"
#include "Combinations.hpp"
/* #include "CommandLine.hpp" */
/* #include "CommandOptionWithCommonTimeArg.hpp" */
#include "Position.hpp"
/* #include "CommandOptionWithPositionArg.hpp" */
/* #include "CommandOptionWithTimeArg.hpp" */
#include "ConfDataReader.hpp"
 /* #include "ConfDataWriter.hpp" */ // error on typemaps for list of double
#include "CubicSpline.hpp"
#include "DCBDataReader.hpp"
/* #include "DebugTrace.hpp" */
#include "DiffCorrBase.hpp"
#include "DiffCorrClk.hpp"
#include "DiffCorrEph.hpp"
#include "GPSZcount.hpp"
#include "UnixTime.hpp"
#include "SystemTime.hpp"
#include "JulianDate.hpp"
#include "GPSWeek.hpp"
#include "GPSWeekZcount.hpp"
#include "GALWeekSecond.hpp"
#include "QZSWeekSecond.hpp"
/* #include "Epoch.hpp" */
#include "RinexObsBase.hpp"
#include "RinexObsHeader.hpp"
#include "RinexDatum.hpp"
#include "RinexObsData.hpp"
#include "GPSEphemeris.hpp"
#include "RinexNavBase.hpp"
#include "EngEphemeris.hpp"
#include "FormattedDouble.hpp"
#include "RNDouble.hpp"
#include "RinexNavData.hpp"
#include "GPSEphemerisStore.hpp"
#include "SP3SatID.hpp"
#include "SP3Base.hpp"
#include "SP3Data.hpp"
/* #include "PositionSatStore.hpp" */
#include "SP3Header.hpp"
#include "Rinex3ClockBase.hpp"
#include "RinexObsID.hpp"
#include "Rinex3ClockHeader.hpp"
/* #include "SP3EphemerisStore.hpp" */
#include "RinexUtilities.hpp"
/* #include "SatPass.hpp" */
/* #include "DiscCorr.hpp" */
#include "DumpDetail.hpp"
#include "ENUUtil.hpp"
#include "IERSConvention.hpp"
/* #include "EarthOrientation.hpp" */
/* #include "EOPPrediction.hpp" */
/* #include "EOPStore.hpp" */
#include "EngAlmanac.hpp"
#include "EphemerisRange.hpp"
#include "IonoModel.hpp"
#include "IonoModelStore.hpp"
#include "ValidType.hpp"
#include "SvObsEpoch.hpp"
#include "ObsEpochMap.hpp"
#include "WxObsMap.hpp"
#include "TropModel.hpp"
#include "NBTropModel.hpp"
#include "ObsRngDev.hpp"
#include "ORDEpoch.hpp"
#include "ObsClockModel.hpp"
#include "EpochClockModel.hpp"
#include "Expression.hpp"
#include "StatsFilterHit.hpp"
#include "RobustStats.hpp"
#include "FDiffFilter.hpp"
#include "FileSpecFind.hpp"
#include "FileUtils.hpp"
#include "FileFilterFrame.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"
#include "FileFilterFrameWithHeader.hpp"
#include "FirstDiffFilter.hpp"
#include "FreqConv.hpp"
#include "GCATTropModel.hpp"
#include "GGHeightTropModel.hpp"
#include "GGTropModel.hpp"
#include "Rinex3NavBase.hpp"
#include "IRNWeekSecond.hpp"
#include "TimeSystemCorr.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavStream.hpp"
#include "PZ90Ellipsoid.hpp"
#include "GloEphemeris.hpp"
#include "GalEphemeris.hpp"
#include "QZSEphemeris.hpp"
#include "Rinex3NavData.hpp"
#include "OrbElemBase.hpp"
#include "OrbElemStore.hpp"
#include "GPSAlmanacStore.hpp"
#include "NavSignalID.hpp"
#include "NavSatelliteID.hpp"
#include "NavMessageType.hpp"
#include "NavMessageID.hpp"
#include "NavData.hpp"
#include "OrbitData.hpp"
#include "SVHealth.hpp"
#include "OrbitDataKepler.hpp"
#include "GPSCNav2Alm.hpp"
#include "GPSCNav2Eph.hpp"
#include "NavHealthData.hpp"
#include "GPSCNav2Health.hpp"
#include "IonoData.hpp"
#include "KlobucharIonoData.hpp"
#include "GPSCNav2Iono.hpp"
#include "TimeOffsetData.hpp"
#include "StdNavTimeOffset.hpp"
#include "GPSCNav2TimeOffset.hpp"
#include "GPSCNavData.hpp"
#include "GPSCNavAlm.hpp"
#include "GPSCNavEph.hpp"
#include "GPSCNavHealth.hpp"
#include "GPSCNavIono.hpp"
#include "GPSCNavRedAlm.hpp"
#include "GPSCNavTimeOffset.hpp"
#include "GPSLNavData.hpp"
#include "GPSLNavAlm.hpp"
#include "GPSLNavEph.hpp"
#include "GPSLNavHealth.hpp"
#include "GPSLNavIono.hpp"
#include "GPSLNavTimeOffset.hpp"
#include "GPSOrbElemStore.hpp"
#include "GSatID.hpp"
#include "GalDataValid.hpp"
#include "GalEphemerisStore.hpp"
#include "GalHealthStatus.hpp"
#include "GalFNavAlm.hpp"
#include "GalFNavEph.hpp"
#include "GalFNavHealth.hpp"
#include "NeQuickIonoData.hpp"
#include "GalFNavIono.hpp"
#include "GalFNavTimeOffset.hpp"
#include "GalINavAlm.hpp"
#include "GalINavEph.hpp"
#include "GalINavHealth.hpp"
#include "GalINavIono.hpp"
#include "GalINavTimeOffset.hpp"
#include "GalileoEllipsoid.hpp"
#include "GalileoIonoEllipsoid.hpp"
#include "GenXSequence.hpp"
#include "GenericNavFilterData.hpp"
#include "GloEphemerisStore.hpp"
#include "GlobalTropModel.hpp"
#include "HelmertTransform.hpp"
 //#include "IERS1996NutationData.hpp"
 //#include "IERS1996UT1mUTCData.hpp"
 //#include "IERS2003NutationData.hpp"
/* #include "IERS2010CIOSeriesData.hpp" */
/* #include "LoopedFramework.hpp" */
/* #include "InOutFramework.hpp" */
#include "IonexBase.hpp"
#include "IonexHeader.hpp"
#include "IonexStream.hpp"
#include "IonexData.hpp"
#include "IonexStore.hpp"
#include "Namelist.hpp"
/* #include "SRIMatrix.hpp" */
/* #include "SparseVector.hpp" */
/* #include "SparseMatrix.hpp" */
/* #include "SRI.hpp" */
/* #include "SRIFilter.hpp" */
/* #include "logstream.hpp" */
#include "KalmanFilter.hpp"
#include "LNavFilterData.hpp"
#include "LNavAlmValFilter.hpp"
#include "LNavCookFilter.hpp"
#include "LNavCrossSourceFilter.hpp"
#include "LNavEmptyFilter.hpp"
#include "LNavEphMaker.hpp"
#include "LNavOrderFilter.hpp"
#include "LNavParityFilter.hpp"
#include "LNavTLMHOWFilter.hpp"
#include "LinearClockModel.hpp"
#include "MODIP.hpp"
#include "MOPSTropModel.hpp"
#include "MatrixImplementation.hpp"
#include "MatrixFunctors.hpp"
#include "MatrixOperators.hpp"
#include "MetReader.hpp"
#include "MostCommonValue.hpp"
#include "NavValidityType.hpp"
#include "NavSearchOrder.hpp"
#include "NavDataFactory.hpp"
#include "NavDataFactoryWithStore.hpp"
#include "NavDataFactoryWithStoreFile.hpp"
#include "MultiFormatNavDataFactory.hpp"
#include "NDFUniqConstIterator.hpp"
#include "NDFUniqIterator.hpp"
#include "NEDUtil.hpp"
#include "NavLibrary.hpp"
#include "NavMsgData.hpp"
#include "NavMsgDataBits.hpp"
#include "NavMsgDataPNB.hpp"
#include "NavMsgDataWords.hpp"
#include "NavOrderFilter.hpp"
#include "NavTimeSystemConverter.hpp"
#include "NeillTropModel.hpp"
#include "OceanLoadTides.hpp"
#include "OceanLoading.hpp"
#include "OrbElem.hpp"
/* #include "OrbAlm.hpp" */
/* #include "OrbAlmFactory.hpp" */
/* #include "OrbAlmGen.hpp" */
/* #include "OrbAlmStore.hpp" */
#include "OrbDataSysFactory.hpp"
#include "OrbDataUTC.hpp"
#include "OrbElemRinex.hpp"
#include "OrbSysGpsC.hpp"
#include "OrbSysGpsC_30.hpp"
#include "OrbSysGpsC_33.hpp"
/* #include "OrbSysGpsC_32.hpp" */
#include "OrbSysGpsL.hpp"
/* #include "OrbSysGpsL_51.hpp" */
#include "OrbSysGpsL_52.hpp"
#include "OrbSysGpsL_55.hpp"
#include "OrbSysGpsL_56.hpp"
/* #include "OrbSysGpsL_63.hpp" */
#include "OrbSysGpsL_Reserved.hpp"
#include "OrbSysStore.hpp"
#include "OrbitDataSP3.hpp"
#include "PNBNavDataFactory.hpp"
#include "PNBGPSCNav2DataFactory.hpp"
#include "PNBGPSCNavDataFactory.hpp"
#include "PNBGPSLNavDataFactory.hpp"
#include "PNBGalFNavDataFactory.hpp"
#include "PNBGalINavDataFactory.hpp"
#include "PNBMultiGNSSNavDataFactory.hpp"
#include "stl_helpers.hpp"
#include "PRSolution.hpp"
#include "PRSolutionLegacy.hpp"
#include "PoleTides.hpp"
#include "PolyFit.hpp"
#include "PosixTime.hpp"
 //#include "PowerSum.hpp"
#include "SolarSystemEphemeris.hpp"
#include "SunEarthSatGeometry.hpp"
#include "SolidEarthTides.hpp"
/* #include "SolarSystem.hpp" */
/* #include "PreciseRange.hpp" */
#include "QZSEphemerisStore.hpp"
#include "RACRotation.hpp"
/* #include "RTFileFrame.hpp" */
#include "RationalizeRinexNav.hpp"
#include "Rinex3ClockData.hpp"
#include "Rinex3ClockStream.hpp"
#include "Rinex3EphemerisStore.hpp"
#include "Rinex3NavFilterOperators.hpp"
/* #include "Rinex3ObsFileLoader.hpp" */
#include "Rinex3ObsFilterOperators.hpp"
#include "Rinex3ObsStream.hpp"
#include "RinexClockBase.hpp"
#include "RinexClockData.hpp"
#include "RinexClockHeader.hpp"
#include "RinexClockStream.hpp"
 //#include "RinexClockStore.hpp"
#include "RinexNavHeader.hpp"
#include "RinexNavStream.hpp"
#include "RinexEphemerisStore.hpp"
#include "RinexMetBase.hpp"
#include "RinexMetHeader.hpp"
#include "RinexMetData.hpp"
#include "RinexMetFilterOperators.hpp"
#include "RinexMetStream.hpp"
#include "RinexNavDataFactory.hpp"
#include "RinexNavFilterOperators.hpp"
#include "RinexObsFilterOperators.hpp"
#include "RinexObsStream.hpp"
#include "RinexTimeOffset.hpp"
#include "SEMBase.hpp"
#include "SEMHeader.hpp"
#include "SEMData.hpp"
#include "SEMStream.hpp"
#include "SEMAlmanacStore.hpp"
#include "SEMNavDataFactory.hpp"
#include "SP3NavDataFactory.hpp"
#include "SP3Stream.hpp"
/* #include "SRIleastSquares.hpp" */
#include "X1Sequence.hpp"
#include "X2Sequence.hpp"
#include "SVPCodeGen.hpp"
#include "SaasTropModel.hpp"
#include "SatDataReader.hpp"
/* #include "SatPassIterator.hpp" */
#include "msecHandler.hpp"
/* #include "SatPassUtilities.hpp" */
#include "SimpleTropModel.hpp"
/* #include "SinexBase.hpp" */
/* #include "SinexHeader.hpp" */
/* #include "SinexTypes.hpp" */
/* #include "SinexBlock.hpp" */
/* #include "SinexData.hpp" */
/* #include "SinexStream.hpp" */
#include "SolarPosition.hpp"
/* #include "SpecialFuncs.hpp" */
#include "StreamBuf.hpp"
#include "TimeCorrection.hpp"
#include "TimeNamedFileStream.hpp"
#include "TimeRange.hpp"
#include "VectorOperators.hpp"
#include "WNJfilter.hpp"
#include "WindowFilter.hpp"
#include "YumaBase.hpp"
#include "YumaHeader.hpp"
#include "YumaData.hpp"
#include "YumaStream.hpp"
#include "YumaAlmanacStore.hpp"
#include "YumaNavDataFactory.hpp"
#include "convhelp.hpp"
#include "expandtilde.hpp"
#include "format.hpp"
/* #include "gdc.hpp" */
#include "ord.hpp"
#include "random.hpp"
#include "singleton.hpp"
