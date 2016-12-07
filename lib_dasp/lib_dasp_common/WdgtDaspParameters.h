#ifndef WDGTDASPPARAMETERS_H
#define WDGTDASPPARAMETERS_H

#include <QtGui/QWidget>
#include "ui_WdgtDaspParameters.h"
#include "Parameters.hpp"
#include <boost/shared_ptr.hpp>

class WdgtDaspParameters : public QWidget
{
    Q_OBJECT

public:
	WdgtDaspParameters(const boost::shared_ptr<dasp::Parameters>& dasp_opt, QWidget *parent = 0);
	~WdgtDaspParameters();

	bool* reload;

	void UpdateActualCountRadius();
	void SetActualCount(unsigned int count);
	void SetActualRadius(float radius);

public Q_SLOTS:
	void ChangeDaspRepairDepth(int state);
	void ChangeDaspSmoothDepth(int state);
	void ChangeDaspSmoothDensity(int state);
	void OnSuperSeedType(int selection);
	void ChangeSuperUseGradientDensity(int state);
	void ChangeSuperpixelSkipBad(int state);
	void ChangeSuperpixelRadius(double val);
	void ChangeSuperpixelCount(int val);
	void ChangeSuperpixelIterations(int val);
	void ChangeSuperpixelWeightSpatial(double val);
	void ChangeSuperpixelWeightColor(double val);
	void OnDaspColorSpace(int selection);
	void ChangeDaspDensityMode(int selection);
	void ChangeSuperpixelWeightNormal(double val);
	void ChangeSuperConquerEnclaves(int val);
	void ChangeRandomSeed(int val);
	void CreateRandomSeed();
	void ChangeSuperpixelCoverage(double val);
	void ChangeClipEnable(int state);
	void ChangeClip();
	void ChangeRoi2DEnable(int state);
	void ChangeRoi2D();

private:
	boost::shared_ptr<dasp::Parameters> dasp_opt_;

private:
    Ui::WdgtDaspParametersClass ui;
};

#endif
