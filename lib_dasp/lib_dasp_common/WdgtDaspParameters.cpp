#include "WdgtDaspParameters.h"
#include <stdlib.h>

WdgtDaspParameters::WdgtDaspParameters(const boost::shared_ptr<dasp::Parameters>& dasp_opt, QWidget *parent)
    : QWidget(parent)
{
	dasp_opt_ = dasp_opt;

	ui.setupUi(this);

	ui.comboBoxSeedType->addItem("Random", dasp::SeedModes::Random);
	ui.comboBoxSeedType->addItem("Grid", dasp::SeedModes::Grid);
	ui.comboBoxSeedType->addItem("SimplifiedPDS_Old", dasp::SeedModes::SimplifiedPDS_Old);
	ui.comboBoxSeedType->addItem("SimplifiedPDS", dasp::SeedModes::SimplifiedPDS);
	ui.comboBoxSeedType->addItem("FloydSteinberg", dasp::SeedModes::FloydSteinberg);
	ui.comboBoxSeedType->addItem("FloydSteinbergExpo", dasp::SeedModes::FloydSteinbergExpo);
	ui.comboBoxSeedType->addItem("FloydSteinbergMultiLayer", dasp::SeedModes::FloydSteinbergMultiLayer);
	ui.comboBoxSeedType->addItem("Fattal", dasp::SeedModes::Fattal);
	ui.comboBoxSeedType->addItem("Delta", dasp::SeedModes::Delta);
	ui.comboBoxSeedType->setCurrentIndex(dasp_opt_->seed_mode);

	ui.comboBoxDaspColorSpace->addItem("RGB", dasp::ColorSpaces::RGB);
	ui.comboBoxDaspColorSpace->addItem("HSV", dasp::ColorSpaces::HSV);
	ui.comboBoxDaspColorSpace->addItem("LAB", dasp::ColorSpaces::LAB);
	ui.comboBoxDaspColorSpace->addItem("HN", dasp::ColorSpaces::HN);
	ui.comboBoxDaspColorSpace->setCurrentIndex(dasp_opt_->color_space);

	ui.comboBoxDensityMode->addItem("ASP_RGB", dasp::DensityModes::ASP_RGB);
	ui.comboBoxDensityMode->addItem("ASP_RGBD", dasp::DensityModes::ASP_RGBD);
	ui.comboBoxDensityMode->addItem("DASP", dasp::DensityModes::DASP);
	ui.comboBoxDensityMode->setCurrentIndex(dasp_opt_->density_mode);

	ui.spinBoxIterations->setValue(dasp_opt_->iterations);
	ui.doubleSpinBoxRadius->setValue(1000.0f*dasp_opt_->base_radius);
	ui.spinBoxSuperCount->setValue(dasp_opt_->count);
	ui.doubleSpinBoxWeightSpatial->setValue(dasp_opt_->weight_spatial);
	ui.doubleSpinBoxWeightColor->setValue(dasp_opt_->weight_color);
	ui.doubleSpinBoxWeightNormal->setValue(dasp_opt_->weight_normal);
	ui.checkBoxDaspRepairDepth->setChecked(dasp_opt_->is_repair_depth);
	ui.checkBoxClipEnable->setChecked(dasp_opt_->enable_clipping);
	ui.doubleSpinBoxClipXMin->setValue(dasp_opt_->clip_x_min);
	ui.doubleSpinBoxClipYMin->setValue(dasp_opt_->clip_y_min);
	ui.doubleSpinBoxClipZMin->setValue(dasp_opt_->clip_z_min);
	ui.doubleSpinBoxClipXMax->setValue(dasp_opt_->clip_x_max);
	ui.doubleSpinBoxClipYMax->setValue(dasp_opt_->clip_y_max);
	ui.doubleSpinBoxClipZMax->setValue(dasp_opt_->clip_z_max);
	ui.doubleSpinBoxCoverage->setValue(dasp_opt_->coverage);
	ui.checkBoxRoiEnable->setChecked(dasp_opt_->enable_roi_2d);
	ui.spinBoxRoiXMin->setValue(dasp_opt_->roi_2d_x_min);
	ui.spinBoxRoiYMin->setValue(dasp_opt_->roi_2d_y_min);
	ui.spinBoxRoiXMax->setValue(dasp_opt_->roi_2d_x_max);
	ui.spinBoxRoiYMax->setValue(dasp_opt_->roi_2d_y_max);

	QObject::connect(ui.checkBoxDaspRepairDepth, SIGNAL(stateChanged(int)), this, SLOT(ChangeDaspRepairDepth(int)));
	QObject::connect(ui.checkBoxDaspSmoothDepth, SIGNAL(stateChanged(int)), this, SLOT(ChangeDaspSmoothDepth(int)));
	QObject::connect(ui.checkBoxDaspSmoothDensity, SIGNAL(stateChanged(int)), this, SLOT(ChangeDaspSmoothDensity(int)));
	QObject::connect(ui.comboBoxSeedType, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSuperSeedType(int)));
	QObject::connect(ui.checkBoxGradientAdaptive, SIGNAL(stateChanged(int)), this, SLOT(ChangeSuperUseGradientDensity(int)));
	QObject::connect(ui.checkBoxSkipBad, SIGNAL(stateChanged(int)), this, SLOT(ChangeSuperpixelSkipBad(int)));
	QObject::connect(ui.doubleSpinBoxRadius, SIGNAL(valueChanged(double)), this, SLOT(ChangeSuperpixelRadius(double)));
	QObject::connect(ui.spinBoxSuperCount, SIGNAL(valueChanged(int)), this, SLOT(ChangeSuperpixelCount(int)));
	QObject::connect(ui.spinBoxIterations, SIGNAL(valueChanged(int)), this, SLOT(ChangeSuperpixelIterations(int)));
	QObject::connect(ui.doubleSpinBoxWeightSpatial, SIGNAL(valueChanged(double)), this, SLOT(ChangeSuperpixelWeightSpatial(double)));
	QObject::connect(ui.doubleSpinBoxWeightColor, SIGNAL(valueChanged(double)), this, SLOT(ChangeSuperpixelWeightColor(double)));
	QObject::connect(ui.doubleSpinBoxWeightNormal, SIGNAL(valueChanged(double)), this, SLOT(ChangeSuperpixelWeightNormal(double)));
	QObject::connect(ui.comboBoxDaspColorSpace, SIGNAL(currentIndexChanged(int)), this, SLOT(OnDaspColorSpace(int)));
	QObject::connect(ui.comboBoxDensityMode, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeDaspDensityMode(int)));
	QObject::connect(ui.doubleSpinBoxCoverage, SIGNAL(valueChanged(double)), this, SLOT(ChangeSuperpixelCoverage(double)));
	QObject::connect(ui.checkBoxDaspConquerEnclaves, SIGNAL(stateChanged(int)), this, SLOT(ChangeSuperConquerEnclaves(int)));
	QObject::connect(ui.spinBoxRandomSeed, SIGNAL(valueChanged(int)), this, SLOT(ChangeRandomSeed(int)));
	QObject::connect(ui.pushButtonRandomSeed, SIGNAL(clicked()), this, SLOT(CreateRandomSeed()));

	QObject::connect(ui.checkBoxClipEnable, SIGNAL(stateChanged(int)), this, SLOT(ChangeClipEnable(int)));
	QObject::connect(ui.doubleSpinBoxClipXMin, SIGNAL(valueChanged(double)), this, SLOT(ChangeClip()));
	QObject::connect(ui.doubleSpinBoxClipYMin, SIGNAL(valueChanged(double)), this, SLOT(ChangeClip()));
	QObject::connect(ui.doubleSpinBoxClipZMin, SIGNAL(valueChanged(double)), this, SLOT(ChangeClip()));
	QObject::connect(ui.doubleSpinBoxClipXMax, SIGNAL(valueChanged(double)), this, SLOT(ChangeClip()));
	QObject::connect(ui.doubleSpinBoxClipYMax, SIGNAL(valueChanged(double)), this, SLOT(ChangeClip()));
	QObject::connect(ui.doubleSpinBoxClipZMax, SIGNAL(valueChanged(double)), this, SLOT(ChangeClip()));

	QObject::connect(ui.checkBoxRoiEnable, SIGNAL(stateChanged(int)), this, SLOT(ChangeRoi2DEnable(int)));
	QObject::connect(ui.spinBoxRoiXMin, SIGNAL(valueChanged(int)), this, SLOT(ChangeRoi2D()));
	QObject::connect(ui.spinBoxRoiYMin, SIGNAL(valueChanged(int)), this, SLOT(ChangeRoi2D()));
	QObject::connect(ui.spinBoxRoiXMax, SIGNAL(valueChanged(int)), this, SLOT(ChangeRoi2D()));
	QObject::connect(ui.spinBoxRoiYMax, SIGNAL(valueChanged(int)), this, SLOT(ChangeRoi2D()));

	// dasp_opt_->is_repair_depth = ui.checkBoxDaspRepairDepth->isChecked();
	dasp_opt_->is_smooth_depth = ui.checkBoxDaspSmoothDepth->isChecked();
	dasp_opt_->is_smooth_density = ui.checkBoxDaspSmoothDensity->isChecked();
	dasp_opt_->seed_mode = (dasp::SeedMode)(ui.comboBoxSeedType->itemData(ui.comboBoxSeedType->currentIndex()).toInt());
	dasp_opt_->gradient_adaptive_density = ui.checkBoxGradientAdaptive->isChecked();
	dasp_opt_->ignore_pixels_with_bad_visibility = ui.checkBoxSkipBad->isChecked();
	// dasp_opt_->base_radius = 0.001f * ui.doubleSpinBoxRadius->value();
	// dasp_opt_->count = ui.spinBoxSuperCount->value();
	// dasp_opt_->iterations = ui.spinBoxIterations->value();
	// dasp_opt_->weight_spatial = ui.doubleSpinBoxWeightSpatial->value();
	// dasp_opt_->weight_color = ui.doubleSpinBoxWeightColor->value();
	// dasp_opt_->weight_normal = ui.doubleSpinBoxWeightNormal->value();
	// dasp_opt_->weight_depth = ui.doubleSpinBoxWeightDepth->value();
	dasp_opt_->color_space = (dasp::ColorSpace)(ui.comboBoxDaspColorSpace->itemData(ui.comboBoxDaspColorSpace->currentIndex()).toInt());
	// dasp_opt_->coverage = ui.doubleSpinBoxCoverage->value();
	dasp_opt_->is_conquer_enclaves = ui.checkBoxDaspConquerEnclaves->isChecked();
	dasp_opt_->random_seed = 0;

	dasp_opt_->enable_clipping = ui.checkBoxClipEnable->isChecked();
	dasp_opt_->clip_x_min = ui.doubleSpinBoxClipXMin->value();
	dasp_opt_->clip_y_min = ui.doubleSpinBoxClipYMin->value();
	dasp_opt_->clip_z_min = ui.doubleSpinBoxClipZMin->value();
	dasp_opt_->clip_x_max = ui.doubleSpinBoxClipXMax->value();
	dasp_opt_->clip_y_max = ui.doubleSpinBoxClipYMax->value();
	dasp_opt_->clip_z_max = ui.doubleSpinBoxClipZMax->value();

	srand(time(0));
}

