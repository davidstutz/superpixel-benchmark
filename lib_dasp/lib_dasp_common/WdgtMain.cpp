#include "WdgtMain.h"
#include <Slimage/IO.hpp>
#include <QtGui/QMdiSubWindow>
#include <QtGui/QLabel>
#include <boost/bind.hpp>

void PrepareEngine(const boost::shared_ptr<Candy::Engine>& engine)
{
	// auto view_ = engine->getView();
	auto scene = engine->getScene();

	// boost::shared_ptr<Candy::DirectionalLight> light1(new Candy::DirectionalLight(Danvil::ctLinAlg::Vec3f(+1.0f, +1.0f, -1.0f)));
	// light1->setDiffuse(Danvil::Colorf(1.0f, 1.0f, 1.0f));
	// scene->addLight(light1);
	// boost::shared_ptr<Candy::DirectionalLight> light2(new Candy::DirectionalLight(Danvil::ctLinAlg::Vec3f(-1.0f, -1.0f, -1.0f)));
	// light2->setDiffuse(Danvil::Colorf(1.0f, 1.0f, 1.0f));
	// scene->addLight(light2);

	engine->setClearColor(Danvil::Color::Grey);

	scene->setShowCoordinateCross(true);

}

WdgtMain::WdgtMain(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);

	std::cout << "Preparing OpenGL widgets ..." << std::endl;

	// strands_ = std::make_shared<dasp::tdasp::Controler>();
	// strands_->image_callback_ = boost::bind(&WdgtMain::showImageThreadsafe, this, _1, _2);

	widget_candy_ = new Candy::GLSystemQtWindow(0);
	widget_candy_->setAutoUpdateInterval(20);
	auto w1 = ui.mdiArea->addSubWindow(widget_candy_);
	w1->setWindowTitle("3D global");
	engine_ = widget_candy_->getEngine();
	PrepareEngine(engine_);

	{
		boost::shared_ptr<Candy::IRenderable> renderling(new Candy::ObjectRenderling(
			[this]() {
				this->render();
			}
		));
		engine_->getScene()->addItem(renderling);
	}

	QObject::connect(&timer_tick_, SIGNAL(timeout()), this, SLOT(tick()));
	timer_tick_.setInterval(50);
	timer_tick_.start();

	QObject::connect(ui.actionTile_Subwindows, SIGNAL(triggered()), ui.mdiArea, SLOT(tileSubWindows()));
	QObject::connect(ui.actionCascade_Subwindows, SIGNAL(triggered()), ui.mdiArea, SLOT(cascadeSubWindows()));
}

WdgtMain::~WdgtMain()
{
}

void WdgtMain::setClient(const std::shared_ptr<Client>& client)
{
	client_ = client;
	client_->SetShowImageFunction(
		boost::bind(&WdgtMain::showImageThreadsafe, this, _1, _2));
}

void WdgtMain::closeEvent(QCloseEvent* event)
{
	event->accept();
}

void WdgtMain::tick()
{
	client_->OnTick();

	std::lock_guard<std::mutex> lock(show_images_cache_mutex_);
	if(show_images_cache_.size() == 0)
		return;
	for(auto p : show_images_cache_) {
		showImage(p.first, p.second);
	}
	show_images_cache_.clear();
}

void WdgtMain::showImageThreadsafe(const std::string& tag, const slimage::Image3ub& img)
{
	std::lock_guard<std::mutex> lock(show_images_cache_mutex_);
	show_images_cache_[tag] = img;
}

void WdgtMain::showImage(const std::string& tag, const slimage::Image3ub& img)
{
	// convert to Qt image
	QImage* qimg = slimage::qt::ConvertToQt(img);
	if(qimg == 0) {
		return;
	}
	// prepare subwindow list
	std::map<std::string, QMdiSubWindow*> subwindows_by_tag;
	for(QMdiSubWindow* w : ui.mdiArea->subWindowList()) {
		subwindows_by_tag[w->windowTitle().toStdString()] = w;
	}
	// find label
	auto it = subwindows_by_tag.find(tag);
	// create new if none exists
	QMdiSubWindow* sw;
	QLabel* qlabel;
	if(it != subwindows_by_tag.end()) {
		sw = it->second;
		qlabel = (QLabel*)sw->widget();
	} else {
		qlabel = new QLabel();
		subwindows_by_tag[tag] = sw;
		sw = ui.mdiArea->addSubWindow(qlabel);
		sw->setWindowTitle(QString::fromStdString(tag));
		sw->show();
	}
	// display image in label
	qlabel->setPixmap(QPixmap::fromImage(*qimg));
	// qlabel->setPixmap(QPixmap::fromImage(qimg->copy(30, 25, 640-65, 480-45)));
	qlabel->adjustSize();
	sw->adjustSize();
	// cleanup
	delete qimg;
}

void WdgtMain::render()
{
	client_->OnRender();
}
