/* **********************************************************************
 * Copyright (C) 2019-2022, Claude Pruneau, Victor Gonzalez, Sumit Basu
 * All rights reserved.
 *
 * Based on the ROOT package and environment
 *
 * For the licensing terms see LICENSE.
 *
 * Author: Claude Pruneau,   04/01/2022
 *
 * *********************************************************************/
#include "ParticlePairDerivedHistogramCalculator.hpp"

ClassImp(ParticlePairDerivedHistogramCalculator);

ParticlePairDerivedHistogramCalculator::ParticlePairDerivedHistogramCalculator(const TString &          _name,
                                                                               const Configuration    & _configuration,
                                                                               vector<EventFilter*>   & _eventFilters,
                                                                               vector<ParticleFilter*>& _particleFilters,
                                                                               LogLevel _selectedLevel)
:
Task(_name, _configuration, _eventFilters, _particleFilters, _selectedLevel)
{
  appendClassName("ParticlePairDerivedHistogramCalculator");
  setInstanceName(_name);
  setDefaultConfiguration();
  setConfiguration(_configuration);
}

ParticlePairDerivedHistogramCalculator::ParticlePairDerivedHistogramCalculator(const TString &        _name,
                                                                               const Configuration &  _configuration,
                                                                               ParticlePairAnalyzer * _analyzer,
                                                                               LogLevel               _selectedLevel)
:
Task(_name,_configuration,_selectedLevel)
{
  appendClassName("ParticlePairDerivedHistogramCalculator");
  setDefaultConfiguration();
  setConfiguration(_configuration);
  eventFilters    = _analyzer->getEventFilters();
  particleFilters = _analyzer->getParticleFilters();
  baseSingleHistograms = _analyzer->getBaseSingleHistograms();
  basePairHistograms   = _analyzer->getBasePairHistograms();
  setDefaultConfiguration();
  setConfiguration(_configuration);
}