WdgtDaspParameters::~WdgtDaspParameters()
{

}

void WdgtDaspParameters::UpdateActualCountRadius()
{
	if(dasp_opt_->count == 0) {
		SetActualCount(dasp_opt_->count_actual);
	}
	else {
		SetActualRadius(dasp_opt_->base_radius);
	}
}

void WdgtDaspParameters::SetActualCount(unsigned int count)
{
	bool oldState = ui.spinBoxSuperCount->blockSignals(true);
	ui.spinBoxSuperCount->setValue(count);
	ui.spinBoxSuperCount->blockSignals(oldState);
}

void WdgtDaspParameters::SetActualRadius(float radius)
{
	bool oldState = ui.doubleSpinBoxRadius->blockSignals(true);
	ui.doubleSpinBoxRadius->setValue(radius / 0.001);
	ui.doubleSpinBoxRadius->blockSignals(oldState);
}

void WdgtDaspParameters::ChangeDaspRepairDepth(int state)
{
	dasp_opt_->is_repair_depth = state;
	*reload = true;
}

void WdgtDaspParameters::ChangeDaspSmoothDepth(int state)
{
	dasp_opt_->is_smooth_depth = state;
	*reload = true;
}

void WdgtDaspParameters::ChangeDaspSmoothDensity(int state)
{
	dasp_opt_->is_smooth_density = state;
	*reload = true;
}

