#ifndef COMMON_WDGTMAIN_H
#define COMMON_WDGTMAIN_H

#include "ui_WdgtMain.h"
#include "Client.hpp"
#include <Candy/System/GLSystemQtWindow.h>
#include <Candy.h>
#include <Slimage/Slimage.hpp>
#include <QtGui/QMainWindow>
#include <QtCore/QTimer>
#include <mutex>
#include <string>
#include <memory>

class WdgtMain : public QMainWindow
{
    Q_OBJECT

public:
	WdgtMain(QWidget *parent = 0);
	~WdgtMain();

	void setClient(const std::shared_ptr<Client>& client);

protected:
	void closeEvent(QCloseEvent* event);

public Q_SLOTS:
	void tick();

private:
	void showImageThreadsafe(const std::string& tag, const slimage::Image3ub& img);
	void showImage(const std::string& tag, const slimage::Image3ub& img);
	void render();

private:
	Candy::GLSystemQtWindow* widget_candy_;
	boost::shared_ptr<Candy::Engine> engine_;

	std::map<std::string, slimage::Image3ub> show_images_cache_;
	std::mutex show_images_cache_mutex_;

	QTimer timer_tick_;

	std::shared_ptr<Client> client_;

private:
    Ui::WdgtMainClass ui;
};

#endif