void ParticlePairDerivedHistogramCalculator::setDefaultConfiguration()
{
  if (reportStart("ParticlePairDerivedHistogramCalculator",getName(),"setDefaultConfiguration()"))
    {
    }
  Configuration & configuration = getConfiguration();
  configuration.setName("ParticlePairDerivedHistogramCalculator Configuration");
  configuration.setParameter("useParticles",     true);
  configuration.setParameter("createHistograms", true);
  configuration.setParameter("loadHistograms",   true);
  configuration.setParameter("saveHistograms",   true);
  configuration.setParameter("binCorrPP", 1.0);
    
  configuration.addParameter("fillEta",  true);
  configuration.addParameter("fillY",    false);
  configuration.addParameter("fillP2",   false);
  
  configuration.addParameter("nBins_n1",  100);
  configuration.addParameter("min_n1",    0.0);
  configuration.addParameter("max_n1",  100.0);
  
  configuration.addParameter("nBins_eTot",  100);
  configuration.addParameter("min_eTot",    0.0);
  configuration.addParameter("max_eTot",  100.0);
  
  configuration.addParameter("nBins_pt",   18);
  configuration.addParameter("min_pt",   0.20);
  configuration.addParameter("max_pt",   2.00);
  
  configuration.addParameter("nBins_phi",  72);
  configuration.addParameter("min_phi",   0.0);
  configuration.addParameter("max_phi", TMath::TwoPi());
  
  configuration.addParameter("nBins_eta",   20);
  configuration.addParameter("min_eta",   -1.0);
  configuration.addParameter("max_eta",    1.0);
  
  configuration.addParameter("nBins_y",     20);
  configuration.addParameter("min_y",     -1.0);
  configuration.addParameter("max_y",      1.0);
  
  configuration.addParameter("nBins_phiEta",    720);
  configuration.addParameter("nBins_phiEtaPt",  7200);
  configuration.addParameter("nBins_phiY",      720);
  configuration.addParameter("nBins_phiYPt",    7200);
  
  configuration.addParameter("nBins_n2",          100);
  configuration.addParameter("min_n2",            0.0);
  configuration.addParameter("max_n2",         1000.0);
  
  configuration.addParameter("nBins_DeltaPlong",   10);
  configuration.addParameter("min_DeltaPlong",   -1.0);
  configuration.addParameter("max_DeltaPlong",    1.0);
  
  configuration.addParameter("nBins_DeltaPside",   10);
  configuration.addParameter("min_DeltaPside",   -1.0);
  configuration.addParameter("max_DeltaPside",    1.0);
  configuration.addParameter("range_DeltaPside",  2.0);
  configuration.addParameter("nBins_DeltaPout",    10);
  configuration.addParameter("min_DeltaPout",    -1.0);
  configuration.addParameter("max_DeltaPout",     1.0);
  configuration.addParameter("range_DeltaPout",   2.0);
  
  configuration.addParameter("nBins_Dphi",         36);
  configuration.addParameter("min_Dphi",          0.0);
  configuration.addParameter("max_Dphi",TMath::TwoPi());
  configuration.addParameter("width_Dphi",TMath::TwoPi());
  
  configuration.addParameter("nBins_Dphi_shft",    36);
  configuration.addParameter("min_Dphi_shft",     0.0);
  configuration.addParameter("max_Dphi_shft",     0.0);
  
  configuration.addParameter("nBins_Deta",         39);
  configuration.addParameter("min_Deta",         -2.0);
  configuration.addParameter("max_Deta",          2.0);
  configuration.addParameter("width_Deta",   4.0/39.0);
  
  configuration.addParameter("nBins_Dy",           39);
  configuration.addParameter("min_Dy",           -2.0);
  configuration.addParameter("max_Dy",            2.0);
  configuration.addParameter("width_Dy",     4.0/39.0);
  
  if (reportDebug("ParticlePairDerivedHistogramCalculator",getName(),"setDefaultConfiguration()"))
    {
    configuration.printConfiguration(cout);
    }
}

void ParticlePairDerivedHistogramCalculator::createHistograms()
{
  TString fct = "createHistograms()";
  if (reportStart(fct))
    { }
  derivedSingleHistograms.clear();
  derivedPairHistograms.clear();
  const Configuration & configuration = getConfiguration();
  LogLevel debugLevel = getReportLevel();
  TString bn  = getName();
  bn += "_";
  unsigned int nEventFilters    = eventFilters.size();
  unsigned int nParticleFilters = particleFilters.size();
  
  Histograms * histos;
  if (reportInfo(fct))
    {
    cout << endl;
    cout << "       Creating Histograms: " << bn << endl;
    cout << "             nEventFilters: " << nEventFilters << endl;
    cout << "          nParticleFilters: " << nParticleFilters << endl;
    cout << endl;
    }
  for (unsigned int iEventFilter=0; iEventFilter<nEventFilters; iEventFilter++ )
    {
    TString efn = eventFilters[iEventFilter]->getName();
    if (reportDebug(fct))
      {
      cout << "Event filter:" << efn << endl;
      }
    // singles
    for (unsigned int iParticleFilter1=0; iParticleFilter1<nParticleFilters; iParticleFilter1++ )
      {
      TString pfn1 = particleFilters[iParticleFilter1]->getName();
      if (reportDebug(fct))
        {
        cout << "Particle with filter: " << pfn1 << endl;
        }
      histos = new ParticleDerivedHistos(createHistogramName(bn,efn,pfn1),configuration,debugLevel);
      histos->createHistograms();
      derivedSingleHistograms.push_back(histos);
      }
    // pairs
    for (unsigned int iParticleFilter1=0; iParticleFilter1<nParticleFilters; iParticleFilter1++ )
      {
      TString pfn1 = particleFilters[iParticleFilter1]->getName();
      for (unsigned int iParticleFilter2=0; iParticleFilter2<nParticleFilters; iParticleFilter2++ )
        {
        TString pfn2 = particleFilters[iParticleFilter2]->getName();
        if (reportDebug(fct))
          {
          cout << "Particle pairs with filter: " << pfn1 << " & " << pfn2 << endl;
          }
        histos = new ParticlePairDerivedHistos(createPairHistogramName(bn,efn,pfn1,pfn2),configuration,debugLevel);
        histos->createHistograms();
        derivedPairHistograms.push_back(histos);
        }
      }
    }
  if (reportEnd(fct))
    { }
}