void WdgtDaspParameters::OnSuperSeedType(int selection)
{
	dasp_opt_->seed_mode = (dasp::SeedMode)(ui.comboBoxSeedType->itemData(selection).toInt());
	*reload = true;
}

void WdgtDaspParameters::ChangeSuperUseGradientDensity(int state)
{
	dasp_opt_->gradient_adaptive_density = state;
	*reload = true;
}

void WdgtDaspParameters::ChangeSuperpixelSkipBad(int state)
{
	dasp_opt_->ignore_pixels_with_bad_visibility = state;
	*reload = true;
}

void WdgtDaspParameters::ChangeSuperpixelRadius(double val)
{
	dasp_opt_->base_radius = 0.001f * val;
	dasp_opt_->count = 0;
	*reload = true;
}

void WdgtDaspParameters::ChangeSuperpixelCount(int val)
{
	dasp_opt_->count = val;
	*reload = true;
}

void WdgtDaspParameters::ChangeSuperpixelIterations(int val)
{
	dasp_opt_->iterations = val;
	*reload = true;
}

void WdgtDaspParameters::ChangeSuperpixelWeightSpatial(double val)
{
	dasp_opt_->weight_spatial = val;
	*reload = true;
}

void WdgtDaspParameters::ChangeSuperpixelWeightColor(double val)
{
	dasp_opt_->weight_color = val;
	*reload = true;
}

