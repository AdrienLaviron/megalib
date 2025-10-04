/*
 * MGUIGeneralAlgorithm.h
 *
 * Copyright (C) by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MGUIGeneralAlgorithm__
#define __MGUIGeneralAlgorithm__


////////////////////////////////////////////////////////////////////////////////


// ROOT libs:

// MEGAlib libs:
#include "MGlobal.h"
#include "MGUIDialog.h"
#include "MSettingsEventReconstruction.h"
#include "MGUIERBList.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class MGUIGeneralAlgorithm : public MGUIDialog
{
  // Public Interface:
 public:
  MGUIGeneralAlgorithm(const TGWindow* Parent, const TGWindow* Main,
                  MSettingsEventReconstruction* Data);
  virtual ~MGUIGeneralAlgorithm();

  // protected methods:
 protected:
  virtual void Create();
  virtual bool OnApply();

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  MSettingsEventReconstruction* m_Data;

  TGLayoutHints* m_ListLayout;
  MGUIERBList* m_CoincidenceList;
  MGUIERBList* m_EventClusteringList;
  MGUIERBList* m_HitClusteringList;
  MGUIERBList* m_EventTypeList;
  //MGUIERBList* m_TrackingList;
  //MGUIERBList* m_CSRList;
  //MGUIERBList* m_PairList;
  //MGUIERBList* m_DecayList;



#ifdef ___CLING___
 public:
  ClassDef(MGUIGeneralAlgorithm, 0)
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