void ParticlePairDerivedHistogramCalculator::loadHistograms(TFile * inputFile)
{
  TString fct = "loadHistograms(TFile * inputFile)";
  if (reportStart(fct))
    { }
  if (!ptrFileExist(fct,inputFile)) return;
  
  baseSingleHistograms.clear();
  basePairHistograms.clear();
  const Configuration & configuration = getConfiguration();
  LogLevel debugLevel = getReportLevel();
  
  TString bn  = getName();
  bn += "_";
  unsigned int nEventFilters    = eventFilters.size();
  unsigned int nParticleFilters = particleFilters.size();
  if (reportDebug(fct))
    {
    cout << endl;
    cout << "       Loading Histograms: " << bn << endl;
    cout << "             nEventFilters: " << nEventFilters << endl;
    cout << "          nParticleFilters: " << nParticleFilters << endl;
    cout << endl;
    }
  baseSingleHistograms.clear();
  basePairHistograms.clear();
  for (unsigned int iEventFilter=0; iEventFilter<nEventFilters; iEventFilter++ )
    {
    TString efn = eventFilters[iEventFilter]->getName();
    if (reportDebug(fct))
      {
      cout << "Event filter:" << efn << endl;
      }
    // singles
    for (unsigned int iParticleFilter1=0; iParticleFilter1<nParticleFilters; iParticleFilter1++ )
      {
      TString pfn1 = particleFilters[iParticleFilter1]->getName();
      if (reportDebug(fct))
        {
        cout << "Particle with filter: " << pfn1 << endl;
        }
      ParticleHistos * histos = new ParticleHistos(createHistogramName(bn,efn,pfn1),configuration,debugLevel);
      histos->loadHistograms(inputFile);
      baseSingleHistograms.push_back(histos);
      }
    // pairs
    for (unsigned int iParticleFilter1=0; iParticleFilter1<nParticleFilters; iParticleFilter1++ )
      {
      TString pfn1 = particleFilters[iParticleFilter1]->getName();
      for (unsigned int iParticleFilter2=0; iParticleFilter2<nParticleFilters; iParticleFilter2++ )
        {
        TString pfn2 = particleFilters[iParticleFilter2]->getName();
        if (reportDebug(fct))
          {
          cout << "Particle pairs with filter: " << pfn1 << " & " << pfn2 << endl;
          }
        ParticlePairHistos * histos = new ParticlePairHistos(createPairHistogramName(bn,efn,pfn1,pfn2),configuration,debugLevel);
        histos->loadHistograms(inputFile);
        basePairHistograms.push_back(histos);
        }
      }
    }
  if (reportEnd(fct))
    { }
}