void WdgtDaspParameters::OnDaspColorSpace(int selection)
{
	dasp_opt_->color_space = (dasp::ColorSpace)(ui.comboBoxDaspColorSpace->itemData(selection).toInt());
	*reload = true;
}

void WdgtDaspParameters::ChangeDaspDensityMode(int selection)
{
	dasp_opt_->density_mode = (dasp::DensityMode)(ui.comboBoxDensityMode->itemData(selection).toInt());
	*reload = true;
}

void WdgtDaspParameters::ChangeSuperpixelWeightNormal(double val)
{
	dasp_opt_->weight_normal = val;
	*reload = true;
}

void WdgtDaspParameters::ChangeSuperConquerEnclaves(int val)
{
	dasp_opt_->is_conquer_enclaves = val;
	*reload = true;
}

void WdgtDaspParameters::ChangeRandomSeed(int val)
{
	dasp_opt_->random_seed = val;
	*reload = true;
}

void WdgtDaspParameters::CreateRandomSeed()
{
	int r = rand();
	ui.spinBoxRandomSeed->setValue(r);
	*reload = true;
}

void WdgtDaspParameters::ChangeSuperpixelCoverage(double val)
{
	dasp_opt_->coverage = val;
	*reload = true;
}

void WdgtDaspParameters::ChangeClipEnable(int state)
{
	dasp_opt_->enable_clipping = state;
	*reload = true;
}

void WdgtDaspParameters::ChangeClip()
{
	dasp_opt_->clip_x_min = ui.doubleSpinBoxClipXMin->value();
	dasp_opt_->clip_y_min = ui.doubleSpinBoxClipYMin->value();
	dasp_opt_->clip_z_min = ui.doubleSpinBoxClipZMin->value();
	dasp_opt_->clip_x_max = ui.doubleSpinBoxClipXMax->value();
	dasp_opt_->clip_y_max = ui.doubleSpinBoxClipYMax->value();
	dasp_opt_->clip_z_max = ui.doubleSpinBoxClipZMax->value();
	*reload = true;
}

void WdgtDaspParameters::ChangeRoi2DEnable(int state)
{
	dasp_opt_->enable_roi_2d = state;
	*reload = true;
}

void WdgtDaspParameters::ChangeRoi2D()
{
	dasp_opt_->roi_2d_x_min = ui.spinBoxRoiXMin->value();
	dasp_opt_->roi_2d_x_max = ui.spinBoxRoiXMax->value();
	dasp_opt_->roi_2d_y_min = ui.spinBoxRoiYMin->value();
	dasp_opt_->roi_2d_y_max = ui.spinBoxRoiYMax->value();
	*reload = true;
}