//!
//! Calculate Derived Histograms
//!
// ====================================
void ParticlePairDerivedHistogramCalculator::execute()
{
  TString fct = "execute()";
  if (reportStart(fct))
    { }
  const Configuration & config  = getConfiguration();
  unsigned int nEventFilters    = eventFilters.size();
  unsigned int nParticleFilters = particleFilters.size();
  if (reportInfo(fct))
    {
    cout << endl;
    cout << "Computing derived histograms for: " << endl;
    cout << "                   nEventFilters: " << nEventFilters << endl;
    cout << "                nParticleFilters: " << nParticleFilters << endl;
    cout << "                   nSingleHistos: " << getNBaseSingleHistograms() << endl;
    cout << "                     nPairHistos: " << getNBasePairHistograms() << endl;
    cout << "                  nDerivedHistos: " << getNDerivedPairHistograms() << endl;
    }
  ParticleHistos        * bSingleHistos1;
  ParticleHistos        * bSingleHistos2;
  ParticlePairHistos    * bPairHistos;
  ParticleDerivedHistos * dSingleHistos1;
  ParticleDerivedHistos * dSingleHistos2;
  ParticlePairDerivedHistos * dPairHistos;
  
  for (unsigned int iEventFilter=0; iEventFilter<nEventFilters; iEventFilter++ )
    {
    if (reportDebug(fct)) cout << "  iEventFilter:" << iEventFilter << endl;
    unsigned int index;
    unsigned int baseSingle        = iEventFilter*nParticleFilters;
    unsigned int basePair          = iEventFilter*nParticleFilters*nParticleFilters;
    unsigned int baseDerivedPair   = basePair;
    
    //! Calculate derived spectra of singles
    for (unsigned int iParticleFilter1=0; iParticleFilter1<nParticleFilters; iParticleFilter1++)
      {
      TString pfn1 = particleFilters[iParticleFilter1]->getName();
      index = baseSingle+iParticleFilter1;
      if (reportDebug(fct))   cout << " (1) iParticleFilter1:" << iParticleFilter1 << " named " << pfn1 << " with index:" << index << endl;
      bSingleHistos1 = (ParticleHistos *) baseSingleHistograms[index];
      if (reportDebug(fct))   cout << " (1a) iParticleFilter1:" << iParticleFilter1 << " named " << pfn1 << " with index:" << index << endl;
      dSingleHistos1 = (ParticleDerivedHistos *) derivedSingleHistograms[index];
      if (reportDebug(fct))   cout << " (2) iParticleFilter1:" << iParticleFilter1 << " named " << pfn1 << " with index:" << index << endl;
      dSingleHistos1->calculateDerivedHistograms(bSingleHistos1);
      if (reportDebug(fct))   cout << " (3) iParticleFilter1:" << iParticleFilter1 << " named " << pfn1 << " with index:" << index << endl;
      }
    
    //! Calculate derived spectra of pairs
    for (unsigned int iParticleFilter1=0; iParticleFilter1<nParticleFilters; iParticleFilter1++)
      {
      TString pfn1 = particleFilters[iParticleFilter1]->getName();
      index = baseSingle+iParticleFilter1;
      bSingleHistos1 = (ParticleHistos *) baseSingleHistograms[index];
      dSingleHistos1 = (ParticleDerivedHistos *) derivedSingleHistograms[index];
      
      for (unsigned int iParticleFilter2=0; iParticleFilter2<nParticleFilters; iParticleFilter2++)
        {
        TString pfn2 = particleFilters[iParticleFilter2]->getName();
        if (reportDebug(fct))
          cout << "  iParticleFilter1:" << iParticleFilter1 << " named " << pfn1 << ";  iParticleFilter2:" << iParticleFilter2<< " named " << pfn2  << endl;
        index = baseSingle+iParticleFilter2;
        bSingleHistos2 = (ParticleHistos *) baseSingleHistograms[index];
        dSingleHistos2 = (ParticleDerivedHistos *) derivedSingleHistograms[index];
        
        index = basePair+iParticleFilter1*nParticleFilters+iParticleFilter2;
        bPairHistos = (ParticlePairHistos *) basePairHistograms[index];
        dPairHistos = (ParticlePairDerivedHistos *) derivedPairHistograms[index];
        dPairHistos->calculatePairDerivedHistograms(*bSingleHistos1,*bSingleHistos2,*dSingleHistos1,*dSingleHistos2,*bPairHistos,
                                                    configuration.getValueDouble("binCorrPP") );
        }
      }
    }
  if (reportEnd(fct))
    { }
}
